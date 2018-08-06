#include "message_processor.h"
#include "attitude_estimator.h"
#include "math.h"
#include "states.h"
#include <iostream>
#define DEFAULT_g0 9.81
#define M_PI 3.1415926535898
#define RESET_VECILOTY_THRESHOLD 2
#define TRANSLATION_COUNT_THRESHOLD 2
#define ACC_THRESHOLD 1.0
#define ANGULAR_UPPER_THRESHOLD 1.0
#define ANGULAR_LOWER_THRESOHLD 0.2

static bool ready_for_next_stage = true; // if at stationary, ready to accept next stage, changed in stage_analysis

States process_message(std::istringstream &message_stream)
{
    static States states;
    static stateestimation::AttitudeEstimator Est;
    Est.setPIGains(2.20, 4.65, 10.0, 1.25);

    double globalAcc[3];
    double w[3];
    double a[3];
    double m[3];
    double t;

    message_stream >> t;
    t /= 1000;
    if (fabs(t - states.global_time) > 0.7)
    {
        states.global_time = t;
        Est.reset();
        for (int i = 0; i < 3; ++i)
        {
            states.set_Stationary();
        }
        throw "status reseted";
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
        a[i] /= -1000.0;
    }
    Est.update(t - states.global_time, w[0], w[1], w[2], a[0], a[1], a[2], m[0], m[1], m[2]);
    // update rotationl states
    w[0] = (Est.eulerRoll() - states.roll);
    while (w[0] > 2 * M_PI - 0.7)
    {
        w[0] -= 2 * M_PI;
    }
    while (w[0] < -2 * M_PI + 0.7)
    {
        w[0] += 2 * M_PI;
    }
    w[1] = (Est.eulerPitch() - states.pitch);
    while (w[1] > M_PI - 0.3)
    {
        w[1] -= M_PI;
    }
    while (w[1] < -M_PI + 0.3)
    {
        w[1] += M_PI;
    }
    w[2] = (Est.eulerYaw() - states.yaw);
    while (w[2] > 2 * M_PI - 0.7)
    {
        w[2] -= 2 * M_PI;
    }
    while (w[2] < -2 * M_PI + 0.7)
    {
        w[2] += 2 * M_PI;
    }
    states.yaw = Est.eulerYaw();
    states.pitch = Est.eulerPitch();
    states.roll = Est.eulerRoll();
    double angular_speed = sqrt(pow(w[0], 2) + pow(w[1], 2) + pow(w[2], 2));

    double cosPsi = cos(Est.eulerYaw());
    double sinPsi = sin(Est.eulerYaw());
    double cosTheta = cos(Est.eulerPitch());
    double sinTheta = sin(Est.eulerPitch());
    double cosPhi = cos(Est.eulerRoll());
    double sinPhi = sin(Est.eulerRoll());
    globalAcc[0] = cosTheta * cosPsi * a[0] + (sinPhi * sinTheta * cosPsi - cosPhi * sinPsi) * a[1] + (cosPhi * sinTheta * cosPsi + sinPhi * sinPsi) * a[2];
    globalAcc[1] = cosTheta * sinPsi * a[0] + (sinPhi * sinTheta * sinPsi + cosPhi * cosPsi) * a[1] + (cosPhi * sinTheta * sinPsi - sinPhi * cosPsi) * a[2];
    globalAcc[2] = -sinTheta * a[0] + sinPhi * cosTheta * a[1] + cosPhi * cosTheta * a[2] - 1.025;

    double chopAcc[3];
    for (int i = 0; i < 3; ++i)
    {
        globalAcc[i] *= DEFAULT_g0;
        chopAcc[i] = globalAcc[i];
        if (fabs(chopAcc[i]) < ACC_THRESHOLD)
        {
            chopAcc[i] = 0.0;
        }
    }

    if (states.stage == STATIONARY)
    {
        if (angular_speed > ANGULAR_UPPER_THRESHOLD)
        {
            states.reset_Rotation();
            states.set_Rotation(w);
        }
        else if (fabs(chopAcc[0]) + fabs(chopAcc[1]) + fabs(chopAcc[2]) != 0.0)
        {
            states.set_Translation(globalAcc);
        }
    }
    else if (states.stage == TRANSLATION)
    {
        if (angular_speed > ANGULAR_UPPER_THRESHOLD)
        {
            states.reset_Rotation();
            states.set_Rotation(w);
        }
        else if (fabs(chopAcc[0]) + fabs(chopAcc[1]) + fabs(chopAcc[2]) == 0.0)
        {
            states.stationaryCount++;
            if (states.stationaryCount >= RESET_VECILOTY_THRESHOLD)
            {
                states.set_Stationary();
            }
        }
        else
        {
            states.set_Translation(globalAcc);
        }
    }
    else // at rotation
    {
        if (angular_speed < ANGULAR_LOWER_THRESOHLD)
        {
            if (fabs(chopAcc[0]) + fabs(chopAcc[1]) + fabs(chopAcc[2]) == 0.0)
            {
                states.set_Stationary();
            }
        }
        else
        {
            states.set_Rotation(w);
        }
    }
    states.global_time = t;

    return states;
}

