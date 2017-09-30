#ifndef MultizoneThermostat_h
#define MultizoneThermostat_h

#include "AvailableZones.h"
#include "Hal.h"
#include "MultizoneThermostatStructs.h"
#include "ThermostatInfo.h"
#include <memory>

class MultizoneThermostat {
public:
  MultizoneThermostat(std::unique_ptr<Hal> hal, const char *commandTopic,
                      const char *eventTopic, const size_t zoneNameLength,
                      const uint8_t applianceActivationIndicatorPin,
                      const uint8_t heatActivationPin,
                      const uint8_t coolActivationPin,
                      const uint8_t fanActivationPin);
  ~MultizoneThermostat();
  void setSettingsData(SettingsData settingsData);
  const ThermostatInfo processMessage(const char *topic, const uint8_t *payload,
                                      unsigned int length) noexcept;
  const PublishData getStatusPublishData() const noexcept;

private:
  uint8_t m_applianceActivationIndicatorPin;
  uint8_t m_heatActivationPin;
  uint8_t m_coolActivationPin;
  uint8_t m_fanActivationPin;
  size_t m_publishDataBufferSize;
  std::unique_ptr<Hal> m_hal;
  std::unique_ptr<char[]> m_eventTopic = nullptr;
  std::unique_ptr<char[]> m_commandTopic = nullptr;
  AvailableZones m_availableZones;
  std::unique_ptr<char[]> m_publishDataBuffer;
  SettingsData m_settingsData;
  float m_lastTemperature;

  constexpr static const size_t m_noOfZones{10};
  constexpr static const char *m_parameterDelimiter = "&";
  constexpr static const char *m_modeOff = "Off";
  constexpr static const char *m_modeCool = "Cool";
  constexpr static const char *m_modeHeat = "Heat";
  constexpr static const char *m_modeFan = "Fan";
  constexpr static const uint8_t LOW = 0x0;
  constexpr static const uint8_t HIGH = 0x1;
  constexpr static const uint8_t INPUT = 0x0;
  constexpr static const uint8_t OUTPUT = 0x1;
  constexpr static const uint8_t INPUT_PULLUP = 0x2;

  void initialize();
  void processEvent(char *payload);
  const SettingsData processCommand(char *payload);
  bool settingsAreEqual(SettingsData previousSettings,
                        SettingsData m_settingsData);
  bool applianceStatesAreEqual(SettingsData previousSettings,
                               SettingsData m_settingsData);
  void turnApplianceOnOff(float temperature);
  void turnOffAllAppliances();
  void turnOnHeater();
  void turnOnAirconditioner();
  void turnOnFan();
  const AvailableZones *getAvailableZones() const;
  const ModeType strToMode(const char *mode) const;
  const char *modeToString(ModeType mode) const;
  char *getValuePart(char *str);
  bool startsWith(const char *str, const char *pre);
};

#endif
