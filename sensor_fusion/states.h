#ifndef STATES_H
#define STATES_H
#include <string>
enum STAGE
{
    STATIONARY = 0,
    TRANSLATION,
    ROTATION

};

std::string STAGE_OUTPUT[3]={"stationary", "translation", "rotation"};

struct States
{
    double v[3] = {0.0};
    double roll, pitch, yaw;
    double w[3] = {0.0};
    int stationaryCount = 0;
    double t = 0.0;
    STAGE stage = STATIONARY;
};

#endif
