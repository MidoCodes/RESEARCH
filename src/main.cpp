#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

// Declare Servos
Servo shead;
Servo sbody;

// Declare Global Variables
int body = 0, head = 0;
int maxbody, maxhead;
byte deviceAddress = 0x10;

// Declare Flags
bool headflag = true;
bool scanning = false;

// Declare Functions
void scan(), hflag(), lidar();


void setup() {
  sbody.attach(5);
  shead.attach(6);

  Wire.begin();
  Serial.begin(115200);

}

void loop() {
  scan();
}

// Side-to-side Movement of Servos
void scan() {
  int direction = (body == 0) ? 1 : -1; // Ternary check

  while ((direction == 1 && body < 180) || (direction == -1 && body > 0)) {
    lidar(); // Get Lidar data for each degree
    delay(5);
    body = sbody.read();

    Serial.print(body);
    Serial.print(",");
    Serial.print(head);
    Serial.print(",");

    sbody.write(sbody.read() + direction);
    delay(5);
  }
  hflag();
}

void hflag() {
  headflag = (head == 0) ? true : ((head == 80) ? false : headflag);
  // Headflag is the second argument so when head != maxhead, it still retains its false bool

  // Change writes angle to head
  int direction = (headflag == true) ? 10 : -10;

  shead.write(head);
  head += direction;
}

void lidar() {
  Wire.beginTransmission(deviceAddress); // The I2C data transmission starts
  Wire.write(0x00); // Send command
  Wire.endTransmission(); // The I2C data transfer is complete

  Wire.requestFrom((uint8_t)deviceAddress, (uint8_t)7); // Read 7 bytes of data
  if (Wire.available() == 7) { // 7 bytes of data are available
    byte data[7];
    for (int i = 0; i < 7; i++) {
      data[i] = Wire.read(); // Read data into an array
    }

    unsigned int distance = (data[1] << 8) | data[0];       // DistanceValue
    unsigned int signalStrength = (data[3] << 8) | data[2]; // Signal strength

    Serial.print(distance);
    Serial.print(",");
    Serial.println(signalStrength);
  }
}