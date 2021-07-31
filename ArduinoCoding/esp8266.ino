#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>

#define SERVER_IP "10.90.89.90:8080"

#ifndef STASSID
#define STASSID "UNISANGIL YOPAL"
#define STAPSK  ""
#endif

#include <SPI.h>              // include libraries
#include <LoRa.h>

const long frequency = 915E6;  // LoRa Frequency

const int csPin = D10;          // LoRa radio chip select
const int resetPin = D9;        // LoRa radio reset
const int irqPin = D8;          // change for your board; must be a hardware interrupt pin

String messageA = "";
bool ok = false;

void setup() {
  Serial.begin(9600);                   // initialize serial
  while (!Serial);

  WiFi.begin(STASSID, STAPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("Connected! IP address: ");
  Serial.println(WiFi.localIP());

  LoRa.setPins(csPin, resetPin, irqPin);

  if (!LoRa.begin(frequency)) {
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
  Serial.println();
  Serial.println("LoRa Simple Gateway");
  Serial.println("Only receive messages from nodes");
  Serial.println("Tx: invertIQ enable");
  Serial.println("Rx: invertIQ disable");
  Serial.println();

  LoRa.onReceive(onReceive);
  LoRa.onTxDone(onTxDone);
  LoRa_rxMode();
}

void loop() {
  if (runEvery(5000)) { // repeat every 5000 millis

    int httpCode = 0;

    if ((WiFi.status() == WL_CONNECTED)) {

      WiFiClient client;

      HTTPClient http;

      if (http.begin(client, "http://10.90.89.90:8080/v1/DataIn/1")) {  // HTTP

        httpCode = http.GET();

        http.end();
      } else {
        httpCode = -9;
      }
    }

    LoRa_sendMessage("AreYouListening? Http: " + String(httpCode)); // send a message

    Serial.println("Send Message!");
  }
  if (ok) {
    ok = false;
    if ((WiFi.status() == WL_CONNECTED)) {

      WiFiClient client;
      HTTPClient http;

      Serial.print("[HTTP] begin...\n");
      Serial.println("http://" SERVER_IP "/v1/DataIn/");
      // configure traged server and url
      http.begin(client, "http://" SERVER_IP "/v1/DataIn/"); //HTTP
      http.addHeader("Content-Type", "application/json");

      Serial.print("[HTTP] POST...\n");
      Serial.println("{\"Text\":\"" + messageA + "\"}");
      // start connection and send HTTP header and body
      int httpCode = http.POST("{\"Text\":\"" + messageA + "\"}");
      Serial.println(httpCode);
      // httpCode will be negative on error
      if (httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[HTTP] POST... code: %d\n", httpCode);

        // file found at server
        if (httpCode == HTTP_CODE_OK) {
          const String& payload = http.getString();
          Serial.println("received payload:\n<<");
          Serial.println(payload);
          Serial.println(">>");
        }
      } else {
        Serial.printf("[HTTP] POST... failed, error: %s\n", http.errorToString(httpCode).c_str());
      }

      http.end();
    }
  }
}

void LoRa_rxMode() {
  LoRa.disableInvertIQ();               // normal mode
  LoRa.receive();                       // set receive mode
}

void LoRa_txMode() {
  LoRa.idle();                          // set standby mode
  LoRa.enableInvertIQ();                // active invert I and Q signals
}

void LoRa_sendMessage(String message) {
  LoRa_txMode();                        // set tx mode
  LoRa.beginPacket();                   // start packet
  LoRa.print(message);                  // add payload
  LoRa.endPacket(true);                 // finish packet and send it
}

void onReceive(int packetSize) {

  messageA = "";

  while (LoRa.available()) {
    messageA += (char)LoRa.read();
  }

  messageA = messageA + ", " + String(LoRa.packetRssi()) + ", " + String(LoRa.packetSnr());

  Serial.print("Gateway Receive: ");
  Serial.println(messageA);

  ok = true;
}

void onTxDone() {
  Serial.println("TxDone");
  LoRa_rxMode();
}

boolean runEvery(unsigned long interval)
{
  static unsigned long previousMillis = 0;
  unsigned long currentMillis = millis();
  if (currentMillis - previousMillis >= interval)
  {
    previousMillis = currentMillis;
    return true;
  }
  return false;
}
