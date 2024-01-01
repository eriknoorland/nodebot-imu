#include <Arduino.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BNO055.h>
#include <PacketSerial.h>

#define IMU_RESET_PIN 3

#define REQUEST_START_FLAG 0xA6
#define REQUEST_IS_READY 0x10
#define REQUEST_RESET_IMU 0x20

#define RESPONSE_START_FLAG_1 0xA6
#define RESPONSE_START_FLAG_2 0x6A
#define RESPONSE_DATA 0x30
#define RESPONSE_READY 0xFF

const int loopTime = 20; // ms
unsigned long previousTime = 0;
bool isReady = false;
bool isImuDetected = false;

Adafruit_BNO055 bno = Adafruit_BNO055(55);
PacketSerial serial;

void handleImuResponse() {
  if (isImuDetected) {
    byte headingPartMsb = 0;
    byte headingPartLsb = 0;

    sensors_event_t orientationData;
    bno.getEvent(&orientationData, Adafruit_BNO055::VECTOR_EULER);

    int heading = orientationData.orientation.x * 100;
    headingPartMsb = heading >> 8;
    headingPartLsb = heading;

    uint8_t response[6] = {
      RESPONSE_START_FLAG_1,
      RESPONSE_START_FLAG_2,
      RESPONSE_DATA,
      0x02,
      headingPartMsb,
      headingPartLsb
    };

    serial.send(response, sizeof(response));
  }
}

void reset() {
  isReady = false;
  isImuDetected = false;

  digitalWrite(IMU_RESET_PIN, LOW);
  delayMicroseconds(1);
  digitalWrite(IMU_RESET_PIN, HIGH);

  if (bno.begin()) {
    isImuDetected = true;
    isReady = true;
  }
}

void handleReady() {
  reset();

  uint8_t readyResponse[5] = {
    RESPONSE_START_FLAG_1,
    RESPONSE_START_FLAG_2,
    RESPONSE_READY,
    0x01,
    isImuDetected
  };

  serial.send(readyResponse, sizeof(readyResponse));
  isReady = true;
}

void onPacketReceived(const uint8_t* buffer, size_t size) {
  byte startFlag = buffer[0];
  byte command = buffer[1];

  if (startFlag == REQUEST_START_FLAG) {
    switch (command) {
      case REQUEST_IS_READY: {
        handleReady();
        break;
      }

      case REQUEST_RESET_IMU: {
        reset();
        break;
      }
    }
  }
}

void setup() {
  SerialUSB.begin(115200);
  serial.setStream(&SerialUSB);
  serial.setPacketHandler(&onPacketReceived);

  pinMode(IMU_RESET_PIN, OUTPUT);
  digitalWrite(IMU_RESET_PIN, HIGH);

  if (bno.begin()) {
    isImuDetected = true;
  }

  while (!SerialUSB);

  handleReady();
}

void loop() {
  serial.update();

  long now = millis();
  int deltaTime = now - previousTime;

  if (isReady && deltaTime >= loopTime) {
    handleImuResponse();
    previousTime = now;
  }
}
