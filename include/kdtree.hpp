#ifndef W_DUGS_NBODY_SIM_KDTREE_HPP
#define W_DUGS_NBODY_SIM_KDTREE_HPP

#include <array>
#include <memory>
#include <vector>

#include "body.hpp"
#include "physics.hpp"

class KDTree {
public:
    KDTree(const std::vector<Body>& bodies, double softening);

    void computeAccelerations(
        const std::vector<Body>& bodies,
        double gravitational_constant,
        double theta,
        std::vector<Acceleration>& accelerations) const;

private:
    struct Node {
        // Spatial boundaries to calculate the widt for the theta approximation
        double min_x;
        double max_x;
        double min_y;
        double max_y;

        // Physics mass properties
        double total_mass;
        double center_of_mass_x;
        double center_of_mass_y;

        int body_index; // -1 if this is an internal center of mass node
        int split_axis; // 0 for x axis cut, 1 for y axis cut

        // A k-d tree only has 2 children (left/right or up/down)
        std::array<std::unique_ptr<Node>, 2> children;

        Node(double x_min, double x_max, double y_min, double y_max, int axis);

        bool isLeaf() const;
    } ;

    std::unique_ptr<Node> root_;

    double softening_;

    //Recursive static helpers
    static std::unique_ptr<Node> buildTree(std::vector<int>& indices, const std::vector<Body>& bodies, int depth);
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