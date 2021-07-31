#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#define LED 2

#include <TinyGPS.h>
TinyGPS gps;

void setup() {
  pinMode(LED, OUTPUT);
  digitalWrite(LED, LOW);
  SerialBT.begin("ESP32_LoRa_BT");
  while (!SerialBT.available()) {
    digitalWrite(LED, !digitalRead(LED));
    delay(250);
  }
  digitalWrite(LED, LOW);
  delay(1000);
  SerialBT.println("Bienvenidos");
  Serial2.begin(9600);

}

void loop() {
  while (Serial2.available())
  {
    char c = Serial2.read();
    //SerialBT.write(c);
    if (gps.encode(c)){
      long Latitud, Longitud, Altitud;
      unsigned long Date, Time, age;
      gps.get_datetime(&Date, &Time, &age);
      gps.get_position(&Latitud, &Longitud, &age);
      Altitud = gps.altitude();

      String GPSData = String(Date) + " " + String(Time) + " " + String(Latitud) + " " + String(Longitud) + " " + String(Altitud);
      
      SerialBT.println(GPSData);
      delay(500);
    }
  }
}
