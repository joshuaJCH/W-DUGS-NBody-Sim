#include "body.hpp"
#include <cmath>
#include <cstdlib>

/*            m1 * m2
 * F = G * ( -------- ) * r
 *             |r^3|
 *
 * This is the formula for the gravitational force between 2 objects.
 * We use this for the physics engine so each body knows what force is acting on it, so then it can move accordingly.
 *
 * F  == FORCE
 * G  == 1     (it's actually a veryy small number irl, so we use 1 here so it's simple)
 * m1 == mass of body 1
 * m2 == mass of body 2
 * r  == distance between body1 and body2
 *
 *
 */

void calculateForce(Body& body1, Body& body2) {
    //get masses and their product
    double mass1 = body1.mass;
    double mass2 = body2.mass;
    double mass_product = mass1 * mass2;

    //set Gravitational constant
    int G = 1;

    //calculate distance
    double safeNum = 0.001; // using this so that we dont divide by 0 on accident
    double delta_x = -(body1.x - body2.x);
    double delta_y = -(body1.y - body2.y);
    double r = std::sqrt( (delta_x * delta_x) + (delta_y * delta_y) + (safeNum * safeNum) );

    //calculate force
    double force_x = (G * mass_product *  r) * 1/(std::abs(r*r*r)) * (delta_x / r);
    double force_y = (G * mass_product *  r) * 1/(std::abs(r*r*r)) * (delta_y / r);

    // f = ma
    // a = f/m
    body1.ax += force_x / body1.mass;
    body1.ay += force_y / body1.mass;
}