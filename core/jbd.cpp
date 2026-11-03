#include "jbd.h"

namespace powersense {
namespace {
uint16_t be16(const uint8_t* p) {
  return static_cast<uint16_t>((static_cast<uint16_t>(p[0]) << 8) | p[1]);
}
}

bool decodeJbdBasicInfo(const uint8_t* frame, size_t size, BatteryTelemetry& out) {
  // DD 03 00 LEN [DATA] CHK_H CHK_L 77
  if (!frame || size < 7 || frame[0] != 0xDD || frame[1] != 0x03 ||
      frame[2] != 0x00 || frame[size - 1] != 0x77) return false;
  const size_t length = frame[3];
  if (size != length + 7 || length < 23) return false;
  uint32_t sum = frame[2] + frame[3];
  for (size_t i = 0; i < length; ++i) sum += frame[4 + i];
  const uint16_t checksum = static_cast<uint16_t>(-static_cast<int32_t>(sum));
  if (be16(frame + 4 + length) != checksum) return false;
  const uint8_t* data = frame + 4;
  BatteryTelemetry t;
  t.voltage_v = be16(data) / 100.0f;
  t.current_a = static_cast<int16_t>(be16(data + 2)) / 100.0f;
  t.remaining_ah = be16(data + 4) / 100.0f;
  t.nominal_ah = be16(data + 6) / 100.0f;
  t.cycles = be16(data + 8);
  t.protection_bits = be16(data + 16);
  t.soc_percent = data[19];
  if (t.soc_percent > 100) return false;
  const size_t requestedTemps = data[22];
  if (requestedTemps > (length - 23) / 2) return false;
  t.temperature_count = static_cast<uint8_t>(requestedTemps > 4 ? 4 : requestedTemps);
  for (size_t i = 0; i < t.temperature_count; ++i)
    t.temperatures_c[i] = (be16(data + 23 + i * 2) - 2731.0f) / 10.0f;
  t.power_w = t.voltage_v * t.current_a;
  out = t;
  return true;
}
}
