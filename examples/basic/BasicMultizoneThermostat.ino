#include "AvailableZones.h"
#include "HalSpark.h"
#include "MultizoneThermostat.h"
#include "MultizoneThermostatStructs.h"
#include "NetworkConnectionManager.h"
#include "TemperatureSensorAdaFrBme280.h"
#include <cmath>
#include <cstring>

uint8_t applianceActivationIndicatorPin = D7;
uint8_t heatActivationPin = D2;
uint8_t coolActivationPin = D3;
uint8_t fanActivationPin = D4;
unsigned long lastPublished;
/* Publish Temperature and Information every 5 seconds */
unsigned int publishInterval = 5000;

/* The MQTT Broker to use */
char *mqttBrokerUrl = "iot.eclipse.org";

/* The name of the Zone for this Device - 25 char max length */
const char *deviceZone = "Family Room";

/*
  commandTopic:
  This is the topic the Thermostat is a subscriber to. When you
  want to change the Settings of the Thermostat, you should
  publish messages to this topic
*/
const char *commandTopic = "mythermostat/cmd";

/*
  eventTopic:
  This is the MQTT Topic that all Remote Sensors will publish
  their temperature and humidity information to. This is
  also the topic that the Thermostat will subscribe to in
  order to receive data from remote sensors. The thermost
  will publish its own sensor data to this topic as well
*/
const char *eventTopic = "mythermostat/evt";

/*
  informationTopic:
  This is the MQTT Topic that the Thermostat will publish
  "information" to. This information, is essentially the
  Current Settings, such as the current Mode, current Zone
  and the Current Temperature it has been set to. In addition,
  the Thermostat also publishes data from all remote Sensors
  to this topic (all of this in the same message)
*/
const char *informationTopic = "mythermostat/inf";

TemperatureSensorAdaFrBme280 temperatureSensor{deviceZone};

NetworkConnectionManager networkConnectionManager{
    mqttBrokerUrl, commandTopic, eventTopic, mqttEventCallback};

MultizoneThermostat multizoneThermostat{std::unique_ptr<Hal>(new HalSpark()),
                                        commandTopic,
                                        eventTopic,
                                        Constants::zoneNameLength,
                                        applianceActivationIndicatorPin,
                                        heatActivationPin,
                                        coolActivationPin,
                                        fanActivationPin};

void setup() {
  temperatureSensor.begin();

  EepromSettings eepromSettings;
  initializeEepromSettings(eepromSettings);
  multizoneThermostat.setSettingsData(SettingsData{
      eepromSettings.mode, eepromSettings.zone, eepromSettings.temperature});
}

void loop() {
  unsigned long now = millis();

  if (now - lastPublished > publishInterval) {
    if (waitFor(WiFi.ready, 30000)) {
      publishTemperatureAndHumidity();
      publishInformation();
      lastPublished = millis();
    } else {
      System.reset();
    }
  }
  networkConnectionManager.loop();
}

void publishTemperatureAndHumidity() {
  const auto weatherPublishData = temperatureSensor.getTemperaturePublishData();
  networkConnectionManager.publish(
      eventTopic, (uint8_t *)weatherPublishData.buffer, weatherPublishData.size,
      true, MQTT::EMQTT_QOS::QOS1);
  networkConnectionManager.flashLed(255, 102, 0, 2);
}

void publishInformation() {
  auto publishData = multizoneThermostat.getStatusPublishData();
  networkConnectionManager.publish(
      informationTopic, (uint8_t *)publishData.buffer, publishData.size, true,
      MQTT::EMQTT_QOS::QOS1);
}

void initializeEepromSettings(EepromSettings &eepromSettings) {
  EEPROM.get(0, eepromSettings);

  if (std::isnan(eepromSettings.temperature)) {
    eepromSettings = {ModeType::Off, deviceZone, 0.0f};
  }
}

void persistSettingsData(const SettingsData *settingsData) {
  EepromSettings eepromSettings{settingsData->mode, settingsData->zone,
                                settingsData->temperature};
  persistEepromSettings(eepromSettings);
}

void persistEepromSettings(const EepromSettings &eepromSettings) {
  EEPROM.put(0, eepromSettings);
}

void mqttEventCallback(char *topic, byte *payload, unsigned int length) {
  const auto thermostatInfo =
      multizoneThermostat.processMessage(topic, payload, length);
  if (thermostatInfo.settingsHaveChanged) {
    persistSettingsData(thermostatInfo.settings);
  }

  if (thermostatInfo.settingsHaveChanged ||
      thermostatInfo.applianceStateHasChanged) {
    publishInformation();
  }
}
