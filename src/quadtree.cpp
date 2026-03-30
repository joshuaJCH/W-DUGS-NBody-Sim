#include "../include/quadtree.hpp"

#include <algorithm>
#include <cmath>
#include <limits>

QuadTree::Node::Node(double node_center_x, double node_center_y, double node_half_size)
    : center_x(node_center_x),
      center_y(node_center_y),
      half_size(node_half_size),
      total_mass(0.0),
      center_of_mass_x(node_center_x),
      center_of_mass_y(node_center_y),
      body_index(-1) {}

bool QuadTree::Node::isLeaf() const {
    return !children[0] && !children[1] && !children[2] && !children[3];
}

bool QuadTree::Node::contains(const Body& body) const {
    return body.x >= center_x - half_size && body.x <= center_x + half_size &&
           body.y >= center_y - half_size && body.y <= center_y + half_size;
}

QuadTree::QuadTree(const std::vector<Body>& bodies, double softening)
    : root_(buildRoot(bodies)), softening_(softening) {
    if (!root_) {
        return;
    }

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        insertBody(*root_, bodies, static_cast<int>(i));
    }

    updateMassProperties(*root_, bodies);
}

void QuadTree::computeAccelerations(
    const std::vector<Body>& bodies,
    double gravitational_constant,
    double theta,
    std::vector<Acceleration>& accelerations) const {
    accelerations.assign(bodies.size(), makeZeroAcceleration());

    if (!root_) {
        return;
    }

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

std::unique_ptr<QuadTree::Node> QuadTree::buildRoot(const std::vector<Body>& bodies) {
    if (bodies.empty()) {
        return nullptr;
    }

    double min_x = std::numeric_limits<double>::max();
    double max_x = std::numeric_limits<double>::lowest();
    double min_y = std::numeric_limits<double>::max();
    double max_y = std::numeric_limits<double>::lowest();

    for (const Body& body : bodies) {
        min_x = std::min(min_x, body.x);
        max_x = std::max(max_x, body.x);
        min_y = std::min(min_y, body.y);
        max_y = std::max(max_y, body.y);
    }

    const double width = std::max(max_x - min_x, max_y - min_y);
    const double half_size = std::max(1.0, width * 0.5 + 1.0);
    const double center_x = (min_x + max_x) * 0.5;
    const double center_y = (min_y + max_y) * 0.5;

    return std::unique_ptr<Node>(new Node(center_x, center_y, half_size));
}

void QuadTree::insertBody(Node& node, const std::vector<Body>& bodies, int body_index) {
    const Body& body = bodies[body_index];
    if (!node.contains(body)) {
        return;
    }

    if (node.isLeaf() && node.body_index == -1) {
        node.body_index = body_index;
        return;
    }

    if (node.isLeaf()) {
        if (node.half_size < 1e-5){return;} //to fix the infinite division problkemem

        const int existing_body_index = node.body_index;
        node.body_index = -1;
        subdivide(node);
        insertBody(*node.children[childIndex(node, bodies[existing_body_index])], bodies, existing_body_index);
    }

    insertBody(*node.children[childIndex(node, body)], bodies, body_index);
}

void QuadTree::subdivide(Node& node) {
    const double child_half_size = node.half_size * 0.5;

    node.children[0] = std::unique_ptr<Node>(new Node(node.center_x - child_half_size, node.center_y + child_half_size, child_half_size));
    node.children[1] = std::unique_ptr<Node>(new Node(node.center_x + child_half_size, node.center_y + child_half_size, child_half_size));
    node.children[2] = std::unique_ptr<Node>(new Node(node.center_x - child_half_size, node.center_y - child_half_size, child_half_size));
    node.children[3] = std::unique_ptr<Node>(new Node(node.center_x + child_half_size, node.center_y - child_half_size, child_half_size));
}

int QuadTree::childIndex(const Node& node, const Body& body) {
    const bool east = body.x >= node.center_x;
    const bool north = body.y >= node.center_y;

    if (north) {
        return east ? 1 : 0;
    }

    return east ? 3 : 2;
}

void QuadTree::updateMassProperties(Node& node, const std::vector<Body>& bodies) {
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
        if (!child) {
            continue;
        }

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

void QuadTree::accumulateAcceleration(
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

    const double node_width = node->half_size * 2.0;
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
