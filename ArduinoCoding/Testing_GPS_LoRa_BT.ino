#include "BluetoothSerial.h"
BluetoothSerial SerialBT;

#define LED 2

#include <TinyGPS.h>
TinyGPS gps;

#include <SPI.h>
#include <LoRa.h>

#define csPin 5
#define resetPin 4
#define irqPin 15

const long frequency = 915E6; 

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

  LoRa.setPins(csPin, resetPin, irqPin);
  if (!LoRa.begin(frequency)){
    SerialBT.println("Falló conexión Shield LoRa");
  }

  SerialBT.println("Shield LoRa Ok");

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();

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
      LoRa_sendMessage(GPSData);
      delay(500);
    }
  }
}

void LoRa_rxMode(){
  LoRa.enableInvertIQ();                // active invert I and Q signals
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode(){
  LoRa.idle();                          // set standby mode
  LoRa.disableInvertIQ();               // normal mode
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {
  String message = "";

  while (LoRa.available()) {
    message += (char)LoRa.read();
  }

  Serial.print("Node Receive: ");
  Serial.println(message);
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}
