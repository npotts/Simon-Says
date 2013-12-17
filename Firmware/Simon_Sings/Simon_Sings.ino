// -*-c++-*-
/*
This is a much hacked up version of the Simon-Says to make it into a
more toddler friendly "Simon-Sings".  Pressing the buttons now plays a tone
and lites up the LEDs, and nothing else.  Should amuse small children for
some time.  Maybe even encourage them to dance to their own music.
*/

#include "hardware_versions.h"
#include "pitches.h"

//How long should a keypress make noise?
#define TONE_LENGTH 10

void setup() {
  /*Setup hardware inputs/outputs. These pins are defined in the hardware_versions header file*/
  Serial.begin(9600);
  
  //Enable pull ups on inputs
  pinMode(BUTTON_RED,    INPUT_PULLUP);
  pinMode(BUTTON_GREEN,  INPUT_PULLUP);
  pinMode(BUTTON_BLUE,   INPUT_PULLUP);
  pinMode(BUTTON_YELLOW, INPUT_PULLUP);

  pinMode(LED_RED,       OUTPUT);
  pinMode(LED_GREEN,     OUTPUT);
  pinMode(LED_BLUE,      OUTPUT);
  pinMode(LED_YELLOW,    OUTPUT);

  pinMode(BUZZER1,       OUTPUT);
  pinMode(BUZZER2,       OUTPUT);
  digitalWrite(BUZZER1, LOW); // setup the "BUZZER1" side of the buzzer to stay low, while we play the tone on the other pin.
  
  //setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
  //delay(1000);
  //setLEDs(CHOICE_NONE); // Turn off LEDs
  Serial.write("\nStarting\r\n");
}

byte checkButton(void) {
  /*Returns a '1' bit in the position corresponding to CHOICE_RED, CHOICE_GREEN, etc.*/
  if      (digitalRead(BUTTON_RED)    == 0) return(CHOICE_RED    ); 
  else if (digitalRead(BUTTON_GREEN)  == 0) return(CHOICE_GREEN  ); 
  else if (digitalRead(BUTTON_BLUE)   == 0) return(CHOICE_BLUE   ); 
  else if (digitalRead(BUTTON_YELLOW) == 0) return(CHOICE_YELLOW );
  return(CHOICE_NONE); // If no button is pressed, return none
}

void setLEDs(byte leds) {
  /* Lights a given LEDs
     Pass in a byte that is made up from CHOICE_RED, CHOICE_YELLOW, etc
  */
  if ((leds & CHOICE_RED) != 0)    digitalWrite(LED_RED,     HIGH);
  else                             digitalWrite(LED_RED,     LOW );                                                           
  if ((leds & CHOICE_GREEN) != 0)  digitalWrite(LED_GREEN,   HIGH);
  else                             digitalWrite(LED_GREEN,   LOW );
  if ((leds & CHOICE_BLUE) != 0)   digitalWrite(LED_BLUE,    HIGH);
  else                             digitalWrite(LED_BLUE,    LOW);
  if ((leds & CHOICE_YELLOW) != 0) digitalWrite(LED_YELLOW,  HIGH);
  else                             digitalWrite(LED_YELLOW,  LOW);
}

void attractMode(void) {
  /* Show an "attract mode" display while waiting for user to press button.*/
  setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
  while(1) if(checkButton() != CHOICE_NONE) break;
  setLEDs(CHOICE_NONE);
}

void loop() {
  /*Go baby go!!*/
  byte key;
  unsigned long time=0;
  attractMode();
  while(1) { //loop forever
    while( (key = checkButton()) == CHOICE_NONE) {
      if ((time != 0) && (millis() - time > TONE_LENGTH) ) {
        time = 0;
        setLEDs(CHOICE_NONE);
      }
    }
    setLEDs(key);
    /*
      Light an LED and play tone
      Red, upper left:     440Hz - 2.272ms - 1.136ms pulse
      Green, upper right:  880Hz - 1.136ms - 0.568ms pulse
      Blue, lower left:    587.33Hz - 1.702ms - 0.851ms pulse
      Yellow, lower right: 784Hz - 1.276ms - 0.638ms pulse
    */
    time = millis();
    switch(key) {
    case CHOICE_RED:    tone(BUZZER2, NOTE_A4, TONE_LENGTH); break;
    case CHOICE_GREEN:  tone(BUZZER2, NOTE_A5, TONE_LENGTH); break;
    case CHOICE_BLUE:   tone(BUZZER2, NOTE_D5, TONE_LENGTH); break;
    case CHOICE_YELLOW: tone(BUZZER2, NOTE_G5, TONE_LENGTH); break;
    }
  }
}
