#include "../core/jbd.h"
#include <cassert>
#include <cmath>
#include <cstdint>
#include <vector>
using namespace powersense;

static std::vector<uint8_t> validFrame() {
  // 23-byte basic info: 26.50V, -2.50A, 70Ah remaining, 100Ah rated,
  // 12 cycles, 70% SOC, no reported temperature probes.
  std::vector<uint8_t> d(23, 0);
  d[0]=0x0A; d[1]=0x5A;
  d[2]=0xFF; d[3]=0x06;
  d[4]=0x1B; d[5]=0x58;
  d[6]=0x27; d[7]=0x10;
  d[9]=12; d[19]=70;
  std::vector<uint8_t> f = {0xDD, 0x03, 0x00, static_cast<uint8_t>(d.size())};
  f.insert(f.end(), d.begin(), d.end());
  uint32_t sum = f[2] + f[3];
  for (auto b : d) sum += b;
  auto chk = static_cast<uint16_t>(-static_cast<int32_t>(sum));
  f.push_back(chk >> 8);
  f.push_back(chk & 0xFF);
  f.push_back(0x77);
  return f;
}
int main() {
  auto f = validFrame();
  BatteryTelemetry t;
  assert(decodeJbdBasicInfo(f.data(), f.size(), t));
  assert(std::fabs(t.voltage_v - 26.5f) < 0.01f);
  assert(std::fabs(t.current_a + 2.5f) < 0.01f);
  assert(std::fabs(t.power_w + 66.25f) < 0.1f);
  assert(t.soc_percent == 70 && t.cycles == 12);
  auto bad = f; bad[5] ^= 1;
  assert(!decodeJbdBasicInfo(bad.data(), bad.size(), t));
  bad = f; bad[3] = 50;
  assert(!decodeJbdBasicInfo(bad.data(), bad.size(), t));
  assert(!decodeJbdBasicInfo(nullptr, 0, t));
}
