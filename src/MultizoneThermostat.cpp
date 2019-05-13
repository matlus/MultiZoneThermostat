#include "MultizoneThermostat.h"
#include <cstdio>
#include <stdlib.h>

MultizoneThermostat::MultizoneThermostat(
    std::unique_ptr<Hal> hal, const char *commandTopic, const char *eventTopic,
    const size_t zoneNameLength, const uint8_t applianceActivationIndicatorPin,
    const uint8_t heatActivationPin, const uint8_t coolActivationPin,
    const uint8_t fanActivationPin)
    : m_hal(std::move(hal)) {

  m_commandTopic = std::unique_ptr<char[]>(
      strcpy(new char[strlen(commandTopic) + 1], commandTopic));
  m_eventTopic = std::unique_ptr<char[]>(
      strcpy(new char[strlen(eventTopic) + 1], eventTopic));
  m_publishDataBufferSize = 24 + zoneNameLength + (70 * m_noOfZones);
  m_publishDataBuffer =
      std::unique_ptr<char[]>(new char[m_publishDataBufferSize]);

  m_lastTemperature = 0.0f;
  m_applianceActivationIndicatorPin = applianceActivationIndicatorPin;
  m_heatActivationPin = heatActivationPin;
  m_coolActivationPin = coolActivationPin;
  m_fanActivationPin = fanActivationPin;
  initialize();
}

MultizoneThermostat::~MultizoneThermostat() {}

void MultizoneThermostat::initialize() {
  m_hal->doPinMode(m_applianceActivationIndicatorPin, OUTPUT);
  m_hal->doDigitalWrite(m_applianceActivationIndicatorPin, LOW);

  m_hal->doPinMode(m_heatActivationPin, OUTPUT);
  m_hal->doDigitalWrite(m_heatActivationPin, LOW);

  m_hal->doPinMode(m_coolActivationPin, OUTPUT);
  m_hal->doDigitalWrite(m_coolActivationPin, LOW);

  m_hal->doPinMode(m_fanActivationPin, OUTPUT);
  m_hal->doDigitalWrite(m_fanActivationPin, LOW);
}

void MultizoneThermostat::setSettingsData(SettingsData settingsData) {
  m_settingsData = settingsData;
}

const ThermostatInfo
MultizoneThermostat::processMessage(const char *topic, const uint8_t *payload,
                                    unsigned int length) noexcept {
  std::unique_ptr<char[]> tempPayload =
      std::unique_ptr<char[]>(new char[length + 1]);
  auto pTempPayload = tempPayload.get();
  strncpy(pTempPayload, (char *)payload, length);
  tempPayload[length] = '\0';

  SettingsData previousSettings = m_settingsData;

  if (strcmp(topic, m_commandTopic.get()) == 0) {
    processCommand(pTempPayload);
  } else if (strcmp(topic, m_eventTopic.get()) == 0) {
    processEvent(pTempPayload);
  }

  bool settingsHaveChanged =
      !settingsAreEqual(previousSettings, m_settingsData);
  bool applianceStateHasChanged =
      !applianceStatesAreEqual(previousSettings, m_settingsData);
  return ThermostatInfo{&m_availableZones, &m_settingsData, settingsHaveChanged,
                        applianceStateHasChanged};
}

const AvailableZones *MultizoneThermostat::getAvailableZones() const {
  return &m_availableZones;
}

