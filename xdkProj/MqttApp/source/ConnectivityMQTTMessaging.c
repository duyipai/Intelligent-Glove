#include <stdint.h>
#include <FreeRTOS.h>
#include <inttypes.h>
#include <Serval_Ip.h>
#include <Serval_Mqtt.h>
#include <PAL_initialize_ih.h>
#include <semphr.h>
#include <BCDS_Retcode.h>
#include "XdkExceptions.h"

static SemaphoreHandle_t mqttEventSemaphore;
static MqttEvent_t lastEvent;

static const char* TEST_NAME = "XDK1";
static StringDescr_T TestDescription;

/**
 * The client identifier (here: clientID) is a identifier of each MQTT client
 * connecting to a MQTT broker. It needs to be unique for the broker to
 * know the state of the client.
 *
 * We define this client ID globally to ensure it's available in memory even
 * after it was passed to the Serval stack in the setup method.
 */
static const char* MQTT_CLIENT_ID = "1234";
static const char* MQTT_BROKER_HOST = "192.168.0.102";
static const uint16_t MQTT_BROKER_PORT = 1883;

static MqttSession_T mqttSession;
static retcode_t mqttEventHandler(MqttSession_T* session, MqttEvent_t event, const MqttEventData_t* eventData);

Retcode_T ConnectivityMQTTMessaging_Setup(void)
{
	mqttEventSemaphore = xSemaphoreCreateBinary();
	StringDescr_wrap(&TestDescription, TEST_NAME);

	return NO_EXCEPTION;
}

Retcode_T ConnectivityMQTTMessaging_Enable(void)
{
	retcode_t rc;

	rc = Mqtt_initialize();
	if(rc != RC_OK)
	{
		return EXCEPTION_EXCEPTION;
	}

	rc = Mqtt_initializeInternalSession(&mqttSession);
	if(rc != RC_OK)
	{
		return EXCEPTION_EXCEPTION;
	}

	mqttSession.MQTTVersion = 3;
	mqttSession.keepAliveInterval = 60;
	mqttSession.cleanSession = true;
	mqttSession.will.haveWill = false;
	mqttSession.onMqttEvent = mqttEventHandler;

	StringDescr_T clientId;
	StringDescr_wrap(&clientId, MQTT_CLIENT_ID);
	mqttSession.clientID = clientId;

	Ip_Address_T brokerIpAddress;
	rc = PAL_getIpaddress((uint8_t *) MQTT_BROKER_HOST, &brokerIpAddress);
	if(rc != RC_OK)
	{
		return EXCEPTION_EXCEPTION;
	}

	/* Note:
	 *   Rather than filling the mqtt target structure ourselves, we could use the
	 *   SupportedUrl_fromString function. However, this would require use to "re-assemble"
	 *   a valid URL from the IP address, port and path.
	 */
	mqttSession.target.scheme = SERVAL_SCHEME_MQTT;
	mqttSession.target.address = brokerIpAddress;
	mqttSession.target.port = Ip_convertIntToPort(MQTT_BROKER_PORT);

	rc = Mqtt_connect(&mqttSession);
	if(rc != RC_OK)
	{
		printf("[ERROR, %s:%d] Could not connect to MQTT broker, error=0x%04x\n", __FILE__, __LINE__, rc);
		return EXCEPTION_EXCEPTION;
	}

	BaseType_t eventSemaphoreTaken = xSemaphoreTake(mqttEventSemaphore, 10000 / portTICK_PERIOD_MS);
	if(eventSemaphoreTaken == pdTRUE && lastEvent == MQTT_CONNECTION_ESTABLISHED)
	{
		printf("[DEBUG, %s:%d] Connected to MQTT broker\n", __FILE__, __LINE__);
	}
	else
	{
		printf("[ERROR, %s:%d] Connection attempt to MQTT broker timed out\n", __FILE__, __LINE__);
		return EXCEPTION_EXCEPTION;
	}

	return NO_EXCEPTION;
}

/**
 * Provides read access to the Test signal.
 */
Retcode_T ConnectivityMQTTMessaging_Test_Read(char** result)
{

	return NO_EXCEPTION;
}

/**
 * Provides write access to the Test signal.
 */
Retcode_T ConnectivityMQTTMessaging_Test_Write(char** value)
{
	retcode_t rc = Mqtt_publish(&mqttSession, TestDescription, *value, strlen(*value), MQTT_QOS_AT_LEAST_ONCE, false);
	if(rc != RC_OK)
	{
		return EXCEPTION_MQTTEXCEPTION;
	}

	BaseType_t eventSemaphoreTaken = xSemaphoreTake(mqttEventSemaphore, 10/ portTICK_PERIOD_MS);
	if(eventSemaphoreTaken == pdTRUE && lastEvent == MQTT_PUBLISHED_DATA)
	{
		return NO_EXCEPTION;
	}
	else
	{
		//printf("[ERROR, %s:%d] Publish for Test failed\n", __FILE__, __LINE__);
		return EXCEPTION_MQTTEXCEPTION;
	}
	return NO_EXCEPTION;
}


retcode_t mqttEventHandler(MqttSession_T* session, MqttEvent_t event, const MqttEventData_t* eventData) {
	BCDS_UNUSED(session);

	switch (event) {
	case MQTT_CONNECTION_ESTABLISHED:
		// We're connected. Now we can subscribe() or publish()
		printf("[DEBUG, %s:%d] connection established\n", __FILE__, __LINE__);
		break;
	case MQTT_CONNECTION_ERROR:
		printf("[ERROR, %s:%d] connection failed. error=%04d\n", __FILE__, __LINE__, (eventData->connect).connectReturnCode);
		break;
	case MQTT_INCOMING_PUBLISH:
		printf("[DEBUG, %s:%d] incoming publish\n", __FILE__, __LINE__);
		break;
	case MQTT_PUBLISHED_DATA:
		printf("[DEBUG, %s:%d] data published\n", __FILE__, __LINE__);
		break;
	case MQTT_SUBSCRIBE_SEND_FAILED:
		printf("[ERROR, %s:%d] subscribe failed\n", __FILE__, __LINE__);
		break;
	case MQTT_SUBSCRIPTION_ACKNOWLEDGED:
		printf("[DEBUG, %s:%d] subscription acknowledged\n", __FILE__, __LINE__);
		break;
	case MQTT_CONNECT_TIMEOUT:
		printf("[ERROR, %s:%d] connect timed out\n", __FILE__, __LINE__);
		break;
	default:
		printf("[ERROR, %s:%d] unhandled MQTT event: %d\n", __FILE__, __LINE__, event);
		break;
	}

	lastEvent = event;
	xSemaphoreGive(mqttEventSemaphore);

	return RC_OK;
}

