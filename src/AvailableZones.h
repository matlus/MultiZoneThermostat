#ifndef AvailableZones_h
#define AvailableZones_h
#include "Zone.h"
#include <cstddef>
#include <memory>
#include <vector>

class AvailableZones {
public:
  AvailableZones();
  ~AvailableZones();
  void upsertZone(const char *name, const float temperature,
                  const float humidity);
  const Zone *findZone(const char *name) const;
  const char *toStringJson() const;
  const char *toString() const;

private:
  constexpr static size_t m_noOfZones{10};
  constexpr static size_t m_sizeOfZonesJson{70 * m_noOfZones + 1};
  std::vector<Zone> m_zones;
  std::unique_ptr<char[]> m_zonesBuffer;
  std::unique_ptr<char[]> m_tempBuffer;
};

#endif