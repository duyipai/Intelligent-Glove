#ifndef KALMAN_H
#define KALMAN_H


#include "math.h"

#ifndef NULL
#ifdef __cplusplus
#define NULL 0
#else
#define NULL ((void*)0)
#endif
#endif

/**
 * Default constant values if they are not provided
 */
#define DEFAULT_g0 9.8
#define DEFAULT_state {0,0,1,0,0,0}
#define DEFAULT_q_dcm2 (0.1*0.1)
#define DEFAULT_q_gyro_bias2 (0.0001*0.0001)
#define DEFAULT_r_acc2 (0.5*0.5)
#define DEFAULT_r_a2 (10*10)
#define DEFAULT_q_dcm2_init (1*1)
#define DEFAULT_q_gyro_bias2_init (0.1*0.1)

//! Kalman class.
class Kalman {

public:
	//! Kalman constructor.
	/*!
	 *  Initializes Kalman either with default values or given parameters. All parameters are in SI-units.
         *
	 *  @param Gravity A magnitude of gravity
	 *  @param State An initial state as a array of six floats, DCM states and bias states.
	 *  @param Covariance A covariance matrix (size of 6x6 floats, array of 36 floats in row-major order). If a custom covariance matrix is given, parameters InitialDCMVariance and InitialBiasVariance are not used.
	 *  @param DCMVariance a variance for DCM state update, Q(0,0), Q(1,1), and Q(2,2)
	 *  @param BiasVariance a variance for bias state update, Q(3,3), Q(4,4), and Q(5,5)
	 *  @param InitialDCMVariance an initial variance for DCM state, P(0,0), P(1,1), and P(2,2). If Covariance matrix is given, this parameter is not used.
	 *  @param InitialBiasVariance an initial variance for bias state, P(3,3), P(4,4), and P(5,5). If Covariance matrix is given, this parameter is not used.
	 *  @param MeasurementVariance a constant part of the variance for measurement update, R(0,0), R(1,1), and R(2,2)
	 *  @param MeasurementVarianceVariableGain a gain for the variable part of the variance for measurement update, R(0,0), R(1,1), and R(2,2)
	 */
	Kalman(const float Gravity = DEFAULT_g0, const float *State = NULL, const float *Covariance = NULL,
			const float DCMVariance = DEFAULT_q_dcm2, const float BiasVariance = DEFAULT_q_gyro_bias2,
			const float InitialDCMVariance = DEFAULT_q_dcm2_init, const float InitialBiasVariance = DEFAULT_q_gyro_bias2_init,
			const float MeasurementVariance = DEFAULT_r_acc2, const float MeasurementVarianceVariableGain = DEFAULT_r_a2);

	//! A method to perform update and give new measurements.
	/*!
	 *  This method is used regularly to update new gyroscope and accelerometer measurements into the system. To get best performance of the filter, please calibrate accelerometer and gyroscope readings before sending them into this method. The calibration process is documented in http://dx.doi.org/10.1155/2015/503814
	 *  In addition, please measure the used sample period as accurately as possible for each iteration (delay between current and the last data which was used at the previous update)
	 *  All parameters are in SI-units.
	 *
	 *  @param Gyroscope an array of gyroscope measurements (the length is 3 floats, angular velocities around x, y and z axis).
	 *  @param Accelerometer an array of accelerometer measurements (the length is 3 floats, accelerations in x, y and z axis).
	 *  @param SamplePeriod A delay between this measurement and the previous measurement in seconds.
	 */
	void updateIMU(const float *Gyroscope, const float *Accelerometer, const float SamplePeriod);

	//! A method to query State.
	/*!
	 *  @param State a 6 units long float array where the current state is stored.
	 */
	void getState(float *State);

	//! A method to query Covariance.
	/*!
	 *  @param Covariance a 36 units long float array where the current covariance is stored in row-major order.
	 */
	void getCovariance(float *Covariance);

	//! A method to query non-gravitational acceleration.
	/*!
	 *  @param a a 3 units long float array where the current non-gravitational acceleration is stored (x, y, and z axis).
	 */
	void getNGAcc(float *a);

	//! A method to return the yaw angle.
	/*!
	 *  @return The yaw angle.
	 */
	inline float getYaw() { return yaw; }

	//! A method to return the pitch angle.
	/*!
	 *  @return The pitch angle.
	 */
	inline float getPitch() { return pitch; }

	//! A method to return the roll angle.
	/*!
	 *  @return The roll angle.
	 */
	inline float getRoll() { return roll; }

private:
	float g0, g0_2;
	float x0, x1, x2, x3, x4, x5;
	float q_dcm2;
	float q_gyro_bias2;
	float r_acc2;
	float r_a2;
	float a0, a1, a2;
	float yaw, pitch, roll;
	float P00, P01, P02, P03, P04, P05;
	float P10, P11, P12, P13, P14, P15;
	float P20, P21, P22, P23, P24, P25;
	float P30, P31, P32, P33, P34, P35;
	float P40, P41, P42, P43, P44, P45;
	float P50, P51, P52, P53, P54, P55;
};

#endif /* Kalman_H */
