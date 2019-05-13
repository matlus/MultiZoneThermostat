#pragma once
#ifndef Zone_h
#define Zone_h
#include <memory>

class Zone {
public:
  Zone(const char *name, const float temperature, const float humidity);
  Zone(const Zone &other);
  Zone(Zone &&other) noexcept;
  ~Zone();
  Zone &operator=(const Zone &other) noexcept;
  Zone &operator=(Zone &&other) noexcept;

  std::unique_ptr<char[]> name = nullptr;
  float temperature = 0.0f;
  float humidity = 0.0f;
};

#endif