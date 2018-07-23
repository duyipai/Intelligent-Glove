// async_subscribe.cpp
//
// This is a Paho MQTT C++ client, sample application.
//
// This application is an MQTT subscriber using the C++ asynchronous client
// interface, employing callbacks to receive messages and status updates.
//
// The sample demonstrates:
//  - Connecting to an MQTT server/broker.
//  - Subscribing to a topic
//  - Receiving messages through the callback API
//  - Receiving network disconnect updates and attempting manual reconnects.
//  - Using a "clean session" and manually re-subscribing to topics on
//    reconnect.
//

/*******************************************************************************
 * Copyright (c) 2013-2017 Frank Pagliughi <fpagliughi@mindspring.com>
 *
 * All rights reserved. This program and the accompanying materials
 * are made available under the terms of the Eclipse Public License v1.0
 * and Eclipse Distribution License v1.0 which accompany this distribution.
 *
 * The Eclipse Public License is available at
 *    http://www.eclipse.org/legal/epl-v10.html
 * and the Eclipse Distribution License is available at
 *   http://www.eclipse.org/org/documents/edl-v10.php.
 *
 * Contributors:
 *    Frank Pagliughi - initial implementation and documentation
 *******************************************************************************/

#include <iostream>
#include <sstream>
#include "math.h"
#include <cstdlib>
#include <string>
#include <cstring>
#include <cctype>
#include <thread>
#include <chrono>
#include "mqtt/async_client.h"
#include "attitude_estimator.h"
#define DEFAULT_g0 9.81
#define M_PI 3.1415926535898

const std::string SERVER_ADDRESS("tcp://localhost:1883");
const std::string CLIENT_ID("subscribe");
const std::string TOPIC("XDK1");

stateestimation::AttitudeEstimator Est;
double v[3] = {0.0};
double prevt = 0.0;
double prevAcc[3] = {0.0};

const int	QOS = 1;
const int	N_RETRY_ATTEMPTS = 5;

/////////////////////////////////////////////////////////////////////////////

// Callbacks for the success or failures of requested actions.
// This could be used to initiate further action, but here we just log the
// results to the console.

class action_listener : public virtual mqtt::iaction_listener
{
	std::string name_;

	void on_failure(const mqtt::token& tok) override {
		std::cout << name_ << " failure";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		std::cout << std::endl;
	}

	void on_success(const mqtt::token& tok) override {
		std::cout << name_ << " success";
		if (tok.get_message_id() != 0)
			std::cout << " for token: [" << tok.get_message_id() << "]" << std::endl;
		auto top = tok.get_topics();
		if (top && !top->empty())
			std::cout << "\ttoken topic: '" << (*top)[0] << "', ..." << std::endl;
		std::cout << std::endl;
	}

public:
	action_listener(const std::string& name) : name_(name) {}
};

/////////////////////////////////////////////////////////////////////////////

/**
 * Local callback & listener class for use with the client connection.
 * This is primarily intended to receive messages, but it will also monitor
 * the connection to the broker. If the connection is lost, it will attempt
 * to restore the connection and re-subscribe to the topic.
 */
class callback : public virtual mqtt::callback,
					public virtual mqtt::iaction_listener

