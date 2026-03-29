#include <chrono>
#include <fstream>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>

#include "simulation.hpp"
#include "kdtree.hpp"
#include "body.hpp"

namespace {

Algorithm parseAlgorithm(const std::string& value) {
    if (value == "brute" || value == "bruteforce" || value == "brute_force") {
        return Algorithm::BruteForce;
    }
    if (value == "bh" || value == "barneshut" || value == "barnes_hut") {
        return Algorithm::BarnesHut;
    }

    if (value == "kd" || value == "kdtree"  || value == "kd_tree"){
      return Algorithm::KDTree;
    }

    throw std::invalid_argument("Unknown algorithm: " + value);
}

std::size_t parseSize(const char* value, const std::string& label) {
    try {
        return static_cast<std::size_t>(std::stoull(value));
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid " + label + ": " + value);
    }
}

unsigned int parseSeed(const char* value) {
    try {
        return static_cast<unsigned int>(std::stoul(value));
    } catch (const std::exception&) {
        throw std::invalid_argument("Invalid seed: " + std::string(value));
    }
}

SimulationConfig defaultConfig(Algorithm algorithm) {
    SimulationConfig config;
    config.gravitational_constant = 1.0;
    config.softening = 0.1;
    config.time_step = 0.01;
    config.theta = 0.6;
    config.algorithm = algorithm;
    return config;
}

void writeBenchmarkHeader(const std::string& path) {
    std::ofstream output(path.c_str(), std::ios::trunc);
    output << "n,algorithm,steps,time_seconds\n";
}

void appendBenchmarkRow(
    const std::string& path,
    std::size_t n,
    Algorithm algorithm,
    std::size_t steps,
    double time_seconds) {
    std::ofstream output(path.c_str(), std::ios::app);
    output << n << "," << algorithmName(algorithm) << "," << steps << "," << time_seconds << "\n";
}

void runSimulationCommand(int argc, char** argv) {
    const Algorithm algorithm = argc > 2 ? parseAlgorithm(argv[2]) : Algorithm::BruteForce;
    const std::size_t body_count = argc > 3 ? parseSize(argv[3], "body count") : 1000;
    const std::size_t steps = argc > 4 ? parseSize(argv[4], "step count") : 500;
    const std::size_t snapshot_interval = argc > 5 ? parseSize(argv[5], "snapshot interval") : 25;
    const std::string output_path = argc > 6 ? argv[6] : "simulation.csv";
    const std::string bodies_path = argc > 7 ? argv[7] : "";

    std::vector<Body> bodies =
        bodies_path.empty() ? makeRandomBodies(body_count, 42U) : loadBodiesFromCsv(bodies_path);

    if (bodies.size() != body_count) {
        throw std::invalid_argument(
            "Body count does not match dataset size. Requested " + std::to_string(body_count) +
            " but loaded " + std::to_string(bodies.size()) + ".");
    }

    Simulation simulation(std::move(bodies), defaultConfig(algorithm));
    simulation.runWithSnapshots(steps, snapshot_interval, output_path);

    std::cout << "Wrote trajectory snapshots to " << output_path << " using "
              << algorithmName(algorithm) << " with " << body_count << " bodies";
    if (!bodies_path.empty()) {
        std::cout << " from " << bodies_path;
    }
    std::cout << ".\n";
}

void runGenerateCommand(int argc, char** argv) {
    const std::size_t body_count = argc > 2 ? parseSize(argv[2], "body count") : 100000;
    const std::string output_path = argc > 3 ? argv[3] : "bodies_100k.csv";
    const unsigned int seed = argc > 4 ? parseSeed(argv[4]) : 42U;

    const std::vector<Body> bodies = makeRandomBodies(body_count, seed);
    writeBodiesToCsv(bodies, output_path);

    std::cout << "Generated " << body_count << " reusable bodies at " << output_path
              << " with seed " << seed << ".\n";
}

void runBenchmarkCommand(int argc, char** argv) {
    const Algorithm algorithm = argc > 2 ? parseAlgorithm(argv[2]) : Algorithm::BruteForce;
    const std::size_t steps = argc > 3 ? parseSize(argv[3], "step count") : 10;
    const std::string output_path = argc > 4 ? argv[4] : "benchmark.csv";

    const std::vector<std::size_t> counts =
        (algorithm == Algorithm::BarnesHut || algorithm == Algorithm::KDTree)
            ? std::vector<std::size_t>{100, 1000, 10000, 100000}
            : std::vector<std::size_t>{100, 1000, 10000};

    writeBenchmarkHeader(output_path);

    for (std::size_t count : counts) {
        Simulation simulation(makeRandomBodies(count, 42U), defaultConfig(algorithm));
        const auto start = std::chrono::steady_clock::now();
        simulation.run(steps);
        const auto end = std::chrono::steady_clock::now();
        const std::chrono::duration<double> elapsed = end - start;

        appendBenchmarkRow(output_path, count, algorithm, steps, elapsed.count());
        std::cout << "Benchmarked " << count << " bodies with " << algorithmName(algorithm)
                  << " in " << elapsed.count() << " seconds.\n";
    }
}

void printUsage() {
    std::cout
        << "Usage:\n"
        << "  ./nbody_sim generate [body_count] [output_csv] [seed]\n"
        << "  ./nbody_sim simulate [brute_force|barnes_hut] [body_count] [steps] [snapshot_interval] [output_csv] [bodies_csv]\n"
        << "  ./nbody_sim benchmark [brute_force|barnes_hut] [steps] [output_csv]\n";
}

}  // namespace

int main(int argc, char** argv) {
    try {
        if (argc < 2) {
            printUsage();
            return 0;
        }

        const std::string command = argv[1];
        if (command == "generate") {
            runGenerateCommand(argc, argv);
            return 0;
        }
        if (command == "simulate") {
            runSimulationCommand(argc, argv);
            return 0;
        }
        if (command == "benchmark") {
            runBenchmarkCommand(argc, argv);
            return 0;
        }

        printUsage();
        return 1;
    } catch (const std::exception& error) {
        std::cerr << "Error: " << error.what() << "\n";
        return 1;
    }
}