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

    void reset_Rotation();
    void set_Rotation(double * angular_velocity);
    void set_Translation(double * accleration);
    void set_Stationary();
    double linear_dot(double * tmp) const;
    double angular_dot(double * tmp) const;

    double a[3] = {0.0};
    double roll, pitch, yaw;
    double w[3] = {0.0}; // notice that we always take the first component to be positive because typical working requeires rotation back-and-forth
    double accumulated_angular_speed=0.0;
    double accumulated_acc=0.0;
    int stationaryCount = 0;
    int translation_count = 0;
    double global_time = 0.0;
    STAGE stage = STATIONARY;
    double temperature;
    double humidity;
    double pressure;
};

#endif
