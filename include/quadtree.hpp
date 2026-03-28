#ifndef W_DUGS_NBODY_SIM_QUADTREE_HPP
#define W_DUGS_NBODY_SIM_QUADTREE_HPP

#include <array>
#include <memory>
#include <vector>

#include "body.hpp"
#include "physics.hpp"

class QuadTree {
public:
    QuadTree(const std::vector<Body>& bodies, double softening);

    void computeAccelerations(
        const std::vector<Body>& bodies,
        double gravitational_constant,
        double theta,
        std::vector<Acceleration>& accelerations) const;

private:
    struct Node {
        double center_x;
        double center_y;
        double half_size;
        double total_mass;
        double center_of_mass_x;
        double center_of_mass_y;
        int body_index;
        std::array<std::unique_ptr<Node>, 4> children;

        Node(double node_center_x, double node_center_y, double node_half_size);

        bool isLeaf() const;
        bool contains(const Body& body) const;
    };

    std::unique_ptr<Node> root_;
    double softening_;

    static std::unique_ptr<Node> buildRoot(const std::vector<Body>& bodies);
    static void insertBody(Node& node, const std::vector<Body>& bodies, int body_index);
    static void subdivide(Node& node);
    static int childIndex(const Node& node, const Body& body);
    static void updateMassProperties(Node& node, const std::vector<Body>& bodies);
    static void accumulateAcceleration(
        const Node* node,
        const std::vector<Body>& bodies,
        int body_index,
        double gravitational_constant,
        double theta,
        double softening,
        Acceleration& acceleration);
};

#endif
