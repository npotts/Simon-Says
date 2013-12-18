// -*-c++-*-
/*
This is a much hacked up version of the Simon-Says to make it into a
more toddler friendly "Simon-Sings".  Pressing the buttons now plays a tone
and lites up the LEDs, and nothing else.  May amuse small children for
some time.  (You) Should encourage them to dance to 80's style 8 bit
beeping (aka music).
*/

#include "hardware_versions.h"
#include "pitches.h"

//How long should a keypress make noise and LED stay lit?
#define TONE_LENGTH 10
//how often should we blink when in attractMode
#define BLINK_EVERY 250
//wait these number of ms until calling attractMode.  60*1000 = 1 min
#define TIME_TILL_BLINKY 60*1000

/*
Make this as long as you wish.  This melody is blatently taken from the
Simon Says "cheat mode" from sfe. The length is retrieved by the sizeof()
operator, so you dont need to remember how long this is until you have
more characters than can fit in a sizeof(unsigned int) (which is 65536)
*/
int melody[] = {NOTE_G4, NOTE_A4,       0, NOTE_C5,
                      0,       0, NOTE_G4,       0,
                      0,       0, NOTE_E4,       0,
                NOTE_D4, NOTE_E4, NOTE_G4,       0,
                NOTE_D4, NOTE_E4,       0, NOTE_G4,
                0,             0, NOTE_D4,       0,
                NOTE_E4,       0, NOTE_G4,       0,
                NOTE_A4,       0, NOTE_C5,       0};
/*how deep are we in the melody array*/
unsigned int melody_pos = 0;

void setup() {
  /*Setup hardware inputs/outputs. These pins are defined in the
    hardware_versions header file*/
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
  // setup the "BUZZER1" side of the buzzer to stay low, while we play the tone on the other pin.
  digitalWrite(BUZZER1, LOW); 

  attractMode(); //make the think blink like crazy
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

byte attractMode(void) {
  /* Show an "attract mode" display while waiting for user to press button.*/
  unsigned long time;
  bool ok = false;
  byte button;
  Serial.write("Waiting for small children to play with me\r\n");
  while(!ok) {
    //loop for BLINK_EVERY ms lookin for a keypress with LEDs ON
    setLEDs(CHOICE_RED | CHOICE_GREEN | CHOICE_BLUE | CHOICE_YELLOW); // Turn all LEDs on
    //send a tone of the melody
    tone(BUZZER2, melody[melody_pos++], BLINK_EVERY+10); melody_pos %= sizeof(melody); //force melody_pos to [0: sizeof(melody)), where sizeof(melody) is always positive
    time = millis();
    //this while polls for a checkButton, set the value to button, and repeats for BLINK_EVERY ms
    while( ( (button = checkButton()) == CHOICE_NONE) && (millis() - time < BLINK_EVERY)) ;
    if (button != CHOICE_NONE) return button; //signal to bail out of 
    setLEDs(CHOICE_NONE);
    //send another tone of the melody
    tone(BUZZER2, melody[melody_pos++], BLINK_EVERY+10); melody_pos %= sizeof(melody);
    time = millis();
    while( ( (button = checkButton()) == CHOICE_NONE) && (millis() - time < BLINK_EVERY)) ;
    if (button != CHOICE_NONE) return button;
  }
}
byte getAKeyPress() {
  /*Loop until we get a keypress*/
  byte key;
  unsigned long time=millis(), offtime=millis();
  while((key = checkButton()) == CHOICE_NONE) {
    if (millis() - time > TONE_LENGTH)  setLEDs(CHOICE_NONE);
    if (millis() - offtime > TIME_TILL_BLINKY)      return attractMode();
  }
}
void loop() {
  /*Go baby go!!*/
  byte key = getAKeyPress();
  setLEDs(key);
  /*
    Light an LED and play tone
    Red, upper left:     NOTE_A4 = 440Hz    - 2.272ms - 1.136ms pulse
    Green, upper right:  NOTE_A5 = 880Hz    - 1.136ms - 0.568ms pulse
    Blue, lower left:    NOTE_D5 = 587.33Hz - 1.702ms - 0.851ms pulse
    Yellow, lower right: NOTE_G5 = 784Hz    - 1.276ms - 0.638ms pulse
  */
  switch(key) {
  case CHOICE_RED:    tone(BUZZER2, NOTE_A4, TONE_LENGTH); break;
  case CHOICE_GREEN:  tone(BUZZER2, NOTE_A5, TONE_LENGTH); break;
  case CHOICE_BLUE:   tone(BUZZER2, NOTE_D5, TONE_LENGTH); break;
  case CHOICE_YELLOW: tone(BUZZER2, NOTE_G5, TONE_LENGTH); break;
  }
}

