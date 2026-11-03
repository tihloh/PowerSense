#include <Arduino.h>
#include <NimBLEDevice.h>
#include "../../core/jbd.h"

using powersense::BatteryTelemetry;
static NimBLEClient* client = nullptr;
static NimBLERemoteCharacteristic* writeCharacteristic = nullptr;
static uint8_t rx[128];
static size_t rxLen = 0;
static uint32_t lastValidMs = 0;
static const uint8_t requestBasic[] = {0xDD,0xA5,0x03,0x00,0xFF,0xFD,0x77};

static void onNotify(NimBLERemoteCharacteristic*, uint8_t* bytes, size_t n, bool) {
  // BLE notifications may split/concatenate JBD frames.
  for (size_t i=0;i<n;++i) {
    const uint8_t b=bytes[i];
    if (rxLen==0 && b!=0xDD) continue;
    if (rxLen >= sizeof(rx)) rxLen=0;
    rx[rxLen++]=b;
    if (rxLen>=4) {
      size_t frameLen = static_cast<size_t>(rx[3])+7;
      if (frameLen > sizeof(rx) || frameLen < 7) { rxLen=0; continue; }
      if (rxLen==frameLen) {
        BatteryTelemetry t;
        if (powersense::decodeJbdBasicInfo(rx,rxLen,t)) {
          lastValidMs=millis();
          Serial.printf("SOC %u%% | %.2fV | %.2fA | %.1fW | %.2fAh | cycles %u | alarms 0x%04X\n",
                        t.soc_percent,t.voltage_v,t.current_a,t.power_w,t.remaining_ah,t.cycles,t.protection_bits);
        } else Serial.println("Rejected invalid or unsupported JBD basic-info packet");
        rxLen=0;
      }
    }
  }
}

static bool connectBms() {
  auto* scan = NimBLEDevice::getScan();
  scan->setActiveScan(true);
  NimBLEScanResults results = scan->getResults(5000, false);
  const NimBLEAdvertisedDevice* match = nullptr;
  for (int i=0;i<results.getCount();++i) {
    const auto* d=results.getDevice(i);
    String name=d->getName().c_str(); name.toUpperCase();
    if (name.indexOf("JBD")>=0 || name.indexOf("GENTAI")>=0) { match=d; break; }
  }
  if (!match) { Serial.println("No matching BMS BLE advertisement"); scan->clearResults(); return false; }
  if (!client) client=NimBLEDevice::createClient();
  bool ok=client->connect(match);
  scan->clearResults();
  if (!ok) return false;
  auto* service=client->getService("ff00");
  auto* notify=service ? service->getCharacteristic("ff01") : nullptr;
  writeCharacteristic=service ? service->getCharacteristic("ff02") : nullptr;
  if (!notify || !writeCharacteristic || !notify->canNotify() ||
      !notify->subscribe(true,onNotify)) {
    Serial.println("JBD UUID profile not supported; inspect GATT with nRF Connect");
    client->disconnect();
    writeCharacteristic=nullptr;
    return false;
  }
  Serial.println("BMS connected; read-only polling started");
  return true;
}

void setup() {
  Serial.begin(115200);
  NimBLEDevice::init("PowerSense");
  Serial.println("PowerSense v0.1.0 BLE prototype (NO cloud integration yet)");
}

void loop() {
  static uint32_t lastAttempt=0,lastPoll=0;
  if (!client || !client->isConnected()) {
    if (millis()-lastAttempt > 7000) {
      lastAttempt=millis();
      connectBms();
    }
    return;
  }
  if (writeCharacteristic && millis()-lastPoll > 2000) {
    lastPoll=millis();
    if (writeCharacteristic->canWrite())
      writeCharacteristic->writeValue(requestBasic,sizeof(requestBasic),true);
    else if (writeCharacteristic->canWriteNoResponse())
      writeCharacteristic->writeValue(requestBasic,sizeof(requestBasic),false);
  }
  if (lastValidMs && millis()-lastValidMs > 10000) {
    static uint32_t lastWarning=0;
    if (millis()-lastWarning > 10000) {
      lastWarning=millis();
      Serial.println("BMS telemetry STALE — do not display as live");
    }
  }
}
