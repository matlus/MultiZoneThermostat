/*
  NetworkManager.h - This class abstracts the management
  of the Wifi and Mqtt Clients. Besides connecting to
  both, it also ensure reconnecting if required
  This class is Particle Photon Specific
  Created by Shiv Kumar, 2014.
  website: http://www.matlus.com
  Released into the public domain.
*/
#ifndef NetworkConnectionManager_h
#define NetworkConnectionManager_h
#include <MQTT.h>

class NetworkConnectionManager {
public:
  NetworkConnectionManager(char *mqttBrokerUrl,
                           const char *commandSubscribeTopic,
                           const char *eventSubscribeTopic,
                           void (*messageReceivedCallback)(char *, uint8_t *,
                                                           unsigned int));
  ~NetworkConnectionManager();
  void loop();
  void ensureMqttBrokerConnectivity();
  bool publish(const char *topic, const uint8_t *payload, unsigned int plength,
               bool retain, MQTT::EMQTT_QOS qos);
  void flashLed(uint8_t red, uint8_t green, uint8_t blue, uint8_t count);

private:
  MQTT m_mqttClient;
  char *m_mqttBrokerUrl = nullptr;
  const char *m_commandSubscribeTopic = nullptr;
  const char *m_eventSubscribeTopic = nullptr;
  int m_ConnectionAttempts = 0;
  bool m_canPublish;
  void ensureWiFiConnectivity();
  void resolveMqttBrokerAddress(const char *mqttBrokerUrl);
  bool tryConnectToMqttBroker(uint8_t noOfRetries);
  void breathingLed(uint8_t red, uint8_t green, uint8_t blue);
};

#endif