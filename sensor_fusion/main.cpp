#include <iostream>
// #include "Kalman.h"
#include "attitude_estimator.h"
#include <fstream>
#include <string>
#include <sstream>
#include "math.h"
#define PI 3.1415926535898
#define SAMPLING_TIME 0.01
#define axBias 9.7522
#define ayBias -20.1861
#define azBias -21.4348
#define wxBias -385.5581
#define wyBias -172.6621
#define wzBias -7.0748
#define DEFAULT_g0 9.81
int main(int argc, char **argv)
{
    if (argc != 2)
    {
        std::cout << "please specify input file" << std::endl;
    }
    std::ifstream in_file;
    in_file.open(argv[1]);
    std::ofstream out_file;
    out_file.open("pose.txt");
    stateestimation::AttitudeEstimator Est;

    std::string data_line;
    double v[3] = {0.0};
    double globalAcc[3];
    double prevAcc[3] = {0.0};
    while (getline(in_file, data_line))
    {
        std::istringstream is(data_line);
        double w[3];
        double a[3];
        double m[3];
        is >> a[0];
        is >> a[1];
        is >> a[2];
        is >> w[0];
        is >> w[1];
        is >> w[2];
        is >> m[0];
        is >> m[1];
        is >> m[2];

        // a[0] -= axBias;
        // a[1] -= ayBias;
        // a[2] -= azBias;
        // w[0] -= wxBias;
        // w[1] -= wyBias;
        // w[2] -= wzBias;
        for (int i = 0; i < 3; ++i)
        {
            w[i] /= 1000.0 * 180 / PI;
            a[i] /= -1000.0/DEFAULT_g0;
        }
        
        // kalman.updateIMU(w, a, SAMPLING_TIME);
        // out_file << kalman.getRoll();
        // out_file << "\t" << kalman.getPitch();
        // out_file << "\t" << kalman.getYaw();

        // float cosPsi = cos(kalman.getYaw());
        // float sinPsi = sin(kalman.getYaw());
        // float cosTheta = cos(kalman.getPitch());
        // float sinTheta = sin(kalman.getPitch());
        // float cosPhi = cos(kalman.getRoll());
        // float sinPhi = sin(kalman.getRoll());
        // kalman.getNGAcc(localAcc);

        Est.update(SAMPLING_TIME, w[0], w[1], w[2], a[0], a[1], a[2], m[0], m[1], m[2]);
        out_file << Est.eulerRoll();
        out_file << "\t" << Est.eulerPitch();
        out_file << "\t" << Est.eulerYaw();

        double cosPsi = cos(Est.eulerYaw());
        double sinPsi = sin(Est.eulerYaw());
        double cosTheta = cos(Est.eulerPitch());
        double sinTheta = sin(Est.eulerPitch());
        double cosPhi = cos(Est.eulerRoll());
        double sinPhi = sin(Est.eulerRoll());

        globalAcc[0] = cosTheta * cosPsi * a[0] + (sinPhi * sinTheta * cosPsi - cosPhi * sinPsi) * a[1] + (cosPhi * sinTheta * cosPsi + sinPhi * sinPsi) * a[2];
        globalAcc[1] = cosTheta*sinPsi*a[0] + (sinPhi*sinTheta*sinPsi+cosPhi*cosPsi)*a[1]+(cosPhi*sinTheta*sinPsi-sinPhi*cosPsi)*a[2];
        globalAcc[2] = -sinTheta*a[0]+sinPhi*cosTheta*a[1]+cosPhi*cosTheta*a[2] - DEFAULT_g0;
        for (int i=0; i<3; ++i)
        {
            v[i] += (prevAcc[i]+globalAcc[i])* SAMPLING_TIME/2.0;
            prevAcc[i] = globalAcc[i];
        }
        out_file << "\t" << v[0];
        out_file << "\t" << v[1];
        out_file << "\t" << v[2] << std::endl;
    }
    in_file.close();
    out_file.close();
    return 0;
}