/*Please read the instruction below to understand the flow of this project
  Like, Share and Subscribe our Youtube Channel for complete tutorial:
  http://youtube.com/c/kajidataonline

  Download following library: https://github.com/adafruit/Adafruit_PM25AQI
  Watch The Tutorial and Tricks for this sketch here:
  1) Setup of the project : 
  2) Discussion :
*/

#include "Adafruit_PM25AQI.h"
#include <ESP8266WiFi.h>
#include <SoftwareSerial.h>
SoftwareSerial pmSerial(2, 3);

// replace with your channel’s thingspeak API key and your SSID and password
String apiKey = "YOUR_API_THINGSPEAK";
const char* ssid = "YOUR_SSID";
const char* password = "YOUR_PASSWORD";
const char* server = "api.thingspeak.com";
WiFiClient client;
Adafruit_PM25AQI aqi = Adafruit_PM25AQI();

void setup()
{
  Serial.begin(115200);
  while (!Serial) delay(10);
  WiFi.begin(ssid, password);
  Serial.println("Adafruit PMSA003I Air Quality Sensor");
  delay(1000);
  pmSerial.begin(9600);

  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  if (! aqi.begin_UART(&pmSerial)) { // connect to the sensor over software serial
    Serial.println("Could not find PM 2.5 sensor!");
    while (1) delay(10);
  }

  Serial.println("PM25 found!");
}

void loop()
{
  PM25_AQI_Data data;
  if (! aqi.read(&data)) {
    Serial.println("Could not read from AQI");
    delay(500);  // try again in a bit!
    return;
  }
  Serial.println("AQI reading success");

  float pm03 = data.particles_03um;
  float pm05 = data.particles_05um;
  float pm1 = data.particles_10um;
  float pm25 = data.particles_25um;
  float pm5 = data.particles_50um;
  float pmten = data.particles_100um;
  if (isnan(pm03) || isnan(pm05) || isnan(pm1) || isnan (pm25) || isnan (pm5) || isnan (pmten))
  {
    Serial.println("Failed to read from PMSA003 Sensor!!!");
    return;
  }

  if (client.connect(server, 80)) {
    String postStr = apiKey;
    postStr += "&field1=";
    postStr += String(pm03);
    postStr += "&field2=";
    postStr += String(pm05);
    postStr += "&field3=";
    postStr += String(pm1);
    postStr += "&field4=";
    postStr += String(pm25);
    postStr += "&field5=";
    postStr += String(pm5);
    postStr += "&field6=";
    postStr += String(pmten);
    postStr += "\r\n\r\n"; // Insert this to return to original (you can add more if you wish)

    client.print("POST /update HTTP/1.1\n");
    client.print("Host: api.thingspeak.com\n");
    client.print("Connection: close\n");
    client.print("X-THINGSPEAKAPIKEY: " + apiKey + "\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);

    Serial.println();
    Serial.println(F("---------------------------------------"));
    Serial.println(F("Concentration Units (standard)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_standard);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_standard);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_standard);
    Serial.println(F("Concentration Units (environmental)"));
    Serial.println(F("---------------------------------------"));
    Serial.print(F("PM 1.0: ")); Serial.print(data.pm10_env);
    Serial.print(F("\t\tPM 2.5: ")); Serial.print(data.pm25_env);
    Serial.print(F("\t\tPM 10: ")); Serial.println(data.pm100_env);
    Serial.println(F("---------------------------------------"));
    Serial.print(F("Particles > 0.3um / 0.1L air:")); Serial.println(data.particles_03um);
    Serial.print(F("Particles > 0.5um / 0.1L air:")); Serial.println(data.particles_05um);
    Serial.print(F("Particles > 1.0um / 0.1L air:")); Serial.println(data.particles_10um);
    Serial.print(F("Particles > 2.5um / 0.1L air:")); Serial.println(data.particles_25um);
    Serial.print(F("Particles > 5.0um / 0.1L air:")); Serial.println(data.particles_50um);
    Serial.print(F("Particles > 10 um / 0.1L air:")); Serial.println(data.particles_100um);
    Serial.println(F("---------------------------------------"));
  }
  client.stop();

  Serial.println("Waiting 1 minutes");
  delay(60000);
}
