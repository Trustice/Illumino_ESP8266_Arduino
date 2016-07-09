#if defined FW_ESP-201 || defined FW_NANO

#include <Arduino.h>

bool processPattern(const unsigned int value)
{
  switch (value)
  {
    case 99: // turn OFF
      if (!lockColors) { // save last colors
        Color1Save = Stripe.Color1;
        Color2Save = Stripe.Color2;
        lockColors = true;
        turnOff(); // triggers processPIRaction()
      }
      break;
    case 98: // turn ON
      if (!turnOnTrigger) {
        turnOnTrigger = true;
        turnOn();
      }
      break;
    case 1: // static
      Stripe.Static(Stripe.Color1);
      break;
    case 2: // waves
      Stripe.Waves(Stripe.Color1, 120, 50);
      break;
    case 3: // rainbow
      Stripe.RainbowCycle(100);
      break;
    case 4: // rainbow long
      Stripe.RainbowCycleLong(100);
      break;
    case 5: // color wipe
      Stripe.ColorWipe(Stripe.Color1, 1, FORWARD);
      break;
    case 6: // theater_chase
      Stripe.TheaterChase(Stripe.Color1, Stripe.Color2, 100);
      Stripe.ActivePattern = THEATER_CHASE;
      Stripe.TotalSteps = Stripe.numPixels();
      break;
    case 7: // fade
      if (Stripe.ActivePattern != STATIC)
      {
        FadeAfterWipe = true;
        Stripe.ColorWipe(Stripe.Color1, 1, FORWARD);
      }
      else
      {
        Stripe.Fade(Stripe.Color1, Stripe.Color2, 150, 10, FORWARD);
        //Stripe.ActivePattern = FADE;
      }
      break;
    case 8: // scanner
      Stripe.Scanner(Stripe.Color1, 1);
      break;
    default:
      return false;
  }
  return true;
}

void processInterval(const unsigned int value)
{
  Stripe.Interval = value;
}

void processDirection()
{
  if (Stripe.Direction == FORWARD)
    Stripe.Direction = REVERSE;
  else
    Stripe.Direction = FORWARD;
}

// set Color
bool processColor(unsigned long value)
{
  int number = value / 1000000000;

  // check if value has correct format (lenght and color number)
  if ( (number != 1) && (number != 2)) {
    return false;
  }

  value -= number * 1000000000;
  uint8_t red = value / 1000000;
  value -= red * 1000000;
  uint8_t green = value / 1000;
  value -= green * 1000;
  uint8_t blue = value;

  if (number == 1)
    Stripe.Color1 = (Stripe.Color(red, green, blue));
  else if (number == 2)
    Stripe.Color2 = (Stripe.Color(red, green, blue));
  
  return true;
}

void processSteps(const unsigned int value)
{
  Stripe.TotalSteps = value;
}

#endif
