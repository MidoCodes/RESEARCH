#include <Arduino.h>
#include <Servo.h>
#include <Wire.h>

//Lidar Data Struct
typedef struct {
  int distance;
  int strength;
  int temp;
  boolean receiveComplete;
} TF;

TF Lidar = {0, 0, 0, false};

//Declare Servos
Servo servoHead;
Servo servoBody;

//Declare Variables
int body = 0;
int head = 40;

//Declare Flags
bool headFlag = true;

//Declare Functions
void getLidarData(TF* Lidar);
void headMovement();

//Functions
void headMovement(){
  headFlag = (head == 40) ? true : ((head == 180) ? false : headFlag);
 
  int direction = (headFlag == true) ? 10 : -10;

  servoHead.write(head);
  head += direction;
}

void getLidarData(TF* lidar) 
{
  static char i = 0;
  char j = 0;
  int checksum = 0;
  static int rx[9];
  if (Serial.available()) 
  {
    rx[i] = Serial.read();
    if (rx[0] != 0x59) 
    {
      i = 0;
    } 
    else if (i == 1 && rx[1] != 0x59) {
      i = 0;
    } 
    else if (i == 8) 
    {
      for (j = 0; j < 8; j++) 
      {
        checksum += rx[j];
      }
      if (rx[8] == (checksum % 256)) 
      {
          lidar->distance = rx[2] + rx[3] * 256;
          lidar->strength = rx[4] + rx[5] * 256;
          lidar->temp = (rx[6] + rx[7] * 256) / 8 - 256;
          lidar->receiveComplete = true;
      }
      i = 0;
    } 
    else 
    {
      i++;
    }
  }
}

void setup() {
  Serial.begin(115200);
  servoBody.attach(3);
  servoHead.attach(5);
}

void loop() 
{
  int direction = (body == 0) ? 1 : -1; // Ternary check

    while ((direction == 1 && body < 180) || (direction == -1 && body > 0)) {

      getLidarData(&Lidar);       //Acquisition of radar data
        if (Lidar.receiveComplete) 
        {
          Lidar.receiveComplete = false;
          Serial.print(body);
          Serial.print(",");
          Serial.print(head);
          Serial.print(",");
          Serial.println(Lidar.distance);

           body = servoBody.read();
          servoBody.write(servoBody.read() + direction);
          delayMicroseconds(500);
        }

    }
    
  headMovement();
}