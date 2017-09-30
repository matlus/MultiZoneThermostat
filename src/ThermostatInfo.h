#pragma once
#ifndef ThermostatInfo_h
#define ThermostatInfo_h

#include "AvailableZones.h"
#include "MultizoneThermostatStructs.h"

struct ThermostatInfo {
  ThermostatInfo(const AvailableZones *availableZones,
                 const SettingsData *settingsData, bool settingsHaveChanged,
                 bool applianceStateHasChanged) {
    zones = availableZones;
    settings = settingsData;
    this->settingsHaveChanged = settingsHaveChanged;
    this->applianceStateHasChanged = applianceStateHasChanged;
  }

  const AvailableZones *zones = nullptr;
  const SettingsData *settings = nullptr;
  bool settingsHaveChanged = false;
  bool applianceStateHasChanged = false;
};

#endif
