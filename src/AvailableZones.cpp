#include "AvailableZones.h"
#include <cstdio>
#include <cstring>

AvailableZones::AvailableZones() {
  m_zones = std::vector<Zone>();
  m_tempBuffer = std::unique_ptr<char[]>(new char[100]{});
  m_zonesBuffer = std::unique_ptr<char[]>(new char[m_sizeOfZonesJson]{});
}

AvailableZones::~AvailableZones() {}

/*
Using Arguments, add a Zone to the private zoneVec member if it does not exist.
If is does exist (names of zones are unique and used as the "key"), then update
the temperature and humidity of the existing zone with those in the arguments
*/
void AvailableZones::upsertZone(const char *name, const float temperature,
                                const float humidity) {

  for (auto &zone : m_zones) {
    if (strcmp(zone.name.get(), name) == 0) {
      zone.temperature = temperature;
      zone.humidity = humidity;
      return;
    }
  }

  m_zones.emplace_back(name, temperature, humidity);
}

/*
Given a Zone name, find the zone and return it
If a Zone with the given name does not exist
return a nullptr
*/
const Zone *AvailableZones::findZone(const char *name) const {
  for (auto const &zone : m_zones) {
    if (strcmp(zone.name.get(), name) == 0) {
      return &zone;
    }
  }

  return nullptr;
}

const char *AvailableZones::toStringJson() const {
  bool atleastOne = false;
  auto pZonesJson = m_zonesBuffer.get();

  memset(pZonesJson, 0, m_sizeOfZonesJson);

  strcat(pZonesJson, "[");

  for (auto const &zone : m_zones) {
    if (atleastOne) {
      strcat(pZonesJson, ",");
    }

    memset(m_tempBuffer.get(), 0, sizeof m_tempBuffer);
    sprintf(m_tempBuffer.get(), "{\"N\": \"%s\",\"T\": %.2f,\"H\": %.2f}",
            zone.name.get(), zone.temperature, zone.humidity);
    strcat(pZonesJson, m_tempBuffer.get());
    atleastOne = true;
  }

  strcat(pZonesJson, "]");
  return pZonesJson;
}

const char *AvailableZones::toString() const {
  auto atleastOne = false;
  auto pZonesJson = m_zonesBuffer.get();

  memset(pZonesJson, 0, m_sizeOfZonesJson);

  for (auto const &zone : m_zones) {

    if (atleastOne) {
      strcat(pZonesJson, "|");
    }

    memset(m_tempBuffer.get(), 0, sizeof m_tempBuffer);
    sprintf(m_tempBuffer.get(), "N=%s&T=%.2f&H=%.2f", zone.name.get(),
            zone.temperature, zone.humidity);
    strcat(pZonesJson, m_tempBuffer.get());
    atleastOne = true;
  }

  if (!atleastOne) {
    return '\0';
  } else {
    return pZonesJson;
  }
}
