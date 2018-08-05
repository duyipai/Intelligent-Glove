#include <string>
#include "states.h"
#include "math.h"
std::string STAGE_OUTPUT[3] = {"stationary", "translation", "rotation"};

void States::reset_Rotation()
{
    accumulated_angular_speed = 0.0;
}

void States::set_Rotation(double * angular_velocity)
{
    this->stage = ROTATION;
    if (angular_velocity[0] < 0.0) // take the opposite of the direction, see description of States::w
    {
        for(int i = 0; i < 3; ++i)
        {
            angular_velocity[i] *= -1;
        }
    }
    for (int i = 0; i < 3; ++i)
    {
        this->w[i] *= this->accumulated_angular_speed;
        this->w[i] += angular_velocity[i];
    } // weighted average of the angular velocity direction

    this->accumulated_angular_speed = sqrt(pow(w[0], 2) + pow(w[1], 2) + pow(w[2], 2));
    for (int i = 0; i < 3; ++i)
    {
        this->w[i] /= accumulated_angular_speed;
    }

    return;
}

void States::set_Translation(double *accleration)
{
    this->stationaryCount = 0;
    this->stage = TRANSLATION;

    if(this->linear_dot(accleration)*accumulated_acc >= 0.0)// need to update acc
    {
        
        for (int i = 0; i < 3; ++i)
        {
            this->a[i] *= accumulated_acc;
            this->a[i] += accleration[i];
        }

        accumulated_acc = sqrt(pow(a[0], 2) + pow(a[1], 2) + pow(a[2], 2));
        for (int i = 0; i < 3; ++i)
        {
            this->a[i] /= accumulated_acc;
        }
    }
    return;
}

void States::set_Stationary()
{
    this->reset_Rotation();
    this->accumulated_acc = 0.0;
    this->stage = STATIONARY;
    return;
}

double States::linear_dot(double * tmp) const
{
    return a[0]*tmp[0] + a[1]*tmp[1] + a[2]*tmp[2];
}

double States::angular_dot(double * tmp) const
{
    return w[0]*tmp[0] + w[1]*tmp[1] + w[2]*tmp[2];
}