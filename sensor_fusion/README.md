To compile:

g++ -o test .\subscribe.cpp .\attitude_estimator.cpp .\message_processor.cpp .\states.cpp -lpaho-mqttpp3 -lpaho-mqtt3a

or using the provided makefile.

Dependancies:
     paho c++ library https://github.com/eclipse/paho.mqtt.cpp
     paho c library https://github.com/eclipse/paho.mqtt.c

The dependancies are used to initialize the message client for mqtt service. To open up the corresponding ports, you might need an mqtt broker, for which I used emqttd (https://github.com/emqtt/emqttd). Message from LAN can then be received and processed in subscribe.cpp.

The attitude_estimator files are from https://github.com/AIS-Bonn/attitude_estimator. It utilized a complementary filter to estimate the attitude. The message is processed in message_processor, which means all threshod tunable are in pre-processors in messsage_processor.cpp. The main job of this sub-project is to receive the data transmitted from XDK using mqtt and then process it, write the relervant data into two txt files (output1.txt, output2.txt) for communication with GUI.