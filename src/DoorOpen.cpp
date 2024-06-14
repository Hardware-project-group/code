#include <Arduino.h>


void OpenDoor(){

    digitalWrite(2, HIGH);
    delay(5000);
    digitalWrite(2, LOW);
}