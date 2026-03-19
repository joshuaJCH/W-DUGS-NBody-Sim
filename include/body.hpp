#ifndef W_DUGS_NBODY_SIM_BODY_HPP
#define W_DUGS_NBODY_SIM_BODY_HPP

class Body {
public:
    // position
    double x;
    double y;

    // velocity
    double vx;
    double vy;

    // acceleration
    double ax;
    double ay;

    Body(double x, double y, double vx, double vy);  //constructor

    void update(double dt); //function for thge=e next position
};

#endif