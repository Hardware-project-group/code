#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <Keypad.h>
#include "Finger_check.h"
#include "enroll.h"
#include "wifi_setup.h"
#include "keyCode.h"




HardwareSerial serialPort(2); // use UART2

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

uint8_t id;
uint8_t fingerId;
bool passcodeVerified = false;
String passcode = "";
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

while (true) {
    Serial.println("Enter the password:");
    passcode = ""; // Clear passcode
    passcodeVerified = false; // Reset passcode verification flag
    while (!passcodeVerified) {
        if (kpd.getKeys()) {
            for (int i = 0; i < LIST_MAX; i++) {
                if (kpd.key[i].stateChanged && kpd.key[i].kstate == PRESSED) {
                    Serial.println(kpd.key[i].kchar);
                    if (kpd.key[i].kchar == '#') {
                        if (passcode == "8504") {
                            Serial.println("Passcode OK!");
                            while(getFingerprintID() == 2)
                              ;
                            passcodeVerified = true; // Set flag to true to exit passcode entry loop
                            break; // Exit passcode entry loop
                        } else {
                            Serial.println("Invalid Password! Try again.");
                            passcode = "";
                            break;
                        }
                    } else {
                        passcode += kpd.key[i].kchar;
                    }
                }
            }
        }
        delay(100); // Add a small delay to avoid busy loop
    }
}

  // Serial.println("Please enter : 1 to check fingerprint or enter : 2 for enroll new fingerprint");
  // //id = readnumber();
  // delay(100);
  // if(kpd.getKeys()){
  //   for(int i = 0; i < LIST_MAX; i++){
  //     if(kpd.key[i].stateChanged && kpd.key[i].kstate == PRESSED){
  //       Serial.println(kpd.key[i].kchar);
  //         if ( kpd.key[i].kchar == '2') {
  //           Serial.println("Ready to enroll a fingerprint!");
  //           Serial.println("Please type in the ID # (from 1 to 127) you want to save this finger as...");
  //           fingerId = idForFingerprint();
  //           if (id == 0){ // ID #0 not allowed, try again!
  //             return;
  //           }
  //           Serial.print("Enrolling ID #");
  //           Serial.println(fingerId);

  //           while (!getFingerprintEnroll(fingerId))
  //             ;
  //         }else if(kpd.key[i].kchar == '1'){
  //           while(getFingerprintID() == 2)
  //             ;
  //         }else{
  //           Serial.println("Invalid Input");
  //         }
  //     }
  //   }
  //   passcode = "";
  //   passcodeVerified = false;
  // }
  
 
}

