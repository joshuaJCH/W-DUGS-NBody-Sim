#include "body.hpp"

Body::Body(double x, double y, double vx, double vy) : x(x), y(y), vx(vx), vy(vy), ax(0), ay(0) {}

void Body::update(double dt) {
    /* WHAT IS DT?
     * 'dt' is the small slice of real world time
     * that passed since the last frame.
     * It is the step size of the simulation basically, and we use 0.01 because its convenient

     * WHY THIS FORMULA? (euler's mehtod)
     * In calculus, v = dx/dt. In code, we rearrange that to dx = v * dt

     * Since we cant do infinitesimal time, we use the smallest measured
     * slice available (the frame time) to approximate continuous motion
     */

    // DYNAMICS: update velocity
    // Same idea as above, but with velocity now, since velocity is the derivative of
    vx = vx + ax * dt;
    vy = vy + ay * dt;

    // KINEMATICS: update position
    // Basically, add to the component (x or y), the direction multiplied by the time they went in that direction
    x = x + vx * dt;
    y = y + vy * dt;

    //MIGHT NEED TO SWAP THE VELOCITY AND POSITION ORDER
}