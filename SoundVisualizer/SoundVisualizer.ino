#include <FTRGBLED.h>
#include <Vect3d.h>

/*
  Arduino Sound Visualizer
  Author: Tristan 'Kennyist' Cunningham - www.tristanjc.com
  Date: 13/02/2014
  License: Creative Commons ShareAlike 3.0 - https://creativecommons.org/licenses/by-sa/3.0/
*/

// =====================================================
// DO NOT EDIT ABOVE

int clockPin = 13;
int dataPin = 11;

// -------- LEDs -------- //

int nLEDs = 25; // Number of LED's 

/*     NOT IN USE AT THE MOMENT
int nLEDbottom = 0; // Number of LEDs on the bottom on the monitor
int nLEDleft = 8; // Number of LEDs on the left side
int nLEDtop = 9; // Number of LEDs on the top
int nLEDright = 8; // NUmber of LEDs on the right side
*/

boolean leftToRight = true; // Does the LED stip start from Left side of the screen?
int mode = 1; // Do you want to  fade from colour to color (0), from black into colour (1)
float sensitivity = 1; // Multiply the incoming volume level so lower sound levels create brighter light
int fadeAm = 7; // Fade speed

// ---- Colour settings ---- //

// Black -> colour, setting
// > Fade colour, The colour you want to fade in with sound. This is set in RGB so Green would be {0,255,0} Each value must be between 0 and 255.
// > > left
int fadeColLeft[3] = { 255, 0, 0 }; //{ 255, 165, 0 };
// > > Right
int fadeColRight[3] = { 0, 0 , 255 }; //{ 5, 125, 200 };

// Colour fade settings
// > Colour one (left)
// > > start
Tinker::Vect3d<float> startColOne(0,255,0);
// > > end
Tinker::Vect3d<float> endColOne(255,0,0);

// > Colour Two (right)
// > > start
Tinker::Vect3d<float> startColTwo(0,0,255);
// > > end
Tinker::Vect3d<float> endColTwo(0,255,0);

// ---- RandomColours ----  (BUGGY, NOT COMPLETE) //
/*
boolean randomColours = true;
boolean leftRightMatch = false;

int swapLevel = 70;

int rndCol[15] = { 255,0,0,0,255,0,0,0,255,255,165,0,5,125,200 };*/

// DO NOT EDIT BELOW, UNLESS YOU KNOW WHAT YOU ARE DOING
// =====================================================

int rgb[3] = {0,0,0}, rgb2[3] = {0,0,0},t1,t2,count = 0;
long int swapDelay;
boolean swapEnbl = true;
RGBLEDChain led(nLEDs, clockPin, dataPin);

void setup() {
  led.begin();
  Serial.begin(19200);
}

void loop() {
  if(Serial.available() > 0){
    char c = Serial.read();
    if(c == '.')
    {      
      switch(mode){
        case 0:
          ColourFade();
         break;
        case 1:
          fade();
         break;
        case 2:
          //soundMeter();
          ColourFade();
         break; 
      }
      
      led.update(); 
      count = 0; 
      t1 = 0; 
      t2 = 0;
    }
    if(c == ':'){ count++; }
    else
    {
      if(c != '.')
      {
        switch(count){
        case 0:
          t1 = t1 * 10 + (c - '0');
          break; 
        case 1:
          t2 = t2 * 10 + (c - '0');
          break;
        }
      }
    } 
  }
}

