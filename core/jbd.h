#pragma once
#include <cstddef>
#include <cstdint>

namespace powersense {
struct BatteryTelemetry {
  float voltage_v = 0;
  float current_a = 0;
  float power_w = 0;
  float remaining_ah = 0;
  float nominal_ah = 0;
  uint16_t cycles = 0;
  uint16_t protection_bits = 0;
  uint8_t soc_percent = 0;
  uint8_t temperature_count = 0;
  float temperatures_c[4] = {};
};

// Parses one complete standard JBD basic-info response, command 0x03.
// Returns false for any unverified/invalid packet. Output is not modified on failure.
bool decodeJbdBasicInfo(const uint8_t* frame, size_t size, BatteryTelemetry& out);
}