const SettingsData MultizoneThermostat::processCommand(char *payload) {
  char *token = strtok(payload, m_parameterDelimiter);

  // cmd=set&Z=Master Bedroom&M=Off&T=68.00
  if (startsWith(token, "cmd")) {
    const char *command = getValuePart(token);

    if (strcmp(command, "set") == 0) {
      token = strtok(NULL, m_parameterDelimiter);
      if (startsWith(token, "Z")) {
        const char *zone = getValuePart(token);
        if (strcmp(zone, "*") != 0) {
          strcpy(m_settingsData.zone, zone);
        }
      }

      token = strtok(NULL, m_parameterDelimiter);
      if (startsWith(token, "M")) {
        const char *mode = getValuePart(token);
        if (strcmp(mode, "*") != 0) {
          m_settingsData.mode = strToMode(mode);
        }
      }

      token = strtok(NULL, m_parameterDelimiter);
      if (startsWith(token, "T")) {
        const char *temp = getValuePart(token);

        if (strcmp(temp, "*") != 0) {
          // Handle increasing or decreasing the temperature value from current
          // value
          const char sign = temp[0];
          if (sign == '+' || sign == '-') {
            const char *tempValue = temp + 1; // skip the first character
            const float tempChange = static_cast<float>(atof(tempValue));

            if (sign == '+') {
              m_settingsData.temperature =
                  m_settingsData.temperature + tempChange;
            } else {
              m_settingsData.temperature =
                  m_settingsData.temperature - tempChange;
            }
          } else {
            m_settingsData.temperature = static_cast<float>(atof(temp));
          }
        }
      }
    }
  }

  return m_settingsData;
}

bool MultizoneThermostat::settingsAreEqual(SettingsData previousSettings,
                                           SettingsData currentSettingsData) {
  return (previousSettings.temperature == currentSettingsData.temperature) &&
         (previousSettings.mode == currentSettingsData.mode) &&
         (strcmp(previousSettings.zone, currentSettingsData.zone) == 0);
}

bool MultizoneThermostat::applianceStatesAreEqual(
    SettingsData previousSettings, SettingsData currentSettingsData) {
  return (previousSettings.heater == currentSettingsData.heater) &&
         (previousSettings.airconditioner ==
          currentSettingsData.airconditioner) &&
         (previousSettings.fan == currentSettingsData.fan);
}

void MultizoneThermostat::processEvent(char *payload) {
  char *token = strtok(payload, m_parameterDelimiter);

  if (startsWith(token, "Z")) {
    char *zone = 0;
    float humidity = 0.0f;
    float temperature = 0.0f;

    zone = getValuePart(token);

    token = strtok(NULL, m_parameterDelimiter);
    if (startsWith(token, "T")) {
      const char *temp = getValuePart(token);
      temperature = static_cast<float>(atof(temp));
    }

    token = strtok(NULL, m_parameterDelimiter);
    if (startsWith(token, "H")) {
      const char *hum = getValuePart(token);
      humidity = static_cast<float>(atof(hum));
    }

    m_availableZones.upsertZone(zone, temperature, humidity);

    // If the event is from the Current Active Zone
    if (strcmp(zone, m_settingsData.zone) == 0) {
      turnApplianceOnOff(temperature);
    } else {
      // Nothing to do here
    }
  }
}

void MultizoneThermostat::turnApplianceOnOff(float temperature) {
  // if lastTemperature has not been initialized or First Time Use
  // then initialize it to the temperature received
  if (m_lastTemperature == 0.0f) {
    m_lastTemperature = temperature;
  }

  // If the temperature change is greater than + or - 10, ignore the new
  // temperature
  if (temperature >= m_lastTemperature + 10.0f ||
      temperature <= m_lastTemperature - 10.0f) {
    temperature = m_lastTemperature;
  } else {
    m_lastTemperature = temperature;
  }

  if (m_settingsData.mode == ModeType::Heat) {
    if (temperature >= m_settingsData.temperature) {
      turnOffAllAppliances();
    } else if (temperature < m_settingsData.temperature - 0.5f) {
      turnOnHeater();
    }
  } else if (m_settingsData.mode == ModeType::Cool) {
    if (temperature >= m_settingsData.temperature + 0.5f) {
      turnOnAirconditioner();
    } else if (temperature < m_settingsData.temperature) {
      turnOffAllAppliances();
    }
  } else if (m_settingsData.mode == ModeType::Fan) {
    turnOnFan();
  } else if (m_settingsData.mode == ModeType::Off) {
    turnOffAllAppliances();
  }
}

