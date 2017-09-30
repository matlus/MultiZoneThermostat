/*
Hal.h - Hardware Abstraction Layer for Arduino
Abstracting some basic functions of the Arduino Library
So as to make it possible to decouple Arduino "things"
from other classes, decoupling those classes from Arduino
Created by Shiv Kumar, 2014.
website: http://www.matlus.com
Released into the public domain.
*/
#pragma once

#ifndef Hal_h
#define Hal_h
#include <stdint.h>

class Hal {
protected:
public:
  Hal() {}
  virtual ~Hal() {}
  virtual void doPinMode(uint16_t pin, uint8_t mode) = 0;
  virtual void doDigitalWrite(uint16_t pin, uint8_t value) = 0;
  virtual void doAnalogWrite(uint16_t pin, int8_t value) = 0;
  virtual void attach(uint8_t pin) = 0;
  virtual void detach() = 0;
  virtual void write(int position) = 0;
  virtual int doConstrain(int numberToConstrain, int lowerBound,
                          int upperBound) = 0;
  virtual int doMap(int value, int sourceMin, int sourceMax, int targetMin,
                    int targetMax) = 0;
};

#endif
