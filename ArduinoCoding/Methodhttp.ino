#include <WiFi.h>
#include <HTTPClient.h>

#define SMDLED 2

const char* ssid     = "UNISANGIL YOPAL";
const char* password = "";

#define GITHUB_RAWURL "https://raw.githubusercontent.com/ElectronicaYopal/Servicios/master/tunneling.md"
String serverip = "";

int i = 0;

void setup() {
  pinMode(SMDLED, OUTPUT);
  digitalWrite(SMDLED, LOW);

  Serial.begin(115200);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    digitalWrite(SMDLED, HIGH);
    delay(250);
    digitalWrite(SMDLED, LOW);
    delay(250);
  }
  serverip = findNgrokIp_inGitHub(GITHUB_RAWURL);
}

void loop() {
  if (WiFi.status() != WL_CONNECTED) {
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
      digitalWrite(SMDLED, HIGH);
      delay(250);
      digitalWrite(SMDLED, LOW);
      delay(250);
    }
    serverip = findNgrokIp_inGitHub(GITHUB_RAWURL);
  }
  else {
    Serial.println(getRequest(serverip+"/v1/DataIn/"+String(i)));
    i++;
  }
  delay(5000);
}

String findNgrokIp_inGitHub(String urlraw) {
  HTTPClient http;

  http.begin(urlraw);

  int httpCode = http.GET();

  String urlis = "";

  if (httpCode > 0) {
    digitalWrite(SMDLED, HIGH);
    if (httpCode == HTTP_CODE_OK) {
      String payload = http.getString();
      byte inx = payload.indexOf("http8080");
      urlis = payload.substring(inx + 10, inx + 38);
    }
  }

  http.end();
  digitalWrite(SMDLED, LOW);
  return urlis;
}

String getRequest(String uri) {
  HTTPClient http;

  http.begin(uri);

  int httpCode = http.GET();

  String payload = "";

  if (httpCode > 0) {
    digitalWrite(SMDLED, HIGH);
    if (httpCode == HTTP_CODE_OK) {
      payload = http.getString();
    }
  }

  http.end();
  digitalWrite(SMDLED, LOW);
  return String(httpCode)+":\n"+payload;
}
