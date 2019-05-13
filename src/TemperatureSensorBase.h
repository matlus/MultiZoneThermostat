/*
TemperatureSensorBase.h - An Abstraction for I2C based
Temperature Sensors. Decendant classes can include the
required libraries and implement as per the library's
requirement while still providing a unified API for
applications requiring Temperature, Humidity, Pressure
and Altitude information
Created by Shiv Kumar, 2014.
website: http://www.matlus.com
Released into the public domain.
*/
#pragma once

#ifndef TemperatureSensorBase_h
#define TemperatureSensorBase_h

#include <cstddef>

struct WeatherData {
  float temperature = 0.0;
  float humidity = 0.0;
  float pressure = 0.0;
  float altitude = 0.0;
};

struct WeatherPublishData {
  const char *buffer;
  size_t size;
};

class TemperatureSensorBase {
public:
  virtual ~TemperatureSensorBase() {}
  virtual void begin() = 0;
  virtual const WeatherData getTemperatureAndHumidity() = 0;
  virtual const WeatherPublishData getTemperaturePublishData() = 0;
};

#endif