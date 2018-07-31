#include "message_processor.h"
#include "attitude_estimator.h"
#include "math.h"
#define DEFAULT_g0 9.81
#define M_PI 3.1415926535898
#define RESET_VECILOTY_COUNTER 2
#define ACC_THRESHOLD 1.3
#define ANGULAR_UPPER_THRESHOLD 0.6
#define ANGULAR_LOWER_THRESOHLD 0.4


States process_message(std::istringstream &message_stream)
{
    static States states;
    static stateestimation::AttitudeEstimator Est;
    static double prevAcc[3] = {0.0};

	double globalAcc[3];
    double w[3];
    double a[3];
    double m[3];
    double t;

    message_stream >> t;
    t /= 1000;
    if (fabs(t - states.t) > 0.7)
    {
        throw "status reseted";
        states.t = t;
        Est.reset();
        for (int i = 0; i < 3; ++i)
        {
            states.v[i] = 0.0;
            prevAcc[i] = 0.0;
        }
        return states;
    }
    message_stream >> a[0];
    message_stream >> a[1];
    message_stream >> a[2];
    message_stream >> w[0];
    message_stream >> w[1];
    message_stream >> w[2];
    message_stream >> m[0];
    message_stream >> m[1];
    message_stream >> m[2];
    for (int i = 0; i < 3; ++i)
    {
        w[i] /= 1000.0 * 180 / M_PI;
        a[i] /= -1000.0 / DEFAULT_g0;
    }
    Est.update(t - states.t, w[0], w[1], w[2], a[0], a[1], a[2], m[0], m[1], m[2]);
    // update rotationl states
    states.w[0] = (Est.eulerRoll() - states.roll);
    while (states.w[0] > 2 * M_PI - 0.5)
    {
        states.w[0] -= 2 * M_PI;
    }
    while (states.w[0] < -2 * M_PI + 0.5)
    {
        states.w[0] += 2 * M_PI;
    }
    states.w[1] = (Est.eulerPitch() - states.pitch);
    while (states.w[1] > M_PI - 0.2)
    {
        states.w[1] -= M_PI;
    }
    while (states.w[1] < -M_PI + 0.2)
    {
        states.w[1] += M_PI;
    }
    states.w[2] = (Est.eulerYaw() - states.yaw);
    while (states.w[2] > 2 * M_PI - 0.2)
    {
        states.w[2] -= 2 * M_PI;
    }
    while (states.w[2] < -2 * M_PI + 0.2)
    {
        states.w[2] += 2 * M_PI;
    }
    states.yaw = Est.eulerYaw();
    states.pitch = Est.eulerPitch();
    states.roll = Est.eulerRoll();
    states.angular_speed = sqrt(pow(states.w[0], 2) + pow(states.w[1], 2) + pow(states.w[2], 2));

    double cosPsi = cos(Est.eulerYaw());
    double sinPsi = sin(Est.eulerYaw());
    double cosTheta = cos(Est.eulerPitch());
    double sinTheta = sin(Est.eulerPitch());
    double cosPhi = cos(Est.eulerRoll());
    double sinPhi = sin(Est.eulerRoll());
    globalAcc[0] = cosTheta * cosPsi * a[0] + (sinPhi * sinTheta * cosPsi - cosPhi * sinPsi) * a[1] + (cosPhi * sinTheta * cosPsi + sinPhi * sinPsi) * a[2];
    globalAcc[1] = cosTheta * sinPsi * a[0] + (sinPhi * sinTheta * sinPsi + cosPhi * cosPsi) * a[1] + (cosPhi * sinTheta * sinPsi - sinPhi * cosPsi) * a[2];
    globalAcc[2] = -sinTheta * a[0] + sinPhi * cosTheta * a[1] + cosPhi * cosTheta * a[2] - DEFAULT_g0;

    // update linear states
    for (int i = 0; i < 3; ++i)
    {
        if (fabs(globalAcc[i]) < ACC_THRESHOLD)
        {
            globalAcc[i] = 0.0;
        }
        states.v[i] += (prevAcc[i] + globalAcc[i]) * (t - states.t) / 2.0;
        prevAcc[i] = globalAcc[i];
    }
    if (states.stage == STATIONARY)
    {
        if (states.angular_speed > ANGULAR_UPPER_THRESHOLD)
        {
            states.stage = ROTATION;
        }
        else if (fabs(globalAcc[0]) + fabs(globalAcc[1]) + fabs(globalAcc[2]) != 0.0)
        {
            states.stage = TRANSLATION;
            states.stationaryCount = 0;
        }
    }
    else if (states.stage == TRANSLATION)
    {
        if (states.angular_speed > ANGULAR_UPPER_THRESHOLD)
        {
            states.stage = ROTATION;
        }
        else if (fabs(globalAcc[0]) + fabs(globalAcc[1]) + fabs(globalAcc[2]) == 0.0)
        {
            states.stationaryCount++;
            if (states.stationaryCount >= RESET_VECILOTY_COUNTER)
            {

                states.stage = STATIONARY;
                for (int i = 0; i < 3; ++i)
                {
                    states.v[i] = 0.0;
                }
            }
        }
    }
    else // at rotation
    {
        if (states.angular_speed < ANGULAR_LOWER_THRESOHLD)
        {
            if (fabs(globalAcc[0]) + fabs(globalAcc[1]) + fabs(globalAcc[2]) == 0.0)
            {
                states.stage = STATIONARY;
            }
            else
            {
                states.stage = TRANSLATION;
            }
        }
    }
    states.t = t;

    return states;
}