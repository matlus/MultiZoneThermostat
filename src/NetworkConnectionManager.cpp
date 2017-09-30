#include "NetworkConnectionManager.h"

NetworkConnectionManager::NetworkConnectionManager(
    char *mqttBrokerUrl, const char *commandSubscribeTopic,
    const char *eventSubscribeTopic,
    void (*messageReceivedCallback)(char *, uint8_t *, unsigned int))
    : m_mqttBrokerUrl(mqttBrokerUrl),
      m_commandSubscribeTopic(commandSubscribeTopic),
      m_eventSubscribeTopic(eventSubscribeTopic),
      m_mqttClient(std::unique_ptr<MQTT>(
          new MQTT(mqttBrokerUrl, 1883, messageReceivedCallback))) {}

NetworkConnectionManager::~NetworkConnectionManager() {}

void NetworkConnectionManager::loop() {
  m_canPublish = false;

  if (!m_mqttClient->loop()) {
    ensureMqttBrokerConnectivity();
  }

  m_canPublish = true;
}

bool NetworkConnectionManager::publish(const char *topic,
                                       const uint8_t *payload,
                                       unsigned int plength, bool retain,
                                       MQTT::EMQTT_QOS qos) {
  if (m_canPublish) {
    return m_mqttClient->publish(topic, payload, plength, retain, qos);
  }
}

void NetworkConnectionManager::ensureMqttBrokerConnectivity() {
  if (m_mqttClient->isConnected()) {
    return;
  }

  ensureWiFiConnectivity();
  resolveMqttBrokerAddress(m_mqttBrokerUrl);
  bool connected = tryConnectToMqttBroker(10);

  if (!connected) {
    // Serial.println("MQTT Broker Connection Re-Try attempts exceeded...");
    // Serial.println("System Resetting");
    System.reset();
  }
}

void NetworkConnectionManager::ensureWiFiConnectivity() {
  while (!WiFi.ready()) {
    Particle.process();
    // Serial.println("Connecting to WiFi...");
    flashLed(255, 51, 204, 3);
    delay(1000);
  }

  // Serial.println("Connected to WiFi");
  // Serial.print("IP Address: ");
  // Serial.println(WiFi.localIP());
}

void NetworkConnectionManager::resolveMqttBrokerAddress(
    const char *mqttBrokerUrl) {
  // Serial.print("Resolving MQTT Broker Address: ");
  // Serial.print(mqttBrokerUrl);
  uint8_t resolveRetryCount = 0;
  IPAddress ip;
  while (!ip) {
    Particle.process();
    // Serial.print(".");
    ip = WiFi.resolve(mqttBrokerUrl);
    resolveRetryCount++;
    if (resolveRetryCount == 10) {
      // Serial.println("Unable to Resolve MQTT Broker Address");
      // Serial.println("System Resetting");
      delay(3000);
      System.reset();
    }
    delay(1000);
  }
  // Serial.println("\r\nMQTT Broker Address resolved");
}

bool NetworkConnectionManager::tryConnectToMqttBroker(uint8_t noOfRetries) {
  uint8_t noOfRetryAttepmts = 0;

  while (!m_mqttClient->isConnected() && noOfRetryAttepmts < noOfRetries) {
    // Serial.println("\r\nMQTT Client Not Connected");

    byte mac[6];
    WiFi.macAddress(mac);
    char clientId[18];
    sprintf(clientId, "%02x%02x%02x%02x%02x%02x%02x", mac[0], mac[1], mac[2],
            mac[3], mac[4], mac[5], random(0xffff));
    // Serial.print("ClientId: ");
    // Serial.println(clientId);

    // Serial.print("Connecting to MQTT Broker: ");
    // Serial.println(m_mqttBrokerUrl);

    m_mqttClient->connect(clientId);

    if (m_mqttClient->isConnected()) {
      // Serial.println("\r\nConnected to MQTT Broker");
      m_mqttClient->subscribe(m_commandSubscribeTopic);
      m_mqttClient->subscribe(m_eventSubscribeTopic);

      // Serial.print("Subscribed to Command Topic: ");
      // Serial.println(m_commandSubscribeTopic);
      // Serial.print("Subscribed to Event Topic: ");
      // Serial.println(m_eventSubscribeTopic);
      // Serial.println("");
    } else {
      // Serial.print("Failed to connect to MQTT Broker: ");
      // Serial.println(noOfRetryAttepmts + 1);
      // Serial.println("Creating a new instance of Mqtt Client");
      // m_mqttClient = createMqttClient();
      // Serial.println("Trying again in 1 second...");
      flashLed(255, 51, 204, 5);
      delay(1000);
      noOfRetryAttepmts++;
    }
  }

  return (noOfRetryAttepmts < noOfRetries);
}

void NetworkConnectionManager::flashLed(uint8_t red, uint8_t green,
                                        uint8_t blue, uint8_t count) {
  RGB.control(true);

  RGB.color(red, green, blue);
  for (uint8_t i = 0; i < count; i++) {
    RGB.brightness(255);
    delay(50);
    RGB.brightness(64);
    delay(50);
  }

  RGB.control(false);
}

void NetworkConnectionManager::breathingLed(uint8_t red, uint8_t green,
                                            uint8_t blue) {
  RGB.control(true);

  RGB.color(red, green, blue);

  for (uint8_t i = 0; i < 254; i = i + 10) {
    RGB.brightness(i);
    delay(100);
    Particle.process();
  }

  for (uint8_t i = 254; i > 0; i = i - 10) {
    RGB.brightness(i);
    delay(100);
    Particle.process();
  }

  RGB.control(false);
}
