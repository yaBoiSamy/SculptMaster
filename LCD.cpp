#include "LCD.h"

#include "Arduino.h"
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

LCD::LCD() {};

LCD::LCD(String _labels[], double* _values[], int _labelcount): display(128, 64, &Wire, -1){
  labelcount = _labelcount;
  carrouselIndex = _labelcount;
  
  for (int i = 0; i < _labelcount; i++) {
    labels[i] = _labels[i];
    valueRefs[i] = _values[i];
  }
}

void LCD::begin(){
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }
}

void LCD::update(){
  display.clearDisplay();
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);

  if (carrouselIndex==labelcount){ // if we are on the main view
    display.setTextSize(1);
    if (labelcount > 6){ // if there is too much data to display
      display.println("Line overflow (max 6)");
    }
    else{ // if we are on the main view with no problems detected
      for (int i = 0; i < labelcount; i++) { // iterate through the lists and display their content
        display.setCursor(0, i * 10);
        overflowPrint(labels[i] + ": " + String(*valueRefs[i]), 21);
      }
    }
  }
  else{ // if we are on zoomed in view
    display.setTextSize(2);
    overflowPrint(labels[carrouselIndex], 10);
    overflowPrint(String(*valueRefs[carrouselIndex]), 10);
  }

  display.display();
}

void LCD::next(){ // go to next view
  carrouselIndex = (carrouselIndex + 1)%(labelcount + 1);
}

void LCD::previous(){ // go to previous view
  carrouselIndex = (carrouselIndex - 1)%(labelcount + 1);
}

void LCD::view(int i){ // go to any view
  carrouselIndex = i % (labelcount + 1);
}

void LCD::view(){
  view(labelcount);
}

void LCD::view(String labelname){
  for (int i=0; i <= labelcount; i++){
    if (labels[i]==labelname){
      view(i);
    }else if (i==labelcount){
      return;
    }
  }
}

void LCD::overflowPrint(String input, int maxlen){ // prints a line but checks for character overflows
  display.println(input.length() <= maxlen ? input : "Overflow");
}
