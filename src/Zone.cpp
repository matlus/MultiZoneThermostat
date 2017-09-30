#include "Zone.h"
#include <memory>
#include <string.h>

using namespace std;

Zone::Zone(const char *name, const float temperature, const float humidity)
    : name(std::unique_ptr<char[]>(strcpy(new char[strlen(name) + 1], name))),
      temperature{temperature}, humidity{humidity} {}
/* Copy Constructor */
Zone::Zone(const Zone &other)
    : name(std::unique_ptr<char[]>(
          strcpy(new char[strlen(other.name.get()) + 1], other.name.get()))),
      temperature{other.temperature}, humidity{other.humidity} {}
/* Move Constructor */
Zone::Zone(Zone &&other) noexcept
    : name(nullptr), temperature(0.0f), humidity(0.0f) {
  // Copy the data pointer and its length from the
  // source object.
  name = std::move(other.name);
  temperature = other.temperature;
  humidity = other.humidity;

  // Release the data pointer from the source object so that
  // the destructor does not free the memory multiple times.
  other.name = nullptr;
  other.temperature = 0.0f;
  other.humidity = 0.0f;
}

Zone::~Zone() {}

/* Copy Assignment Operator */
Zone &Zone::operator=(const Zone &other) noexcept {
  Zone tmpZone(other);
  std::swap(name, tmpZone.name);
  std::swap(temperature, tmpZone.temperature);
  std::swap(humidity, tmpZone.humidity);
  return *this;
}

/* Move Assignment Operator */
Zone &Zone::operator=(Zone &&other) noexcept {
  if (this != &other) {
    // Copy the data pointer and its length from the
    // source object.
    name.reset(other.name.release());
    temperature = other.temperature;
    humidity = other.humidity;

    // Release the data pointer from the source object so that
    // the destructor does not free the memory multiple times.
    other.name = nullptr;
    other.temperature = 0.0f;
    other.humidity = 0.0f;
  }

  return *this;
}
