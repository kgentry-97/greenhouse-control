#include <DHT.h>
#include <SPI.h>
#include <WiFi101.h>
#include <AdafruitIO.h>
#include "AdafruitIO_WiFi.h"
#include <SerLCD.h>
#include <Wire.h>

#include "secrets.h"
//temp setup
#define DHTPIN 1
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);
#define DISPLAY_ADDRESS1 0x72 //This is the default address of the OpenLCD

char ssid[] = SECRET_SSID;
char pass[] = SECRET_PASS;
int status = WL_IDLE_STATUS;
int statusTime = 0;

//adafruit io setup
AdafruitIO_WiFi io(SECRET_IO_USERNAME, SECRET_IO_KEY, SECRET_SSID, SECRET_PASS);
AdafruitIO_Feed *temperature = io.feed("temperature");
AdafruitIO_Feed *humidity = io.feed("humidity");
AdafruitIO_Feed *heatIndex = io.feed("heatIndex");

SerLCD lcd;
int i = 0;

void setup() {
  Serial.begin(19200);
  Serial.println("DHTxx test!");
  dht.begin();
  Wire.begin();
  lcd.begin(Wire);


  lcd.clear();

  //wifi connection
  while ( status != WL_CONNECTED) {

    Serial.print("Attempting to connect to WPA SSID: ");
    Serial.println(ssid);
    // Connect to WPA/WPA2 network:
    status = WiFi.begin(ssid, pass);

    // wait 10 seconds for connection:
    delay(10000);
  }

  Serial.print("You're connected to the network");
  printWiFiData();

  ioConnect();

}

void loop() {
  io.run();
  while (i >=  9)
  {
    //statusTime++;
    float h = dht.readHumidity();
    float f = dht.readTemperature(true);
    float hif = dht.computeHeatIndex(f, h);

    // Check if any reads failed and exit early (to try again).
    if (isnan(h) || isnan(f)) {
      Serial.println("Failed to read from DHT sensor!");
      return;
    }
    tempPrint(h, f, hif);
    i2cSendValue(f, h);
    humidity->save(h);
    temperature->save(f);
    heatIndex->save(hif);
    i = 0;
  }
  delay(1000);
  i++;

}

void tempPrint(float h, float f, float hif) {
  Serial.print("Humidity: ");
  Serial.print(h);
  Serial.print(" %\t");
  Serial.print("Temperature: ");
  Serial.print(f);
  Serial.print(" *F\t");
  Serial.print("Heat index: ");
  Serial.print(hif);
  Serial.println(" *F");
}

void printWiFiData() {
  Serial.print("SSID: ");
  Serial.println(WiFi.SSID());
  IPAddress ip = WiFi.localIP();
  Serial.print("IP Address: ");
  Serial.println(ip);
  long rssi = WiFi.RSSI();
  Serial.print("signal strength (RSSI):");
  Serial.println(rssi);
}

void ioConnect() {
  // connect to io.adafruit.com
  Serial.print("Connecting to Adafruit IO\n");
  io.connect();

  // wait for a connection
  while (io.status() < AIO_CONNECTED) {
    Serial.println(io.status());
    //Serial.print(".");
    delay(500);
  }

  // we are connected
  Serial.println();
  Serial.println(io.statusText());
  lcd.print(io.statusText());
}

void i2cSendValue(float temp, float hum )
{

  lcd.clear();

  lcd.print("Temp: ");
  lcd.print(temp);
  lcd.setCursor(0, 1); //colum, row
  lcd.print("Hum: ");
  lcd.print(hum);


}
