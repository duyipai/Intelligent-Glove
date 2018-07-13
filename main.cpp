#include <iostream>
#include "Kalman.h"
#include <fstream>
#include <string>
#include <sstream>
#include "math.h"
#define PI 3.1415926536
#define SAMPLING_TIME 0.01
#define axBias 18.4895
#define ayBias -16.2859
#define azBias 0.0392
#define wxBias -0.6129
#define wyBias -0.0916
#define wzBias -0.0412
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
    Kalman kalman;

    std::string data_line;
    float x[3] = {0.0};
    float v[3] = {0.0};
    float globalAcc[3];
    float localAcc[3];
    float prevAcc[3] = {0.0};
    float prevV[3] = {0.0};
    while (getline(in_file, data_line))
    {
        std::istringstream is(data_line);
        float w[3];
        float a[3];
        is >> a[0];
        is >> a[1];
        is >> a[2];
        is >> w[0];
        is >> w[1];
        is >> w[2];

        a[0] -= axBias;
        a[1] -= ayBias;
        a[2] -= azBias;
        w[0] -= wxBias;
        w[1] -= wyBias;
        w[2] -= wzBias;
        for (int i = 0; i < 3; ++i)
        {
            w[i] /= 1000.0 * 180 / PI;
            a[i] /= 1000.0/DEFAULT_g0;
        }
        
        kalman.updateIMU(w, a, SAMPLING_TIME);
        out_file << kalman.getRoll();
        out_file << " " << kalman.getPitch();
        out_file << " " << kalman.getYaw();

        float cosPsi = cos(kalman.getYaw());
        float sinPsi = sin(kalman.getYaw());
        float cosTheta = cos(kalman.getPitch());
        float sinTheta = sin(kalman.getPitch());
        float cosPhi = cos(kalman.getRoll());
        float sinPhi = sin(kalman.getRoll());
        kalman.getNGAcc(localAcc);
        globalAcc[0] = cosTheta * cosPsi * localAcc[0] + (sinPhi * sinTheta * cosPsi - cosPhi * sinPsi) * localAcc[1] + (cosPhi * sinTheta * cosPsi + sinPhi * sinPsi) * localAcc[2];
        globalAcc[1] = cosTheta*sinPsi*localAcc[0] + (sinPhi*sinTheta*sinPsi+cosPhi*cosPsi)*localAcc[1]+(cosPhi*sinTheta*sinPsi-sinPhi*cosPsi)*localAcc[2];
        globalAcc[2] = -sinTheta*localAcc[0]+sinPhi*cosTheta*localAcc[1]+cosPhi*cosTheta*localAcc[2];// + DEFAULT_g0;
        for (int i=0; i<3; ++i)
        {
            v[i] += (prevAcc[i]+globalAcc[i])* SAMPLING_TIME/2.0;
            prevAcc[i] = globalAcc[i];
            x[i] += (prevV[i] + v[i])*SAMPLING_TIME/2;
            prevV[i] = v[i];
        }
        out_file << " " << x[0];
        out_file << " " << x[1];
        out_file << " " << x[2] << std::endl;
    }
    in_file.close();
    out_file.close();
    return 0;
}