void MultizoneThermostat::turnOffAllAppliances() {
  m_hal->doDigitalWrite(m_heatActivationPin, LOW);
  m_hal->doDigitalWrite(m_coolActivationPin, LOW);
  m_hal->doDigitalWrite(m_fanActivationPin, LOW);
  m_hal->doDigitalWrite(m_applianceActivationIndicatorPin, LOW);

  m_settingsData.heater = OnOffState::Off;
  m_settingsData.airconditioner = OnOffState::Off;
  m_settingsData.fan = OnOffState::Off;
  m_settingsData.applianceOn = false;
}

void MultizoneThermostat::turnOnHeater() {
  m_hal->doDigitalWrite(m_heatActivationPin, HIGH);
  m_hal->doDigitalWrite(m_coolActivationPin, LOW);
  m_hal->doDigitalWrite(m_fanActivationPin, LOW);
  m_hal->doDigitalWrite(m_applianceActivationIndicatorPin, HIGH);

  m_settingsData.heater = OnOffState::On;
  m_settingsData.airconditioner = OnOffState::Off;
  m_settingsData.fan = OnOffState::Off;
  m_settingsData.applianceOn = true;
}

void MultizoneThermostat::turnOnAirconditioner() {
  m_hal->doDigitalWrite(m_heatActivationPin, LOW);
  m_hal->doDigitalWrite(m_coolActivationPin, HIGH);
  m_hal->doDigitalWrite(m_fanActivationPin, LOW);
  m_hal->doDigitalWrite(m_applianceActivationIndicatorPin, HIGH);

  m_settingsData.heater = OnOffState::Off;
  m_settingsData.airconditioner = OnOffState::On;
  m_settingsData.fan = OnOffState::Off;
  m_settingsData.applianceOn = true;
}

void MultizoneThermostat::turnOnFan() {
  m_hal->doDigitalWrite(m_heatActivationPin, LOW);
  m_hal->doDigitalWrite(m_coolActivationPin, LOW);
  m_hal->doDigitalWrite(m_fanActivationPin, HIGH);
  m_hal->doDigitalWrite(m_applianceActivationIndicatorPin, HIGH);

  m_settingsData.heater = OnOffState::Off;
  m_settingsData.airconditioner = OnOffState::Off;
  m_settingsData.fan = OnOffState::On;
  m_settingsData.applianceOn = true;
}

const ModeType MultizoneThermostat::strToMode(const char *mode) const {
  if (strcmp(mode, m_modeCool) == 0) {
    return ModeType::Cool;
  } else if (strcmp(mode, m_modeHeat) == 0) {
    return ModeType::Heat;
  } else if (strcmp(mode, m_modeFan) == 0) {
    return ModeType::Fan;
  } else {
    return ModeType::Off;
  }
}

const char *MultizoneThermostat::modeToString(const ModeType mode) const {
  switch (mode) {
  case ModeType::Off:
  default:
    return m_modeOff;
  case ModeType::Cool:
    return m_modeCool;
  case ModeType::Heat:
    return m_modeHeat;
  case ModeType::Fan:
    return m_modeFan;
  }
}

const PublishData MultizoneThermostat::getStatusPublishData() const noexcept {
  // AM - Active Mode
  // AZ - Active Zone
  // ST - Set Temperature
  // AO - Appliance On
  // Followed by Data pertaining to all Zones in a "|" delimited string format
  auto modeAsString = modeToString(m_settingsData.mode);
  auto availableZonesAsString = m_availableZones.toString();
  auto pPublishDataBuffer = m_publishDataBuffer.get();

  memset(pPublishDataBuffer, 0, m_publishDataBufferSize);

  size_t bufferSize = snprintf(pPublishDataBuffer, m_publishDataBufferSize,
                               "AM=%s&AZ=%s&ST=%3.2f&AO=%s|%s", modeAsString,
                               m_settingsData.zone, m_settingsData.temperature,
                               m_settingsData.applianceOn ? "true" : "false",
                               availableZonesAsString);

  return PublishData{pPublishDataBuffer, bufferSize};
}

char *MultizoneThermostat::getValuePart(const char *str) {
  char *value = strchr(str, '=');
  if (value != NULL) {
    ++value;
  }

  return value;
}

bool MultizoneThermostat::startsWith(const char *str, const char *pre) {
  return strncmp(pre, str, strlen(pre)) == 0;
}