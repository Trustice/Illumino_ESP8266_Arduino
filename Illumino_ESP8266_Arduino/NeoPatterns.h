#include <Arduino.h>
#include <Adafruit_NeoPixel.h>

// Pattern types supported:
enum  pattern { NONE, STATIC, WAVES, RAINBOW_CYCLE, RAINBOW_CYCLE_LONG, COLOR_WIPE, THEATER_CHASE, FADE,  SCANNER };
// Patern directions supported:
enum  direction { FORWARD, REVERSE };

// precalculated values for sinus animations
const uint8_t SinusTab[] = { 0, 0, 1, 2, 3, 4, 6, 8, 11, 14, 17, 20, 24, 28, 33, 37, 42, 47, 52, 58, 64, 69, 75, 81, 88, 94, 101, 107, 114, 120, 127, 134, 140, 147, 153, 160, 166, 173, 179, 185, 191, 196, 202, 207, 212, 217, 221, 226, 230, 234, 237, 240, 243, 246, 248, 250, 251, 252, 253, 254 };

// NeoPattern Class - derived from the Adafruit_NeoPixel class
class NeoPatterns : public Adafruit_NeoPixel
{
  public:

    // Member Variables:
    pattern  ActivePattern;   // which pattern is running
    direction Direction;      // direction to run the pattern

    unsigned long Interval;   // milliseconds between updates
    unsigned long lastUpdate; // last update of position
    
    uint32_t Color1, Color2;  // What colors are in use
    uint16_t TotalSteps;      // total number of steps in the pattern
    uint16_t Index;           // current step within the pattern
    bool StartPattern;
    
    void (*OnComplete)();     // Callback on completion of pattern

    // Constructor - calls base-class constructor to initialize strip
    NeoPatterns(uint16_t pixels, uint8_t pin, uint8_t type, void (*callback)())
      : Adafruit_NeoPixel(pixels, pin, type)
    {
      OnComplete = callback;
    }

    // Update the pattern
    void Update() 
    {
      if ((millis() - lastUpdate) > Interval) // time to update
      {
        lastUpdate = millis();
        switch (ActivePattern) 
        {
          case STATIC:
            StaticUpdate();
            break;
          case RAINBOW_CYCLE:
            RainbowCycleUpdate();
            break;
          case THEATER_CHASE:
            TheaterChaseUpdate();
            break;
          case COLOR_WIPE:
            ColorWipeUpdate();
            break;
          case SCANNER:
            ScannerUpdate();
            break;
          case FADE:
            FadeUpdate();
            break;
          case RAINBOW_CYCLE_LONG:
            RainbowCycleLongUpdate();
            break;
          case WAVES:
            WavesUpdate();
            break;
          default:
            break;
        }
      }
    }

    // Increment the Index and reset at the end
    void Increment() 
    {
      if (Direction == FORWARD) 
      {
        Index++;
        if (Index >= TotalSteps) 
        {
          Index = 0;
          if (OnComplete != NULL) 
          {
            OnComplete();             // call the comlpetion callback
          }
        }
      }
      else // Direction == REVERSE
      {                          
        --Index;
        if (Index == 65535) // Index = 0 - 1 = 65535! Overflow
        { 
          Index = TotalSteps - 1;
          if (OnComplete != NULL) 
          {
            OnComplete(); // call the comlpetion callback
          }
        }
      }
    }

    // Reverse pattern direction
    void Reverse() 
    {
      if (Direction == FORWARD) 
      {
        Direction = REVERSE;
        Index = TotalSteps - 1;
      }
      else 
      {
        Direction = FORWARD;
        Index = 0;
      }
    }

    // Initialize Static Pattern
    void Static(uint32_t color1)
    {
      ActivePattern = STATIC;
      Color1 = color1;
      Interval = 100;
    }

    // Update the Static Pattern
    void StaticUpdate()
    {
      ColorSet(Color1);
      show();
    }

    // Initialize for a RainbowCycle
    void RainbowCycle(uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = RAINBOW_CYCLE;
      Interval = interval;
      TotalSteps = 256; // should not be changed
      Index = 0;
      Direction = dir;
      StartPattern = true;
    }

