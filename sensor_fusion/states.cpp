#include <string>
#include "states.h"
#include "math.h"
std::string STAGE_OUTPUT[3] = {"stationary", "translation", "rotation"};

void States::set_Rotation(double *angular_velocity)
{
    this->stage = ROTATION;
    this->angular_speed = sqrt(pow(angular_velocity[0], 2) + pow(angular_velocity[1], 2) + pow(angular_velocity[2], 2));
    ;
    for (int i = 0; i < 3; ++i)
    {
        this->w[i] = w[i] / this->angular_speed;
    }
    return;
}

void States::set_Translation(double *accleration)
{
    this->stage = TRANSLATION;
    this->acc = sqrt(pow(accleration[0], 2) + pow(accleration[1], 2) + pow(accleration[2], 2));
    this->stationaryCount = 0;
    for (int i = 0; i < 3; ++i)
    {
        this->a[i] = accleration[i] / this->acc;
    }
    return;
}

void States::set_Stationary()
{
    this->stage = STATIONARY;
    return;
}

double States::linear_dot(double * tmp)
{
    return a[0]*tmp[0] + a[1]*tmp[1] + a[2]*tmp[2];
}

double States::angular_dot(double * tmp)
{
    return w[0]*tmp[0] + w[1]*tmp[1] + w[2]*tmp[2];
}