{
	// Counter for the number of connection retries
	int nretry_;
	// The MQTT client
	mqtt::async_client& cli_;
	// Options to use if we need to reconnect
	mqtt::connect_options& connOpts_;
	// An action listener to display the result of actions.
	action_listener subListener_;

	// This deomonstrates manually reconnecting to the broker by calling
	// connect() again. This is a possibility for an application that keeps
	// a copy of it's original connect_options, or if the app wants to
	// reconnect with different options.
	// Another way this can be done manually, if using the same options, is
	// to just call the async_client::reconnect() method.
	void reconnect() {
		std::this_thread::sleep_for(std::chrono::milliseconds(2500));
		try {
			cli_.connect(connOpts_, nullptr, *this);
		}
		catch (const mqtt::exception& exc) {
			std::cerr << "Error: " << exc.what() << std::endl;
			exit(1);
		}
	}

	// Re-connection failure
	void on_failure(const mqtt::token& tok) override {
		std::cout << "Connection failed" << std::endl;
		if (++nretry_ > N_RETRY_ATTEMPTS)
			exit(1);
		reconnect();
	}

	// Re-connection success
	void on_success(const mqtt::token& tok) override {
		std::cout << "\nConnection success" << std::endl;
		std::cout << "\nSubscribing to topic '" << TOPIC << "'\n"
			<< "\tfor client " << CLIENT_ID
			<< " using QoS" << QOS << "\n"
			<< "\nPress Q<Enter> to quit\n" << std::endl;

		cli_.subscribe(TOPIC, QOS, nullptr, subListener_);
	}

	// Callback for when the connection is lost.
	// This will initiate the attempt to manually reconnect.
	void connection_lost(const std::string& cause) override {
		std::cout << "\nConnection lost" << std::endl;
		if (!cause.empty())
			std::cout << "\tcause: " << cause << std::endl;

		std::cout << "Reconnecting..." << std::endl;
		nretry_ = 0;
		reconnect();
	}

	// Callback for when a message arrives.
	void message_arrived(mqtt::const_message_ptr msg) override {
		static int i=0;
    	double globalAcc[3];
		std::istringstream is(msg->to_string());
		double w[3];
        double a[3];
        double m[3];
		double t;
		is >> t;
		t /= 1000;
		if (fabs(t-prevt) > 0.1)
		{
			std::cout<<"status reseted"<<std::endl;
			prevt = t;
			Est.reset();
			for(int i = 0; i < 3; ++i)
			{
				v[i] = 0.0;
				prevAcc[i] = 0.0;
			}
		}
        is >> a[0];
        is >> a[1];
        is >> a[2];
        is >> w[0];
        is >> w[1];
        is >> w[2];
        is >> m[0];
        is >> m[1];
        is >> m[2];
		for (int i = 0; i < 3; ++i)
        {
            w[i] /= 1000.0 * 180 / M_PI;
            a[i] /= -1000.0/DEFAULT_g0;
        }
		Est.update(t-prevt, w[0], w[1], w[2], a[0], a[1], a[2], m[0], m[1], m[2]);
        
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
            v[i] += (prevAcc[i]+globalAcc[i])* (t-prevt)/2.0;
            prevAcc[i] = globalAcc[i];
        }
		prevt = t;
		++i;
		if (i%50 == 0)
		{
			std::cout<<"roll: "<<Est.eulerRoll()<<" pitch: "<<Est.eulerPitch()<<" yaw: "<<Est.eulerYaw();
			std::cout<<" Vx: "<<v[0]<<" Vy: "<<v[1]<<" Vz: "<<v[2]<<std::endl;
		}
	}

	void delivery_complete(mqtt::delivery_token_ptr token) override {}

public:
	callback(mqtt::async_client& cli, mqtt::connect_options& connOpts)
				: nretry_(0), cli_(cli), connOpts_(connOpts), subListener_("Subscription") {}
};

/////////////////////////////////////////////////////////////////////////////

int main(int argc, char* argv[])
{
	mqtt::connect_options connOpts;
	connOpts.set_keep_alive_interval(20);
	connOpts.set_clean_session(true);

	mqtt::async_client client(SERVER_ADDRESS, CLIENT_ID);

	callback cb(client, connOpts);
	client.set_callback(cb);

	// Start the connection.
	// When completed, the callback will subscribe to topic.

	try {
		std::cout << "Connecting to the MQTT server..." << std::flush;
		client.connect(connOpts, nullptr, cb);
	}
	catch (const mqtt::exception&) {
		std::cerr << "\nERROR: Unable to connect to MQTT server: '"
			<< SERVER_ADDRESS << "'" << std::endl;
		return 1;
	}

	// Just block till user tells us to quit.

	while (std::tolower(std::cin.get()) != 'q')
		;

	// Disconnect

	try {
		std::cout << "\nDisconnecting from the MQTT server..." << std::flush;
		client.disconnect()->wait();
		std::cout << "OK" << std::endl;
	}
	catch (const mqtt::exception& exc) {
		std::cerr << exc.what() << std::endl;
		return 1;
	}

 	return 0;
}

