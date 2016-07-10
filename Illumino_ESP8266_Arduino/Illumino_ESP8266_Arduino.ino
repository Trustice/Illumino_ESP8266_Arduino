
/*********
 * Setup *
 *********/
void setup() {
  Serial.begin(115200);
  delay(10);

  #if defined FW_ESP-01 || defined FW_ESP-201
  connectWiFi();
  dhtSetup();
  #endif

  #if defined FW_ESP-201 || defined FW_NANO
  neopixelSetup();
  #endif

  #ifdef PIN_DHT
  Serial.println("DHT");
  dhtSetup();
  #else
  Serial.println("PIN_DHT not defined");
  #endif
}

/********
 * Loop *
 ********/
void loop() {
  #if defined FW_ESP-201 || defined FW_NANO
  neopixelUpdate();
  #endif
  
  #if defined FW_ESP-01 || defined FW_ESP-201
  listenWebServer();
  dhtUpdate();
  #endif

  #if defined FW_ESP-201
  serialEvent();
  #endif

  #ifdef PIN_DHT
  Serial.println("DHT");
  dhtUpdate();
  #endif
}

