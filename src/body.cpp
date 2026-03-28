#include "body.hpp"

Body::Body() : x(0.0), y(0.0), vx(0.0), vy(0.0), mass(1.0) {}

Body::Body(double x_pos, double y_pos, double x_vel, double y_vel, double body_mass)
    : x(x_pos), y(y_pos), vx(x_vel), vy(y_vel), mass(body_mass) {}
