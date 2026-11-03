# PowerSense

Open-source ESP32 smart battery monitor for Bluetooth BMS devices, with a planned custom Tuya / Smart Life product.

> **Status: v0.1.0 development foundation.** A real JBD frame decoder and a BLE gateway prototype are included. Tuya cloud pairing, a Smart Life panel, and hardware compatibility are **not** yet implemented or verified.

## Goals
- Read real telemetry from Gentai/JBD-compatible BLE BMS units.
- Publish read-only battery data to a custom Tuya product using the official TuyaOpen SDK (future integration).
- Display a circular state-of-charge gauge with voltage, current, power, temperature, capacity, cycle count, and alarms.
- Reconnect safely and mark disconnected/stale measurements as unavailable.
- Add other BMS protocol drivers without changing the telemetry schema.

## Repository layout
- `firmware/`: PlatformIO ESP32 BLE proof-of-concept (serial output only for now).
- `core/`: portable validated JBD frame parser and telemetry model.
- `tests/`: native parser tests that run without hardware.
- `docs/tuya-product.md`: custom Tuya product / DP design and integration checklist.
- `.github/workflows/ci.yml`: automated native test and ESP32 compile.
- `docs/roadmap.md`: implementation stages and known limitations.

## ESP32 quick start
1. Install VS Code and PlatformIO.
2. Open the repository root, then run `pio run -d firmware`.
3. Flash with `pio run -d firmware -t upload` and monitor at 115200 baud.
4. Start the battery's BLE advertisement and review the serial log. The scanner selects only a BLE device whose advertised name matches `JBD` or `GENTAI`. Modify the filter for your verified BMS.
5. **Do not use the output for charge/discharge safety decisions.** BLE UUIDs, permissions and payloads vary between JBD variants and may need adjustment after live captures.

## Tuya integration
The official [TuyaOpen](https://github.com/tuya/TuyaOpen) project supports ESP32, but its cloud stack is **not** linked into this PlatformIO BLE test app yet. See [custom product setup](docs/tuya-product.md). Product registration, device UUID/auth key and panel setup are required; never commit keys.

## Safety and privacy
- Read-only by design: no BMS MOSFET writes, protection overrides, or inverter control.
- Never publish device authorization keys, Wi-Fi passwords or private BLE traffic.
- Stale measurements must not be presented as live values.
- No simulated SOC readings are sent to an inverter or cloud.

## License
MIT; third-party libraries keep their own licenses.
