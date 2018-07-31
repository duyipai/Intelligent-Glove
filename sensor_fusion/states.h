#ifndef STATES_H
#define STATES_H
enum STAGE
{
    STATIONARY = 0,
    TRANSLATION,
    ROTATION
};

struct States
{
    double v[3] = {0.0};
    double roll, pitch, yaw;
    double w[3] = {0.0};
    int stationaryCount = 0;
    double t = 0.0;
    double angular_speed;
    STAGE stage = STATIONARY;
};

#endif
