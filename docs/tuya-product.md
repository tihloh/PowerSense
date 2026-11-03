# Tuya / Smart Life product integration

**Not implemented in v0.1.0.** The PlatformIO demo only decodes BLE JBD telemetry and prints to serial. The official TuyaOpen SDK cannot be assumed to work by simply adding a REST key to Arduino firmware.

## Proposed product
- Name: PowerSense
- Connectivity: ESP32 BLE client to BMS; ESP32 Wi-Fi to Tuya Cloud
- UI: round SOC indicator, status line, compact metrics, BMS faults
- Read-only BMS telemetry; app configuration may control only local gateway alert thresholds

## Proposed DP specification (register in Tuya IoT Platform)
| Code | Type | Scale | Units | Access |
| --- | --- | --- | --- | --- |
| battery_soc | value | 0 | % | Report |
| voltage | value | 2 | V | Report |
| current | value or signed representation | 2 | A | Report |
| power | value or signed representation | 0 | W | Report |
| remaining_capacity | value | 2 | Ah | Report |
| cycle_count | value | 0 | cycles | Report |
| bms_status | enum | n/a | normal/charging/discharging/offline/fault | Report |
| bms_alarm | bitmap | n/a | alarms | Report |
| telemetry_age | value | 0 | seconds | Report |

**Important:** Verify each DP's signed range, scale, panel compatibility and permissions in the current Tuya product creator. The table is a proposal, not an approved schema.

## Steps to cloud-ready firmware
1. Create a custom PID and panel in https://platform.tuya.com/.
2. Obtain supported ESP32 provisioning and authorization per official TuyaOpen docs.
3. Port the BLE JBD client to a supported TuyaOpen/ESP-IDF application and retain the portable decoder unchanged.
4. Register DP callbacks and report fresh data only, with throttling to avoid excessive cloud traffic.
5. Pair to Smart Life and test reconnect, offline, invalid checksums, alarm display and OTA on actual hardware.
6. Never hardcode or commit UUID/auth keys. Confirm custom panel availability under the account/product.

Reference: https://github.com/tuya/TuyaOpen
