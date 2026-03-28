#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include "../include/physics.hpp"
#include "../include/body.hpp"

// =========================================================
// NOTES:
// - run 'main' for the simulation
// - run 'tests' for testing (the other things named "Catch/Catch2WithMain" are junk, just ignore)
// =========================================================



int main() { // Where the whole simulation will happen with loops and the action of the data structures

    //brute force loop to draw
    Body body1(1,1,2,2);
    Body body2(1,3,2,2);
    Body body3(1,6,3,2);
    Body body4(3,1,2,1);
    Body body5(6,1,1,2);
    Body body6(1,1,2,1);

    std::vector<Body> bodies ={body1, body2, body3, body4, body5, body6};

    std::vector<Body> sim_bodies;
    std::srand(std::time(0)); // seed the random number generator
    for(int i = 0 ; i < 1000 ; i++) {
        sim_bodies.push_back(Body(rand() % 100, rand() % 100, rand() % 5, rand() % 5));
    }


    std::ofstream outFile("simulation.csv");
    if(!outFile) {
        std::cerr << "Error opening output file!" << std::endl;
        return 1;
    }
    
    outFile << "frame,body_id,x,y,vx,vy,mass" << std::endl;

    //loop that makes all the frames
    for (int i = 0 ; i < 1000 ; i++) {

        for (auto& body : bodies) { //reset accelerations so they dont grow out of control
            body.ax = 0;
            body.ay = 0;
        }

        for(auto& body : sim_bodies) { //reset accelerations so they dont grow out of control
            body.ax = 0;
            body.ay = 0;
        }

        //brute force it
        for (int x = 0 ; x < bodies.size() ; x++) {
            for (int y = 0 ; y < bodies.size() ; y++) {
                if (x == y){continue;}
                calculateForce(bodies[x], bodies[y]);
            }
        }

        for (int x = 0 ; x < sim_bodies.size() ; x++) { //simulation bodies
            for (int y = 0 ; y < sim_bodies.size() ; y++) {
                if (x == y){continue;}
                calculateForce(sim_bodies[x], sim_bodies[y]);
            }
        }

        int id = 0;
        for (int j = 0 ; j < bodies.size() ; j++) {
            bodies[j].update(0.01);
            outFile << i << "," << j << "," << bodies[j].x << "," << bodies[j].y << "," << bodies[j].vx << "," << bodies[j].vy << "," << bodies[j].mass << std::endl; //write the frame number, and the x,y coords of each body
            id++;
        }
        
        for (int j = 0 ; j < sim_bodies.size() ; j++) {
            sim_bodies[j].update(0.01);
            outFile << i << "," << j << "," << sim_bodies[j].x << "," << sim_bodies[j].y << "," << sim_bodies[j].vx << "," << sim_bodies[j].vy << "," << sim_bodies[j].mass << std::endl; //write the frame number, and the x,y coords of each body
        }
    }

    outFile.close();
    return 0;
}