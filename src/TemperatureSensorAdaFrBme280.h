#ifndef TemperatureSensor_h
#define TemperatureSensor_n

#include "TemperatureSensorBase.h"
#include <Adafruit_BME280.h>

class TemperatureSensorAdaFrBme280 : public TemperatureSensorBase {
public:
  TemperatureSensorAdaFrBme280(const char *deviceZone);
  virtual ~TemperatureSensorAdaFrBme280() override final;
  virtual void begin() override final;
  virtual const WeatherData getTemperatureAndHumidity() override final;
  virtual const WeatherPublishData getTemperaturePublishData() override final;

private:
  bool bmeSensorFound = false;
  Adafruit_BME280 bme280Sensor;
  const char *m_deviceZone = nullptr;
  static const uint8_t m_weatherPublishBufferSize = 100;
  char *m_weatherPublishBuffer = nullptr;
};

#endif
