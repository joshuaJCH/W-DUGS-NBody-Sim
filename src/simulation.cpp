#include "../include/simulation.hpp"

#include <fstream>
#include <iomanip>
#include <sstream>
#include <random>
#include <stdexcept>

#include "../include/quadtree.hpp"
#include "../include/kdtree.hpp"

Simulation::Simulation(std::vector<Body> initial_bodies, const SimulationConfig& config)
    : bodies_(std::move(initial_bodies)),
      config_(config),
      accelerations_(bodies_.size(), makeZeroAcceleration()) {}

void Simulation::step() {
    computeAccelerations();
    integrateEuler();
}

void Simulation::run(std::size_t steps) {
    for (std::size_t step_index = 0; step_index < steps; ++step_index) {
        step();
    }
}

void Simulation::runWithSnapshots(std::size_t steps, std::size_t snapshot_interval, const std::string& output_path) {
    if (snapshot_interval == 0) {
        throw std::invalid_argument("snapshot_interval must be greater than zero");
    }

    writeSnapshot(0, 0, output_path, false);
    for (std::size_t step_index = 1; step_index <= steps; ++step_index) {
        step();
        if (step_index % snapshot_interval == 0 || step_index == steps) {
            writeSnapshot(step_index, 0, output_path, true);
        }
    }
}

const std::vector<Body>& Simulation::bodies() const {
    return bodies_;
}

void Simulation::computeAccelerations() {
    if (config_.algorithm == Algorithm::BarnesHut) {
        QuadTree tree(bodies_, config_.softening);
        tree.computeAccelerations(bodies_, config_.gravitational_constant, config_.theta, accelerations_);
        return;
    }

    if (config_.algorithm == Algorithm::KDTree){
        KDTree tree(bodies_, config_.softening);
        tree.computeAccelerations(bodies_, config_.gravitational_constant, config_.theta, accelerations_);
        return;
    }

    computeBruteForceAccelerations(
        bodies_,
        config_.gravitational_constant,
        config_.softening,
        accelerations_);
}

void Simulation::integrateEuler() {
    for (std::size_t i = 0; i < bodies_.size(); ++i) {
        bodies_[i].vx += accelerations_[i].ax * config_.time_step;
        bodies_[i].vy += accelerations_[i].ay * config_.time_step;
        bodies_[i].x += bodies_[i].vx * config_.time_step;
        bodies_[i].y += bodies_[i].vy * config_.time_step;
    }
}

void Simulation::writeSnapshot(
    std::size_t step,
    std::size_t body_offset,
    const std::string& output_path,
    bool append) const {
    std::ofstream output;
    if (append) {
        output.open(output_path.c_str(), std::ios::app);
    } else {
        output.open(output_path.c_str(), std::ios::trunc);
        output << "step,body_id,x,y,vx,vy,mass\n";
    }

    output << std::fixed << std::setprecision(6);
    for (std::size_t i = 0; i < bodies_.size(); ++i) {
        const Body& body = bodies_[i];
        output << step << "," << (body_offset + i) << "," << body.x << "," << body.y
               << "," << body.vx << "," << body.vy << "," << body.mass << "\n";
    }
}

std::vector<Body> makeRandomBodies(std::size_t count, unsigned int seed) {
    std::mt19937 generator(seed);
    std::uniform_real_distribution<double> position_distribution(-100.0, 100.0);
    std::uniform_real_distribution<double> velocity_distribution(-0.25, 0.25);
    std::uniform_real_distribution<double> mass_distribution(0.5, 10.0);

    std::vector<Body> bodies;
    bodies.reserve(count);

    for (std::size_t i = 0; i < count; ++i) {
        bodies.emplace_back(
            position_distribution(generator),
            position_distribution(generator),
            velocity_distribution(generator),
            velocity_distribution(generator),
            mass_distribution(generator));
    }

    return bodies;
}

void writeBodiesToCsv(const std::vector<Body>& bodies, const std::string& output_path) {
    std::ofstream output(output_path.c_str(), std::ios::trunc);
    if (!output) {
        throw std::runtime_error("Could not open output file: " + output_path);
    }

    output << "body_id,x,y,vx,vy,mass\n";
    output << std::fixed << std::setprecision(6);
    for (std::size_t i = 0; i < bodies.size(); ++i) {
        const Body& body = bodies[i];
        output << i << "," << body.x << "," << body.y << "," << body.vx << ","
               << body.vy << "," << body.mass << "\n";
    }
}

std::vector<Body> loadBodiesFromCsv(const std::string& input_path) {
    std::ifstream input(input_path.c_str());
    if (!input) {
        throw std::runtime_error("Could not open body dataset: " + input_path);
    }

    std::string line;
    if (!std::getline(input, line)) {
        throw std::runtime_error("Body dataset is empty: " + input_path);
    }

    std::vector<Body> bodies;
    while (std::getline(input, line)) {
        if (line.empty()) {
            continue;
        }

        std::stringstream row(line);
        std::string cell;
        std::vector<std::string> cells;
        while (std::getline(row, cell, ',')) {
            cells.push_back(cell);
        }

        if (cells.size() != 6) {
            throw std::runtime_error("Invalid row in body dataset: " + line);
        }

        bodies.emplace_back(
            std::stod(cells[1]),
            std::stod(cells[2]),
            std::stod(cells[3]),
            std::stod(cells[4]),
            std::stod(cells[5]));
    }

    if (bodies.empty()) {
        throw std::runtime_error("No bodies were loaded from dataset: " + input_path);
    }

    return bodies;
}

std::string algorithmName(Algorithm algorithm) {
    switch (algorithm) {
        case Algorithm::BruteForce:
            return "brute_force";
        case Algorithm::BarnesHut:
            return "barnes_hut";
        case Algorithm::KDTree:
            return "kd_tree";
        default:
            return "unknown";
    }
}
