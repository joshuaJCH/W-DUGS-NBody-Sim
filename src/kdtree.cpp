#include "kdtree.hpp"
#include <algorithm>
#include <limits>
#include "math.h"

KDTree::Node::Node(double x_min, double x_max, double y_min, double y_max, int axis)
    : min_x(x_min), max_x(x_max), min_y(y_min), max_y(y_max),
      total_mass(0.0), center_of_mass_x(0.0), center_of_mass_y(0.0),
      body_index(-1), split_axis(axis) {}

bool KDTree::Node::isLeaf() const { return !children[0] && !children[1]; }


KDTree::KDTree(const std::vector<Body>& bodies, double softening)
    : softening_(softening) {

    if (bodies.empty()) {
        root_ = nullptr;
        return;
    }


    std::vector<int> indices(bodies.size());
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        indices[i] = static_cast<int>(i);
    }

    root_ = buildTree(indices, bodies, 0);

    if (root_) {
        updateMassProperties(*root_, bodies);
    }
}


std::unique_ptr<KDTree::Node> KDTree::buildTree(std::vector<int>& indices, const std::vector<Body>& bodies, int depth) {
    if (indices.empty()) {
        return nullptr;
    }

    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();

    for (int idx : indices) {
        const Body& b = bodies[idx];
        min_x = std::min(min_x, b.x);
        max_x = std::max(max_x, b.x);
        min_y = std::min(min_y, b.y);
        max_y = std::max(max_y, b.y);
    }

    int axis = depth % 2;
    auto node = std::unique_ptr<Node>(new Node(min_x, max_x, min_y, max_y, axis));

    if (indices.size() == 1) {
        node->body_index = indices[0];
        return node;

    }

    if (axis == 0) {
        std::sort(indices.begin(), indices.end(), [&bodies](int a, int b) {
            return bodies[a].x < bodies[b].x;
        });

    } else {
        std::sort(indices.begin(), indices.end(), [&bodies](int a, int b) {
            return bodies[a].y < bodies[b].y;
        });
    }

    std::size_t median = indices.size() / 2;
    std::vector<int> left_indices(indices.begin(), indices.begin() + median);
    std::vector<int> right_indices(indices.begin() + median, indices.end());

    node->children[0] = buildTree(left_indices, bodies, depth + 1);

    node->children[1] = buildTree(right_indices, bodies, depth + 1);

    return node;
}


void KDTree::updateMassProperties(Node& node, const std::vector<Body>& bodies) {
    if (node.isLeaf()) {
        if (node.body_index >= 0) {
            const Body& body = bodies[static_cast<std::size_t>(node.body_index)];
            node.total_mass = body.mass;
            node.center_of_mass_x = body.x;
            node.center_of_mass_y = body.y;
        }
        return;
    }

    double total_mass = 0.0;
    double weighted_x = 0.0;
    double weighted_y = 0.0;

    for (const std::unique_ptr<Node>& child : node.children) {
        if (!child) continue;

        updateMassProperties(*child, bodies);
        total_mass += child->total_mass;
        weighted_x += child->center_of_mass_x * child->total_mass;
        weighted_y += child->center_of_mass_y * child->total_mass;
    }

    node.total_mass = total_mass;
    if (total_mass > 0.0) {
        node.center_of_mass_x = weighted_x / total_mass;
        node.center_of_mass_y = weighted_y / total_mass;
    }
}


void KDTree::accumulateAcceleration(
    const Node* node,
    const std::vector<Body>& bodies,
    int body_index,
    double gravitational_constant,
    double theta,
    double softening,
    Acceleration& acceleration) {

    if (!node || node->total_mass == 0.0) {
        return;
    }

    if (node->isLeaf() && node->body_index == body_index) {
        return;

    }

    const Body& body = bodies[static_cast<std::size_t>(body_index)];
    const double dx = node->center_of_mass_x - body.x;
    const double dy = node->center_of_mass_y - body.y;
    const double distance_squared = dx * dx + dy * dy + softening * softening;
    const double distance = std::sqrt(distance_squared);

    if (distance == 0.0) {
        return;
    }

    const double node_width = std::max(node->max_x - node->min_x, node->max_y - node->min_y);

    if (node->isLeaf() || (node_width / distance) < theta) {
        const double inverse_distance_cubed = 1.0 / (distance_squared * distance);
        const double scale = gravitational_constant * node->total_mass * inverse_distance_cubed;
        acceleration.ax += scale * dx;
        acceleration.ay += scale * dy;
        return;
    }

    for (const std::unique_ptr<Node>& child : node->children) {
        accumulateAcceleration(
            child.get(),
            bodies,
            body_index,
            gravitational_constant,
            theta,
            softening,
            acceleration);
    }

}

void KDTree::computeAccelerations(
    const std::vector<Body>& bodies,
    double gravitational_constant,
    double theta,
    std::vector<Acceleration>& accelerations) const {

    accelerations.assign(bodies.size(), makeZeroAcceleration());

    if (!root_) { return; }

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        accumulateAcceleration(
            root_.get(),
            bodies,

            static_cast<int>(i),
            gravitational_constant,
            theta,
            softening_,
            accelerations[i]);
    }
}