    // Update the Rainbow Cycle Pattern
    void RainbowCycleUpdate()
    {
      for (unsigned long i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Wheel(((i * 256 / numPixels()) + Index) & 255));
        if (StartPattern)
          show();
      }
      StartPattern = false;
      show();
      Increment();
    }

    // Initialize for a RainbowCycle Long
    void RainbowCycleLong(uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = RAINBOW_CYCLE_LONG;
      Interval = interval;
      TotalSteps = 768;  // due to smoothness just use multiples of 256 (512, 768, 1024, 1280, 1536...)
      Index = 0;
      Direction = dir;
      StartPattern = true;
    }

    // Update the Rainbow Cycle Long Pattern
    void RainbowCycleLongUpdate()
    {
      for (unsigned long i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, Wheel(((i * 256 / TotalSteps) + Index) & 255));
        if (StartPattern)
          show();
      }
      StartPattern = false;
      show();
      Increment();
    }

    // Initialize for a Theater Chase
    void TheaterChase(uint32_t color1, uint32_t color2, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = THEATER_CHASE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
      StartPattern = true;
    }

    // Update the Theater Chase Pattern
    void TheaterChaseUpdate()
    {
      for (unsigned int i = 0; i < numPixels(); i++)
      {
        if ((i + Index) % 5 == 0)
        {
          setPixelColor(i, Color1);
        }
        else
        {
          setPixelColor(i, Color2);
        }
        if (StartPattern)
          show();
      }
      StartPattern = false;
      show();
      Increment();
    }

    // Initialize for a ColorWipe
    void ColorWipe(uint32_t color, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = COLOR_WIPE;
      Interval = interval;
      TotalSteps = numPixels();
      Color1 = color;
      Index = 0;
      Direction = dir;
    }

    // Update the Color Wipe Pattern
    void ColorWipeUpdate()
    {
      if (Index < (numPixels()))
      {
        setPixelColor(Index, Color1);
        show();

//        if (((Direction == FORWARD) && (Index == (numPixels() - 1))) || ((Direction == REVERSE) && (Index == 0)))
//        {
//          Index = TotalSteps + 1;
//        }
//        else
//        {
//          Increment();
//        }
        Increment();
      }
    }

    // Initialize for a SCANNNER
    void Scanner(uint32_t color1, uint8_t interval)
    {
      ActivePattern = SCANNER;
      Interval = interval;
      TotalSteps = (numPixels() - 1) * 2;
      Color1 = color1;
      Index = 0;
      StartPattern = true;
    }

    // Update the Scanner Pattern
    void ScannerUpdate()
    {
      if (StartPattern)
      {
        for (int i = numPixels() - 1; i >= 0; i--)
        {
          setPixelColor(i, Color(0,0,0));
          show();
        }  
      }
      
      for (unsigned int i = 0; i < numPixels(); i++)
      {
        if ((i == Index) || (i == TotalSteps - Index))  // Scan Pixel to the right
          setPixelColor(i, Color1);
        else if ((i == Index - 1) || (i == TotalSteps - Index + 1))
          setPixelColor(i,DimColor(Color1));
        else if ((i == Index - 2) || (i == TotalSteps - Index + 2))
          setPixelColor(i,DimColor(DimColor(Color1)));
        else if ((i == Index - 3) || (i == TotalSteps - Index + 3))
          setPixelColor(i,DimColor(DimColor(DimColor(Color1))));
        else if ((i == Index - 4) || (i == TotalSteps - Index + 4))
          setPixelColor(i,DimColor(DimColor(DimColor(DimColor(Color1)))));
        else if ((i == Index - 5) || (i == TotalSteps - Index + 5))
          setPixelColor(i,DimColor(DimColor(DimColor(DimColor(DimColor(Color1))))));
        else if ((i == Index - 6) || (i == TotalSteps - Index + 6))
          setPixelColor(i,DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(Color1)))))));
        else if ((i == Index - 7) || (i == TotalSteps - Index + 7))
          setPixelColor(i,DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(Color1))))))));
        else if ((i == Index - 8) || (i == TotalSteps - Index + 8))
          setPixelColor(i,DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(DimColor(Color1)))))))));
        else
          setPixelColor(i, Color(0,0,0));
      }
      StartPattern = false;
      show();
      Increment();
    }

    // Initialize for a Fade
    void Fade(uint32_t color1, uint32_t color2, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = FADE;
      Interval = interval;
      TotalSteps = steps;
      Color1 = color1;
      Color2 = color2;
      Index = 0;
      Direction = dir;
    }

    // Update the Fade Pattern
    void FadeUpdate()
    {
      // Calculate linear interpolation between Color1 and Color2
      // Optimise order of operations to minimize truncation error
      uint8_t red = ((Red(Color1) * (TotalSteps - Index)) + (Red(Color2) * Index)) / TotalSteps;
      uint8_t green = ((Green(Color1) * (TotalSteps - Index)) + (Green(Color2) * Index)) / TotalSteps;
      uint8_t blue = ((Blue(Color1) * (TotalSteps - Index)) + (Blue(Color2) * Index)) / TotalSteps;

      ColorSet(Color(red, green, blue));
      show();
      Increment();
    }

    // Initialize for Waves
    void Waves(uint32_t color1, uint16_t steps, uint8_t interval, direction dir = FORWARD)
    {
      ActivePattern = WAVES;
      Interval = interval;
      TotalSteps = steps;
      Color1 = color1;
      Index = 0;
      Direction = dir;
      StartPattern = true;
    }

    // Update the Waves Pattern

    void WavesUpdate()
    {
      float dimFactor;

      for (unsigned int i = 0; i < numPixels(); i++) {
        int sinusIndex = (i + Index) % 60;
        if (((i + Index) / 60) % 2 == 1) {
          dimFactor = (SinusTab[59 - sinusIndex]) / 255.0;
        }
        else {
          dimFactor = SinusTab[sinusIndex] / 255.0;
        }
        setPixelColor(i, Color(Red(Color1) * dimFactor, Green(Color1) *  dimFactor, Blue(Color1) *  dimFactor));
        if (StartPattern)
          show();
      }
      StartPattern = false;
      show();
      Increment();
    }

    // Calculate 50% dimmed version of a color (used by ScannerUpdate)
    uint32_t DimColor(uint32_t color)
    {
      // Shift R, G and B components one bit to the right
      uint32_t dimColor = Color(Red(color) >> 1, Green(color) >> 1, Blue(color) >> 1);
      return dimColor;
    }

    // Set all pixels to a color (synchronously)
    void ColorSet(uint32_t color)
    {
      for (unsigned int i = 0; i < numPixels(); i++)
      {
        setPixelColor(i, color);
      }
      show();
    }

    // Returns the Red component of a 32-bit color
    uint8_t Red(uint32_t color)
    {
      return (color >> 16) & 0xFF;
    }

    // Returns the Green component of a 32-bit color
    uint8_t Green(uint32_t color)
    {
      return (color >> 8) & 0xFF;
    }

    // Returns the Blue component of a 32-bit color
    uint8_t Blue(uint32_t color)
    {
      return color & 0xFF;
    }

    // Input a value 0 to 255 to get a color value.
    // The colours are a transition r - g - b - back to r.
    uint32_t Wheel(byte WheelPos) 
    {
      WheelPos = 255 - WheelPos;
      if (WheelPos < 85) {
        return Color(255 - WheelPos * 3, 0, WheelPos * 3);
      }
      else if (WheelPos < 170) {
        WheelPos -= 85;
        return Color(0, WheelPos * 3, 255 - WheelPos * 3);
      }
      else {
        WheelPos -= 170;
        return Color(WheelPos * 3, 255 - WheelPos * 3, 0);
      }
    }
    
    // Turn Pattern On
//    void TurnPatternOn()
//    {
//      if (StartPattern)
//        show();
//    }
};


