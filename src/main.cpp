#include <iostream>
#include <chrono>
#include <vector>
#include <algorithm>
#include <fstream>
#include "physics.cpp"
#include "body.hpp"

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


    std::ofstream outFile("simulation.csv");
    outFile << "frame,x,y" << std::endl;

    //loop that makes all the frames
    for (int i = 0 ; i < 1000 ; i++) {

        for (auto& body : bodies) { //reset accelerations so they dont grow out of control
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

        for (auto& body : bodies) {
            body.update(0.01);
            outFile << i << "," << body.x << "," << body.y << std::endl; //write the frame number, and the x,y coords of each body
        }
    }

    return 0;
}