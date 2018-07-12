#include <iostream>
#include "Kalman.h"
#include <fstream>
#include <string>
#include <sstream>
#define PI 3.1415926536
int main(int argc, char** argv)
{
    if (argc != 2)
    {
        std::cout<<"please specify input file"<<std::endl;
    }
    std::ifstream in_file;
    in_file.open(argv[1]);
    std::ofstream out_file;
    out_file.open("pose.txt");
    Kalman kalman;

    std::string data_line;
    while(getline(in_file, data_line))
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
        for (int i =0; i < 3; ++i)
        {
            w[i] /= 1000.0*180/PI;
            a[i] /= 1000.0;
        }
        kalman.updateIMU(w, a, 0.01);
        out_file << kalman.getRoll();
        out_file << " " << kalman.getPitch();
        out_file << " " << kalman.getYaw() << std::endl;
    }
    in_file.close();
    out_file.close();
    return 0;
}