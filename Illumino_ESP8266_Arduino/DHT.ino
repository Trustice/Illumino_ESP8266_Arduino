#include "DHT.h"

// Uncomment whatever type you're using!
//#define DHTTYPE DHT11   // DHT 11
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
//#define DHTTYPE DHT21   // DHT 21 (AM2301)

// Connect pin 1 (on the left) of the sensor to +5V
// NOTE: If using a board with 3.3V logic like an Arduino Due connect pin 1
// to 3.3V instead of 5V!
// Connect pin 2 of the sensor to whatever your DHTPIN is
// Connect pin 4 (on the right) of the sensor to GROUND
// Connect a 10K resistor from pin 2 (data) to pin 1 (power) of the sensor

// Initialize DHT sensor.
DHT dht(PIN_DHT, DHTTYPE);

void dhtSetup() {
  Serial.println(PIN_DHT);
  dht.begin();
  sendDHTdata(dht_temp_uuid, dhtReadTemp());
  sendDHTdata(dht_humid_uuid, dhtReadHumidity());
}

long dhtLastUpdate = 0;
void dhtUpdate() {
  if (millis() - dhtLastUpdate >= DHT_INTERVAL){
    dhtSend();
  }
}

void dhtSend() {
  sendDHTdata(dht_temp_uuid, dhtReadTemp());
  sendDHTdata(dht_humid_uuid, dhtReadHumidity());
  dhtLastUpdate = millis();
}

float dhtReadTemp() {
  float t = dht.readTemperature();

  // Check if any reads failed and exit early (to try again).
  if (isnan(t)){
    Serial.println("Failed to read from DHT sensor!");
    //return 99;
  }
  return t;
}

float dhtReadHumidity() {
  float h = dht.readHumidity();

  // Check if any reads failed and exit early (to try again).
  if (isnan(h)){
    Serial.println("Failed to read from DHT sensor!");
    //return 99;
  }
  return h;
}