void fade(){
  
  int i = 0, imax = 0;
  if(leftToRight){ i = 0; imax = (nLEDs / 2); } else { i = (nLEDs / 2), imax = nLEDs; }
  
  for(i;i<imax;i++){
   rgb[0] = fadeInt(fadeColLeft[0],rgb[0],t1);
   rgb[1] = fadeInt(fadeColLeft[1],rgb[1],t1);
   rgb[2] = fadeInt(fadeColLeft[2],rgb[2],t1);
   
   FTLEDColour col = { rgb[0] , rgb[1] , rgb[2] };
   led.setLED(i, col);
 }
 
 if(leftToRight){ i = (nLEDs / 2), imax = nLEDs; } else { i = 0; imax = (nLEDs / 2); }
 
 for(i;i<imax;i++){
   rgb2[0] = fadeInt(fadeColRight[0],rgb2[0],t2);
   rgb2[1] = fadeInt(fadeColRight[1],rgb2[1],t2);
   rgb2[2] = fadeInt(fadeColRight[2],rgb2[2],t2);
   
   FTLEDColour col = { rgb2[0] , rgb2[1] , rgb2[2] };
   led.setLED(i, col);
 }
}

int fadeInt(int setlevel, int col, int lr){
  int tempa = (setlevel / 100) * (lr * sensitivity);
  float temp = (col * fadeAm + tempa + fadeAm) / (fadeAm +1 );
  if(temp < 0) { return 0; } else { if(temp > 255) { return 255; } else { return temp; } }
}

void ColourFade(){
  int i = 0, imax = 0;
  
  float fade = (t1 * sensitivity) / 100;
  
  if(leftToRight){ i = 0; imax = (nLEDs / 2); } else { i = (nLEDs / 2), imax = nLEDs; }
  
  for(i; i < imax; i++){
    Tinker::Vect3d<float> finalColour(startColOne*(1-fade) + endColOne*(fade));  
    
    FTLEDColour col = { finalColour[0] , finalColour[1] , finalColour[2] };
    led.setLED(i, col);
  }
  
  fade = (t2 * sensitivity) / 100;
  
  if(leftToRight){ i = (nLEDs / 2), imax = nLEDs; } else { i = 0; imax = (nLEDs / 2); }
  
  for(i; i < imax; i++){
    Tinker::Vect3d<float> finalColour(startColTwo*(1-fade) + endColTwo*(fade));  
    
    FTLEDColour col = { finalColour[0] , finalColour[1] , finalColour[2] };
    led.setLED(i, col);
  }
}



/* BUGGY
void soundMeter(){
  
  float gapTot = 100 / nLEDleft;
  int gap = t1 / gapTot;
  
  for(int i=0; i < gap; i++){
    Tinker::Vect3d<float> finalColour(startColOne*(1-(gap / 10)) + endColOne*(gap / 10)); 
    FTLEDColour col = { finalColour[0] , finalColour[1] , finalColour[2] };
    led.setLED((nLEDbottom / 2) + i, col);
    led.setLED((nLEDs - (nLEDbottom / 2)) - i, col);
  }
  
  for(int i = gap; i < nLEDleft; i++){
    FTLEDColour col = { 0 , 0 , 0 };
    led.setLED((nLEDbottom / 2) + i, col);
    led.setLED((nLEDs - (nLEDbottom / 2)) - i, col);
  }
}
*/



/* DOES NOT WORK WELL
void swapColoursLR(boolean leftRight){
  int rnd = 3 * random(0,((sizeof(rndCol) / 3) -1));
  
  if(leftRight){
    fadeColLeft[0] = rndCol[rnd];
    fadeColLeft[1] = rndCol[rnd + 1];
    fadeColLeft[2] = rndCol[rnd + 2];
  } else {
    fadeColRight[0] = rndCol[rnd];
    fadeColRight[1] = rndCol[rnd + 1];
    fadeColRight[2] = rndCol[rnd + 2];
  }
}

void swapColours(){
  int rnd = 3 * random(0,((sizeof(rndCol) / 3) -1));
  
  fadeColLeft[0] = rndCol[rnd];
  fadeColLeft[1] = rndCol[rnd + 1];
  fadeColLeft[2] = rndCol[rnd + 2];
  fadeColRight[0] = rndCol[rnd];
  fadeColRight[1] = rndCol[rnd + 1];
  fadeColRight[2] = rndCol[rnd + 2];
}*/
