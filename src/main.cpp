#include <iostream>
#include <chrono>
#include <vector>
#include <string>
#include "simulation.hpp"
#include "body.hpp"


void timePhysicsEngine(Algorithm algo, const std::string& name, const std::vector<Body>& bodies) {
    SimulationConfig config;
    config.gravitational_constant = 1.0;
    config.softening = 0.1;
    config.time_step = 0.01;
    config.theta = 0.5;
    config.algorithm = algo;

    Simulation sim(bodies, config);

    std::cout << "Running " << name << " physics engine... ";

    auto start = std::chrono::high_resolution_clock::now();
    sim.run(10);
    auto end = std::chrono::high_resolution_clock::now();

    std::chrono::duration<double, std::milli> duration = end - start;
    std::cout << duration.count() << " milliseconds.\n";
}

int main() {
    std::cout << "Loading 100k dataset...\n";
    std::vector<Body> test_bodies = loadBodiesFromCsv("../100k_data");

    std::cout << "\n--- Physics Calculation Benchmark (10 time steps) ---\n";

    // COMPARISON RUN
    // timePhysicsEngine(Algorithm::BruteForce, "Brute Force", test_bodies);
    timePhysicsEngine(Algorithm::BarnesHut, "QuadTree", test_bodies);
    timePhysicsEngine(Algorithm::KDTree, "k-d Tree", test_bodies);

    // TO RUN THE SIMULATION: UNCOMMENT THESE LINES BELOW BUT COMMENT THE ONES ABOVE

    // SIMULATION RUN
    // std::cout << "\nGenerating Animation Data for Warrick...\n";
    // SimulationConfig video_config{1.0, 0.1, 0.01, 0.5, Algorithm::BarnesHut};
    // Simulation(test_bodies, video_config).runWithSnapshots(100, 5, "demo_output.csv");

    std::cout << "---------------------------------------------------\n";
    return 0;
}