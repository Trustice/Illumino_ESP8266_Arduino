#if defined FW_ESP-01 || defined FW_ESP-201
/*
    Illumino WebServer
    ------------------

    Web server sendet content eines GET requests zum Arduino via Serial.
    Die Antwort vom Arduino wird als content der response gesendet.
*/
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <ESP8266HTTPClient.h>
#include <Ethernet.h>

/*
   Globals
*/

char http_response_msg[32];

WiFiServer server(80);

//IPAddress ip(192,168,178,61);

void connectWiFi() {
  // Connect to WiFi network
  Serial.print("\nConnecting to ");
  Serial.print(ssid);
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);
  WiFi.config(ip, gateway, subnet);//, myDNS, mySUBNET, myDNSServer);

  // Wait for connection and print dots on Serial while not connected
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("\nWiFi connected");

  // Print the IP address
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());

  // Start the server
  server.begin();
  Serial.println("Server started");
}

void listenWebServer() {
  // Check if WiFi is still connected
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("WiFi disconnected");
    connectWiFi();  // reconnect
    return;
  }
  
  // Check if a client has connected
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  //clear garbage on serial input
  while (Serial.available() > 0) {
    Serial.read();
  }

  // Wait until the client sends some data
  Serial.println("new Client");
  long now = millis();
  while (!client.available()) {
    if (millis() - now >= 5000){
      Serial.println("no client...?");
      return;
    }
    delay(1);
  }

  delay(100);

  // Read message from the first line of the request
  char req[32];
  int req_len = client.readBytesUntil('\r', req, 32); // e.g. req = GET /P99 HTTP/1.1 from GET-request: http://SERVER_IP/P99

  char req_msg[20]; // req_msg = P99
  int i;
  for (i = 0; i < req_len - 14; i++) {
    req_msg[i] = req[i + 5];
  }
  req_msg[i] = '\0';
  //String req_msg = req.substring(5, req.length()-9);
  client.flush();

  char response[60];
  // build HTTP response String
  strcpy(response, "HTTP/1.1 200 OK\r\nContent-Type: text/html\r\n\r\n");

  #if defined FW_ESP-01  // control Neopixels via Serial
    Serial.println(req_msg);
  
    // wait for response from Serial
    now = millis();
    while ((Serial.available() == 0) && (millis() - now <= 1000)) {
      delay(1);
    }
  
    if (Serial.available() > 0) {
      // Prepare the response
      char content[18];
      int content_len = Serial.readBytesUntil('\n', content, 18);
      content[content_len] = '\0';
      strcat(response, content);
      strcat(response, "\n");
    }
    else {
      strcat(response, "no response\n");
    }
  #else // control Neopixels local
    Serial.print("Request: ");
    Serial.println(req_msg);
    setState(req_msg[0]);
    char *response_msg;
    unsigned long value = 0;
    for (int j = 1; j < i+1; j++) {
      byte inByte = req_msg[j];
      //Serial.println(j);
      //Serial.println(inByte);
      if (isdigit(inByte)) {
        value *= 10;
        value += inByte - '0';
      }
      else if (inByte == '_') {
        response_msg = getInfo(value);
        break;
      }
      else {
        response_msg = handlePreviousState(value);
        currentValue = 0;
        break;
      }
    }
    strcat(response, response_msg);
    strcat(response, "\r\n");
  #endif

  // Send the response to the client
  Serial.println(F("Response:"));
  Serial.println(response);
  client.print(response);
  delay(1);
  Serial.println("- Client\n");

  // The client will actually be disconnected
  // when the function returns and 'client' object is destroyed
}

void sendDHTdata(const char uuid[38], float value) {
  if ((WiFi.status() == WL_CONNECTED) && !(isnan(value))) {
    Serial.println("[DHT] send data");
    
    HTTPClient http;

    char value_str [8];
    dtostrf(value, 3, 2, value_str);
    Serial.println(value_str);
    
    char url[128];
    sprintf(url, "http://192.168.178.27/middleware.php/data/%s.json?operation=add&value=%s", uuid, value_str);
    Serial.print("[DHT] ");
    Serial.println(url);

    http.begin(url);

    // start connection and send HTTP header
    int httpCode = http.GET();

    // httpCode will be negative on error
    if(httpCode > 0) {
        // HTTP header has been send and Server response header has been handled
        Serial.printf("[DHT] done... code: %d\n", httpCode);

        // file found at server
//        if(httpCode == HTTP_CODE_OK) {
//            String payload = http.getString();
//            Serial.println(payload);
//        }
    } else {
        Serial.printf("[DHT] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
    
  }
  else {
    Serial.println(value);
  }
}
#endif
