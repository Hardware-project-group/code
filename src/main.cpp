//Global Section
#include <Arduino.h>
#include <Adafruit_Fingerprint.h>
#include <WiFi.h>
#include <Keypad.h>
#include <Arduino_JSON.h>
#include <HTTPClient.h>
#include <WebServer.h>

//Header Files
#include "Finger_check.h"
#include "enroll.h"
#include "wifi_setup.h"
#include "keyCode.h"
#include "Display.h"

HardwareSerial serialPort(2); // use UART2

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&serialPort);

uint8_t id;
uint8_t fingerId;
bool passcodeVerified = false;
String passcode = "";

WebServer server(80);

void handleFingerprint();

void setup() {
  Serial.begin(9600);
  Displaysetup();
  connectWiFi();
  
  while (!Serial) {
    ; // Wait for the serial port to connect
  }
  
  delay(100);
  Serial.println("\n\nAdafruit Fingerprint sensor enrollment");

  // Initialize the fingerprint sensor
  finger.begin(57600);

  if (finger.verifyPassword()) {
    Serial.println("Found fingerprint sensor!");
  } else {
    Serial.println("Did not find fingerprint sensor :(");
    while (1) {
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
  pinMode(2, OUTPUT);
  digitalWrite(2, LOW);
  
  

  server.on("/enroll", HTTP_POST, handleFingerprint);
  server.begin();
  Serial.println(WiFi.localIP());
}

void handleFingerprint() {
  Serial.println("Handling fingerprint request...");
  if (server.hasArg("fingerID")) {
    lcd.clear();
    lcd.print("Start Enrolling Fingerprint");
    String paramValueStr = server.arg("fingerID");
    int paramValue = paramValueStr.toInt();
    // Process the parameter value (e.g., print to Serial)
    Serial.print("Received parameter value: ");
    Serial.println(paramValue);
    
    int enrollStatus = -1;
    while (enrollStatus != 1) {
      // Attempt to enroll the fingerprint
      lcd.clear();
      lcd.print("Enrolling fingerprint...");
      enrollStatus = getFingerprintEnroll(paramValue);
      if(enrollStatus != 1){
        lcd.print("Sometime went wrong..Try again!");
      }
      delay(2000); // Add a small delay between enrollment attempts
    }
    
    // Send a response to the client
    server.send(200, "text/plain", "POST request processed successfully");
  } else {
    // No parameters were sent or an error occurred
    server.send(400, "text/plain", "Bad Request");
  }
  lcd.clear();
  lcd.print("Enter the passcode");
}



uint8_t readnumber(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

uint8_t idForFingerprint(void) {
  uint8_t num = 0;

  while (num == 0) {
    while (!Serial.available());
    num = Serial.parseInt();
  }
  return num;
}

void loop() {
  
  lcd.clear();
  lcd.print("Connecting");
  if (WiFi.status() != WL_CONNECTED) {
    connectWiFi();
  }
  Serial.println("Welcome to StockSync!");
  lcd.clear();
  lcd.println("Welcome to StockSync!");
  delay(3000);

  while (true) {
    Serial.println("Enter the password:");
    lcd.clear();
    lcd.print("Enter the password:");
    passcode = ""; // Clear passcode
    passcodeVerified = false; // Reset passcode verification flag
    while (!passcodeVerified) {
      server.handleClient();
      if (kpd.getKeys()) {
        for (int i = 0; i < LIST_MAX; i++) {
          if (kpd.key[i].stateChanged && kpd.key[i].kstate == PRESSED) {
            Serial.println(kpd.key[i].kchar);
            if (kpd.key[i].kchar == '#') {
              if (passcode == "8504") {
                Serial.println("Passcode OK!");
                lcd.clear();
                lcd.print("Passcode OK!");
                while (getFingerprintID() == 2);
                passcodeVerified = true; // Set flag to true to exit passcode entry loop
                break; // Exit passcode entry loop
              } else {
                Serial.println("Invalid Password! Try again.");
                lcd.clear();
                lcd.print("Invalid Passcode");
                passcode = "";
                break;
              }
            } else {
              passcode += kpd.key[i].kchar;
              lcd.clear();
              lcd.print(passcode);
            }
          }
        }
      }
      delay(100); // Add a small delay to avoid busy loop
    }
  }
}

  