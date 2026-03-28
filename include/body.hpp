#ifndef W_DUGS_NBODY_SIM_BODY_HPP
#define W_DUGS_NBODY_SIM_BODY_HPP

struct Body {
    double x;
    double y;
    double vx;
    double vy;
    double mass;

    Body();
    Body(double x_pos, double y_pos, double x_vel, double y_vel, double body_mass);
};

#endif
