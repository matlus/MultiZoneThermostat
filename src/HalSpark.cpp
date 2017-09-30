#include "HalSpark.h"
#include "spark_wiring.h"

HalSpark::HalSpark() {}

HalSpark::~HalSpark() {}

void HalSpark::doPinMode(uint16_t pin, uint8_t mode) {
  PinMode pMode = PinMode::OUTPUT;

  switch (mode) {
  case 0:
    pMode = PinMode::INPUT;
    break;
  case 1:
    pMode = PinMode::OUTPUT;
    break;
  case 2:
    pMode = PinMode::INPUT_PULLUP;
    break;
  }

  pinMode(pin, pMode);
}

void HalSpark::doDigitalWrite(uint16_t pin, uint8_t value) {
  digitalWrite(pin, value);
}

void HalSpark::doAnalogWrite(uint16_t pin, int8_t value) {
  analogWrite(pin, value);
}

void HalSpark::attach(uint8_t pin) {
  // No implementation for this yet.
  // Will need to include the Servo Library
}

void HalSpark::detach() {
  // No implementation for this yet.
  // Will need to include the Servo Library
}

void HalSpark::write(int position) {
  // No implementation for this yet.
  // Will need to include the Servo Library
}

int HalSpark::doConstrain(int numberToConstrain, int lowerBound,
                          int upperBound) {
  return constrain(numberToConstrain, lowerBound, upperBound);
}

int HalSpark::doMap(int value, int sourceMin, int sourceMax, int targetMin,
                    int targetMax) {
  return map(value, sourceMin, sourceMax, targetMin, targetMax);
}
