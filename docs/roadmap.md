# Roadmap

## v0.1.0 foundation
- [x] Portable standard JBD basic-info parser with checksum and validation
- [x] Native parser unit test
- [x] ESP32 BLE scanning and read-only polling prototype
- [x] Document custom Tuya data points and circular SOC panel
- [x] CI: parser test and firmware compilation (status depends on workflow runs)
- [ ] Confirm Gentai ECO039E GATT UUIDs and manufacturer-specific response format against real hardware
- [ ] Verify full ESP32 BLE compile and connection on physical board

## v0.2.0
- [ ] Robust BLE discovery (service-based or explicit address), retry/backoff, connection state tracking
- [ ] JBD individual cell voltage support and more telemetry
- [ ] Port BLE client to TuyaOpen-supported ESP-IDF platform
- [ ] Provision custom PID and TuyaOpen cloud auth safely

## v0.3.0
- [ ] Actual Smart Life device pairing, DP telemetry, offline status
- [ ] Circular SOC gauge in supported Tuya panel
- [ ] Remote alerts and event rate limiting
- [ ] OTA firmware builds and signed release process

## Constraints
The BMS is the safety authority. Missing telemetry must be marked offline; never infer true SOC from guessed values. Do not expose BMS protection writes by default.
