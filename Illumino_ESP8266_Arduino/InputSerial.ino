#if defined FW_ESP-201 || defined FW_NANO

#include <Arduino.h>

/************************
 * Serial communication *
 ************************/

// current partial number
unsigned long currentValue;


void processIncomingByte(const byte inByte) {
  if (isdigit(inByte)) {   // calculate incoming value
    currentValue *= 10;
    currentValue += inByte - '0';
  }
  else if (inByte == '_') {
  	char *info;
  	info = getInfo(currentValue);
  	Serial.println(info);
    currentValue = 0;
  }
  else {
    // end of the number signals a state change (in most cases this will be '\n')
    if (currentValue != 0) {
  	  char *resp;
      resp = handlePreviousState(currentValue);
  	  Serial.println(resp);  	
  	  currentValue = 0;
    }
    // set the new state, if we recognize it
	  setState(inByte);
  }
}

// check for serial input
// called automatically after every loop() --> at least on Arduino
void serialEvent() {
  //delay(10);
  char IncomingByte;
  while (Serial.available() > 0) 
  {
    IncomingByte = Serial.read();
    processIncomingByte(IncomingByte);
  }
}

#endif
