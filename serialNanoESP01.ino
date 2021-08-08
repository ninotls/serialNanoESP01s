#include <DHT.h>
#include <SoftwareSerial.h>

#define DHTPIN 2
#define DHTTYPE DHT22 // DHT 22 (AM2302), AM2321

#define RX 10
#define TX 11
String WIFI_SSID = "MyNetwork";       // Your WiFi ssid
String PASSWORD = "XXXXXXXX";         // Password

String HOST = "api.thingspeak.com";
String PATH = "/update?api_key=";
String writeAPIKey = "XXXXXXXX";
String PORT = "80";

int countTrueCommand;
int countTimeCommand;
boolean found = false;

SoftwareSerial esp8266(RX, TX);
DHT dht(DHTPIN, DHTTYPE);

String str;

void setup() {
  Serial.begin(9600);
  esp8266.begin(115200);
  esp8266.println("AT");
  Serial.println(esp8266.read());
  sendCommandToESP8266("AT", 5, "OK");
  sendCommandToESP8266("AT+CWMODE=1", 5, "OK");
  sendCommandToESP8266("AT+CWJAP=\"" + WIFI_SSID + "\",\"" + PASSWORD + "\"", 20, "OK");
  
  dht.begin();
  delay(2000);
}

void loop()
{
  float h = dht.readHumidity();
  // Read temperature as Celsius (the default)
  float t = dht.readTemperature();
  Serial.print("H: ");
  Serial.print(h);
  Serial.print("% ");
  Serial.print(" T: ");
  Serial.print(t);
  Serial.println("C");

  String dataValues = "field1=" + String(t) + "&field2=" + String(h);

  String request = "GET " + PATH + writeAPIKey + "&" + dataValues + "\r\n";
  
  sendCommandToESP8266("AT+CIPMUX=0", 5, "OK");
  sendCommandToESP8266("AT+CIPSTART=\"TCP\",\"" + HOST + "\"," + PORT, 15, "OK");
  String cipSend = "AT+CIPSEND=" + String(request.length());
  sendCommandToESP8266(cipSend, 4, ">");
  sendData(request);
  sendCommandToESP8266("AT+CIPCLOSE", 5, "OK");
  delay(5000);
}

void sendCommandToESP8266(String command, int maxTime, char readReplay[]) {
  Serial.print(countTrueCommand);
  Serial.print(". at command => ");
  Serial.print(command);
  Serial.print(" ");
  while (countTimeCommand < (maxTime * 1))
  {
    esp8266.println(command);
    if (esp8266.find(readReplay))
    {
      found = true;
      break;
    }

    countTimeCommand++;
  }

  if (found == true)
  {
    Serial.println("Success");
    countTrueCommand++;
    countTimeCommand = 0;
  }

  if (found == false)
  {
    Serial.println("Fail");
    countTrueCommand = 0;
    countTimeCommand = 0;
  }

  found = false;
}

void sendData(String postRequest) {
  Serial.println(postRequest);
  esp8266.println(postRequest);
  delay(1500);
  countTrueCommand++;
}
