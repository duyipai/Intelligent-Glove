#ifndef STATES_H
#define STATES_H
enum STAGE
{
    STATIONARY = 0,
    TRANSLATION,
    ROTATION
};

class States
{
   public:

    void set_Rotation(double * angular_velocity);
    void set_Translation(double * accleration);
    void set_Stationary();
    double linear_dot(double * tmp);
    double angular_dot(double * tmp);

    double a[3] = {0.0};
    double acc;
    double roll, pitch, yaw;
    double w[3] = {0.0};
    double angular_speed;
    int stationaryCount = 0;
    double t = 0.0;
    STAGE stage = STATIONARY;
};

#endif
