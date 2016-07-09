#if defined FW_ESP-201 || defined FW_NANO

#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include "NeoPatterns.h"


bool leds_off = false;  // state of pir usage
void StripeComplete();

// Define NeoPattern: led stripe
NeoPatterns Stripe(NUMPIXELS, PIN_NEOPIXEL, NEO_GRB + NEO_KHZ800, &StripeComplete);

//------------------------------------------------------------
// Setup
//------------------------------------------------------------
void neopixelSetup() {
  //state = EMPTY;
  Stripe.setBrightness(150);  // limit the led brightness to limit current
  Stripe.Color1 = Stripe.Color(250, 170, 30);
  Stripe.begin();
  processPattern(99);
}

//------------------------------------------------------------
// Main loop
//------------------------------------------------------------
void neopixelUpdate() {
  Stripe.Update();
}

//------------------------------------------------------------
//Completion Routines - get called on completion of a pattern
//------------------------------------------------------------

// Stripe Completion Callback

uint32_t Color1Save, Color2Save;
bool FadeAfterWipe = false;
bool turnOnTrigger = false;
bool lockColors = false;

void StripeComplete()
{
  if (Stripe.ActivePattern == FADE) // color faded from 1 to 2
  {
    switchColors();    
    // switch to static pattern
    Stripe.Static(Stripe.Color1);
    if (turnOnTrigger) {
      Stripe.Color2 = Color2Save;
      turnOnTrigger = false;
      lockColors = false;
    }
  }
  
  if (Stripe.ActivePattern == COLOR_WIPE)
  {
    if (FadeAfterWipe)
    {
      Stripe.Fade(Stripe.Color1, Stripe.Color2, 150, 10, FORWARD);
      FadeAfterWipe = false;
    }
    else
      Stripe.Static(Stripe.Color1);
  }
}

void switchColors() // change color 1 and color 2
{
  uint32_t tempColor = Stripe.Color1;
  Stripe.Color1 = Stripe.Color2;
  Stripe.Color2 = tempColor;
}

//------------------------------------------------------------
//handle PIR sensor
//------------------------------------------------------------


void turnOff()
{
  #ifdef PIN_PIR
    attachInterrupt(digitalPinToInterrupt(PIN_PIR), processPirAction, CHANGE);
    
    Serial.println(F("PIR enabled"));
  #else
    Stripe.Fade(Stripe.Color1, Stripe.Color(0, 0, 0), 100, 1, FORWARD);
  #endif
  leds_off = true;
}

void turnOn()
{
  #ifdef PIN_PIR
    detachInterrupt(digitalPinToInterrupt(PIN_PIR));
    Serial.println(F("PIR disabled"));
  #endif
  leds_off = false;
  Stripe.Fade(Stripe.Color1, Color1Save, 100, 1, FORWARD);
}

void processPirAction() // triggered by interrupt
{
  #ifdef PIN_PIR
    if (digitalRead(PIN_PIR) == 1)
    {
      Stripe.Fade(Stripe.Color1, Stripe.Color(250, 170, 30), 100, 1, FORWARD);
      Serial.println(F("motion detetcted - ON "));
  //    Serial.println(millis());
    }
    else
    {
      Stripe.Fade(Stripe.Color1, Stripe.Color(0, 0, 0), 100, 1, FORWARD);
      Serial.println(F("no motion - OFF "));
  //    Serial.println(millis());
    }
  #endif
}

#endif
