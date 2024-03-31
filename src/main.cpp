#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>
#include <avr/wdt.h> // Include the watchdog library

// Declare Servos
Servo shead;
Servo sbody;

// Declare Global Variables
int body = 0, head = 50;
byte deviceAddress = 0x10;
unsigned int distance;
unsigned int signalStrength;

// Declare Flags
bool headflag = true;

// Declare Functions
void scan(), hflag(), lidar(), printData();

void setup() {
  sbody.attach(5);
  shead.attach(6);

  Wire.begin();
  Serial.begin(115200);
  
  shead.write(head);

  // Enable the watchdog timer with a timeout of 2 seconds
  wdt_enable(WDTO_1S);
}

void loop() {
  scan();
}

// Side-to-side Movement of Servos
void scan() {
  int direction = (body == 0) ? 1 : -1; // Ternary check

  while ((direction == 1 && body < 180) || (direction == -1 && body > 0)) {
    lidar(); // Get Lidar data for each degree
    body = sbody.read();

    sbody.write(sbody.read() + direction);
    delay(5);
  }
  hflag();
}

void hflag() {
  headflag = (head == 50) ? true : ((head == 80) ? false : headflag);
  // Change writes angle to head
  int direction = (headflag == true) ? 10 : -10;

  shead.write(head);
  head += direction;
}

void lidar() {
  // Attempt to read Lidar data
  Wire.beginTransmission(deviceAddress); // The I2C data transmission starts
  Wire.write(0x00); // Send command
  Wire.endTransmission(); // The I2C data transfer is complete

  Wire.requestFrom((uint8_t)deviceAddress, (uint8_t)7); // Read 7 bytes of data

  // Check if data is available
  if (Wire.available() == 7) {
    byte data[7];
    for (int i = 0; i < 7; i++) {
      data[i] = Wire.read(); // Read data into an array
    }

    distance = (data[1] << 8) | data[0];       // DistanceValue
    signalStrength = (data[3] << 8) | data[2]; // Signal strength

    if (distance < 900) {
      printData();
    }
  } else {
    // If I2C communication fails, trigger watchdog timer reset
    wdt_reset(); // Reset the watchdog timer
  }
}

void printData() {
  Serial.print(body);
  Serial.print(",");
  Serial.print(head);
  Serial.print(",");
  Serial.print(distance);
  Serial.print(",");
  Serial.println(signalStrength);
}