std::ostringstream stage_analysis(States &states, std::deque<States> &states_queue, bool learning_mode)
{
    std::ostringstream return_val;
    extern std::string STAGE_OUTPUT[3];
    static States prev_States;
    return_val << "roll: " << states.roll << " pitch: " << states.pitch << " yaw: " << states.yaw << STAGE_OUTPUT[states.stage] << std::endl;
    if (learning_mode)
    {
        if (ready_for_next_stage && states.stage != STATIONARY)
        {
            states_queue.push_back(states);
            return_val << STAGE_OUTPUT[states.stage] << " added. Stage size: " << states_queue.size() << std::endl;
        }
        else if (states.stage == ROTATION && states_queue.back().stage == ROTATION)
        {
            states_queue.pop_back();
            states_queue.push_back(states);
        }
        else if (states.stage == TRANSLATION && states_queue.back().stage == TRANSLATION)
        {
            states.translation_count = (states_queue.back().translation_count)+1;
            states_queue.pop_back();
            states_queue.push_back(states);
        }
    }
    else
    {
        if (states_queue.empty())
        {
            return_val << "finished all the steps";
        }
        else if (states_queue.front().stage == ROTATION)
        {
            if (states.stage == ROTATION && states.angular_dot(states_queue.front().w) > 0.65)
            {
                return_val << "rotation accepted" << std::endl;
                states_queue.pop_front();
            }
            else
            {
                return_val << "rotation needed, direction ";
                return_val << " wx: " << states_queue.front().w[0] << " wy: " << states_queue.front().w[1] << " wz: " << states_queue.front().w[2];
            }
        }
        else
        {
            if (states.stage == TRANSLATION && states.linear_dot(states_queue.front().a) > 0.65)
            {
                return_val << "translation accepted" << std::endl;
                states_queue.pop_front();
            }
            else
            {
                return_val << "translation needed, direction ";
                return_val << " ax: " << states_queue.front().a[0] << " ay: " << states_queue.front().a[1] << " az: " << states_queue.front().a[2];
            }
        }
        while(!states_queue.empty() && states_queue.front().stage == TRANSLATION && states_queue.front().translation_count < TRANSLATION_COUNT_THRESHOLD)
        {
            states_queue.pop_front();
        }
    }

    if (states.stage == STATIONARY)
    {
        ready_for_next_stage = true;
    }
    else if (states.stage == ROTATION && !states_queue.empty() && prev_States.stage == TRANSLATION)
    {
        if (learning_mode)
            states_queue.pop_back();
        ready_for_next_stage = true;
    }
    else
    {
        ready_for_next_stage = false;
    }
    prev_States = states;
    return return_val;
}