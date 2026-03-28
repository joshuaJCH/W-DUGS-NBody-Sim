#include "physics.hpp"

#include <cmath>

namespace {

    double softenedDistanceSquared(double dx, double dy, double softening) {
        return dx * dx + dy * dy + softening * softening;
    }

}  // namespace

Acceleration makeZeroAcceleration() {
    return {0.0, 0.0};
}

void computeBruteForceAccelerations(
    const std::vector<Body>& bodies,
    double gravitational_constant,
    double softening,
    std::vector<Acceleration>& accelerations) {
    accelerations.assign(bodies.size(), makeZeroAcceleration());

    for (std::size_t i = 0; i < bodies.size(); ++i) {
        for (std::size_t j = i + 1; j < bodies.size(); ++j) {
            const double dx = bodies[j].x - bodies[i].x;
            const double dy = bodies[j].y - bodies[i].y;
            const double distance_squared = softenedDistanceSquared(dx, dy, softening);
            const double inverse_distance = 1.0 / std::sqrt(distance_squared);
            const double inverse_distance_cubed = inverse_distance * inverse_distance * inverse_distance;
            const double scale = gravitational_constant * inverse_distance_cubed;

            accelerations[i].ax += scale * bodies[j].mass * dx;
            accelerations[i].ay += scale * bodies[j].mass * dy;
            accelerations[j].ax -= scale * bodies[i].mass * dx;
            accelerations[j].ay -= scale * bodies[i].mass * dy;
        }
    }
}