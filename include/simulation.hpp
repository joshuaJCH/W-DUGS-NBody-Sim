#ifndef W_DUGS_NBODY_SIM_SIMULATION_HPP
#define W_DUGS_NBODY_SIM_SIMULATION_HPP

#include <cstddef>
#include <string>
#include <vector>

#include "body.hpp"
#include "physics.hpp"
#include "kdtree.hpp"

enum class Algorithm {
    BruteForce,
    BarnesHut,
    KDTree
};

struct SimulationConfig {
    double gravitational_constant;
    double softening;
    double time_step;
    double theta;
    Algorithm algorithm;
};

class Simulation {
public:
    Simulation(std::vector<Body> initial_bodies, const SimulationConfig& config);

    void step();
    void run(std::size_t steps);
    void runWithSnapshots(std::size_t steps, std::size_t snapshot_interval, const std::string& output_path);

    const std::vector<Body>& bodies() const;

private:
    std::vector<Body> bodies_;
    SimulationConfig config_;
    std::vector<Acceleration> accelerations_;

    void computeAccelerations();
    void integrateEuler();
    void writeSnapshot(std::size_t step, std::size_t body_offset, const std::string& output_path, bool append) const;
};

std::vector<Body> makeRandomBodies(std::size_t count, unsigned int seed);
void writeBodiesToCsv(const std::vector<Body>& bodies, const std::string& output_path);
std::vector<Body> loadBodiesFromCsv(const std::string& input_path);
std::string algorithmName(Algorithm algorithm);

#endif
