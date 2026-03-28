#ifndef W_DUGS_NBODY_SIM_PHYSICS_HPP
#define W_DUGS_NBODY_SIM_PHYSICS_HPP

#include <vector>

#include "body.hpp"

struct Acceleration {
    double ax;
    double ay;
};

Acceleration makeZeroAcceleration();

void computeBruteForceAccelerations(
    const std::vector<Body>& bodies,
    double gravitational_constant,
    double softening,
    std::vector<Acceleration>& accelerations);

#endif
