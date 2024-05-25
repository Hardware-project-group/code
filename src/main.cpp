#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include "Finger_check.h"
#include "enroll.h"
#include "wifi_setup.h"




HardwareSerial serialPort(2); // use UART2

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

uint8_t id;
uint8_t fingerId;
void connectWiFi();

void setup()
{
  Serial.begin(9600);
  connectWiFi();
  while (!Serial)
    ; // For Yun/Leo/Micro/Zero/...
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // set the data rate for the sensor serial port
  finger.begin(57600);

  if (finger.verifyPassword())
  {
    Serial.println("Found fingerprint sensor!");
  }
  else
  {
    Serial.println("Did not find fingerprint sensor :(");
    while (1)
    {
      delay(1);
    }
  }

  Serial.println(F("Reading sensor parameters"));
  finger.getParameters();
  Serial.print(F("Status: 0x"));
  Serial.println(finger.status_reg, HEX);
  Serial.print(F("Sys ID: 0x"));
  Serial.println(finger.system_id, HEX);
  Serial.print(F("Capacity: "));
  Serial.println(finger.capacity);
  Serial.print(F("Security level: "));
  Serial.println(finger.security_level);
  Serial.print(F("Device address: "));
  Serial.println(finger.device_addr, HEX);
  Serial.print(F("Packet len: "));
  Serial.println(finger.packet_len);
  Serial.print(F("Baud rate: "));
  Serial.println(finger.baud_rate);
}
uint8_t readnumber(void)
{
  uint8_t num = 0;

  while (num == 0 )
  {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

uint8_t idForFingerprint(void)
{
  uint8_t num = 0;

  while (num == 0 )
  {
    while (!Serial.available())
      ;
    num = Serial.parseInt();
  }
  return num;
}

void loop(){


  if(WiFi.status() != WL_CONNECTED){
    connectWiFi();
  }
  

  Serial.println("Welcome to StockSync!");
  Serial.println("Please enter : 1 to check fingerprint or enter : 2 for enroll new fingerprint");
  id = readnumber();
  if (id == 2) {
    Serial.println("Ready to enroll a fingerprint!");
    Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
    fingerId = idForFingerprint();
    if (id == 0){ // ID #0 not allowed, try again!
      return;
    }
    Serial.print("Enrolling ID #");
    Serial.println(fingerId);

    while (!getFingerprintEnroll(fingerId))
      ;
  }else if(id == 1){
    while(getFingerprintID() == 2)
      ;
  }else{
    Serial.println("Invalid Input");
  }
}

