/*
  HalSpark.h - Implementation of the Hal abstract class
  where this implementation delegates to the Arduino
  library functions
  Created by Shiv Kumar, 2014.
  website: http://www.matlus.com
  Released into the public domain.
*/
#pragma once

#ifndef HalSpark_h
#define HalSpark_h
#include "Hal.h"

class HalSpark : public Hal {
public:
  HalSpark();
  ~HalSpark();
  void doPinMode(uint16_t pin, uint8_t mode) override final;
  void doDigitalWrite(uint16_t pin, uint8_t value) override final;
  void doAnalogWrite(uint16_t pin, int8_t value) override final;
  void attach(uint8_t pin) override final;
  void detach() override final;
  void write(int position) override final;
  int doConstrain(int numberToConstrain, int lowerBound,
                  int upperBound) override final;
  int doMap(int value, int sourceMin, int sourceMax, int targetMin,
            int targetMax) override final;
};

#endif
