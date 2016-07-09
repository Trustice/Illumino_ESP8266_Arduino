#if defined FW_ESP-201 || defined FW_NANO
/*
 * Illumino State Machine
 * ----------------------
 */
#include <Arduino.h>

// the possible states of the state-machine
typedef enum { GOT_P, GOT_I, GOT_D, GOT_C, GOT_S, GOT_T, GOT_H, EMPTY } states;
states state = EMPTY; // current state-machine state

void setState(const byte inByte) {
  switch (inByte) {
    case 'P':
      state = GOT_P;
      break;
    case 'I':
      state = GOT_I;
      break;
    case 'D':
      state = GOT_D;
      break;
    case 'C':
      state = GOT_C;
      break;
    case 'S':
      state = GOT_S;
      break;
    case 'T':
      state = GOT_T;
      break;
    case 'H':
      state = GOT_H;
      break;
    default:
      state = EMPTY;
      break;
  }
}

char * handlePreviousState(unsigned long value) {
  static char r[16]; // return string as char array

  switch (state) {
    case GOT_P:
      strcpy(r, "P");
      if (processPattern(value) == false) {
        strcat(r, "_ERR:");
      }
      break;
    case GOT_I:
      processInterval(value);
      strcpy(r, "I");
      break;
    case GOT_D:
      processDirection();
      strcpy(r, "D");
      break;
    case GOT_C:
      strcpy(r, "C");
      if (processColor(value) == false) {
        strcat(r, "_ERR:");
      }
      break;
    case GOT_S:
      processSteps(value);
      strcpy(r, "S");
      break;
    default:
      strcpy(r, "ERR_StateMachine:");
      break;
  }  // end of switch

  char value_str[12];
  sprintf(value_str, "%lu!", value);
  strcat(r, value_str);

  return r;
}

char * getInfo(unsigned long value) {
  static char r[16]; // return string

  switch (state) {
    case GOT_P: // return current Pattern number
      int p_num;
      if (leds_off)
        p_num = 97;
      else
        p_num = Stripe.ActivePattern;
      sprintf(r, "P%d", p_num);
      break;
    case GOT_I: // send current Interval in ms
      sprintf(r, "I%lu", Stripe.Interval);
      break;
    case GOT_C: // send current color, value required to define color1 or color2!
      uint32_t color_return;
      if (value == 1)
        color_return = Stripe.Color1;
      else if (value == 2)
        color_return = Stripe.Color2;
      else {
        sprintf(r, "C_ReqERR:%lu", value);
        break;
      }
      sprintf(r, "C%lu%03d%03d%03d", value, Stripe.Red(color_return), Stripe.Green(color_return), Stripe.Blue(color_return));
      break;
    case GOT_S:
      sprintf(r, "S%d", Stripe.TotalSteps);
      break;
    //    case GOT_T:
    //      Serial.print(F("T"));
    //      Serial.print(dht_read_temp());
    //      break;
    //    case GOT_H:
    //      Serial.print(F("H"));
    //      Serial.print(dht_read_humidity());
    //      break;
    default:
      strcpy(r, "N/A");
      break;
  }
  strcat(r, "!");
  return r;
}
#endif
