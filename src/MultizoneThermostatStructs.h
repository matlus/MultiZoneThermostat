#ifndef MultizoneThermostatStructs_h
#define MultizoneThermostatStructs_h
#include <string.h>

enum class OnOffState { Off, On };
enum class ModeType { Off, Cool, Heat, Fan };

struct Constants {
public:
  /*
  zoneNameLength:
  The length of the name of a Zone is 25 characters. If any
  of your Zones has a name longer than 25 characters, change
  this const accordingly
  */
  constexpr static const size_t zoneNameLength{26};

private:
  Constants() {}
};

struct EepromSettings {
  EepromSettings() {}
  EepromSettings(const ModeType &mode, const char *zone,
                 const float temperature) {
    this->mode = mode;
    auto zoneSize = sizeof(this->zone);
    strncpy(this->zone, zone, zoneSize - 1);
    this->zone[zoneSize - 1] = '\0';
    this->temperature = temperature;
  }
  ModeType mode{ModeType::Off};
  /* This needs to be a fixed size due to EEPROM needs */
  char zone[Constants::zoneNameLength]{};
  float temperature = 0.0f;
};

struct SettingsData {
  SettingsData(const ModeType &mode, const char *zone,
               const float temperature) {
    this->mode = mode;
    strcpy(this->zone, zone);
    this->temperature = temperature;
  }
  SettingsData()
      : mode{ModeType::Off}, zone{}, temperature{0.0f}, heater{OnOffState::Off},
        airconditioner{OnOffState::Off}, fan{OnOffState::Off}, applianceOn{
                                                                   false} {}
  ModeType mode = ModeType::Off;
  char zone[Constants::zoneNameLength];
  float temperature = 0.0f;
  OnOffState heater = OnOffState::Off;
  OnOffState airconditioner = OnOffState::Off;
  OnOffState fan = OnOffState::Off;
  bool applianceOn = false;
};

struct PublishData {
  PublishData(const char *buf, size_t sz) {
    buffer = buf;
    size = sz;
  }
  const char *buffer;
  size_t size;
};

#endif
