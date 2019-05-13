#include "TemperatureSensorAdaFrBme280.h"

#define SEALEVELPRESSURE_HPA (1013.25)

TemperatureSensorAdaFrBme280::TemperatureSensorAdaFrBme280(
    const char *deviceZone) {
  m_deviceZone = deviceZone;
  m_weatherPublishBuffer = new char[m_weatherPublishBufferSize];
}

TemperatureSensorAdaFrBme280::~TemperatureSensorAdaFrBme280() {
  delete[] m_weatherPublishBuffer;
}

void TemperatureSensorAdaFrBme280::begin() {
  uint8_t bmeWaitCount = 0;
  while (!bme280Sensor.begin() && bmeWaitCount < 10) {
    Serial.println("Could not find a valid BME280 sensor, check wiring!");
    delay(1000);
    bmeWaitCount++;
  }

  if (bmeWaitCount < 10) {
    bmeSensorFound = true;
  }
}

const WeatherData TemperatureSensorAdaFrBme280::getTemperatureAndHumidity() {
  WeatherData weatherData;
  if (bmeSensorFound) {
    float temperatureInCentigrade = bme280Sensor.readTemperature();
    float temperatureInFahrenheit = temperatureInCentigrade * 1.8 + 32;
    weatherData.temperature = temperatureInFahrenheit;
    weatherData.humidity = bme280Sensor.readHumidity();
  } else {
    weatherData.temperature = 0.0;
    weatherData.humidity = 0.0;
  }
  return weatherData;
}

const WeatherPublishData
TemperatureSensorAdaFrBme280::getTemperaturePublishData() {
  const WeatherData weatherData = getTemperatureAndHumidity();
  // Z - Zone
  // T - Temperature
  // H - Humidity
  memset(m_weatherPublishBuffer, 0, m_weatherPublishBufferSize);
  auto size = snprintf(m_weatherPublishBuffer, m_weatherPublishBufferSize - 1,
                       "Z=%s&T=%3.2f&H=%3.2f", m_deviceZone,
                       weatherData.temperature, weatherData.humidity);
  WeatherPublishData weatherPublishData{m_weatherPublishBuffer, size};
  return weatherPublishData;
}