/*  PS2Keyboard library example
  
  PS2Keyboard now requries both pins specified for begin()

  keyboard.begin(data_pin, irq_pin);
  
  Valid irq pins:
     Arduino:      2, 3
     Arduino Mega: 2, 3, 18, 19, 20, 21
     Teensy 1.0:   0, 1, 2, 3, 4, 6, 7, 16
     Teensy 2.0:   5, 6, 7, 8
     Teensy++ 1.0: 0, 1, 2, 3, 18, 19, 36, 37
     Teensy++ 2.0: 0, 1, 2, 3, 18, 19, 36, 37
     Sanguino:     2, 10, 11
  
  for more information you can read the original wiki in arduino.cc
  at http://www.arduino.cc/playground/Main/PS2Keyboard
  or http://www.pjrc.com/teensy/td_libs_PS2Keyboard.html
  
  Like the Original library and example this is under LGPL license.
  
  Modified by Cuninganreset@gmail.com on 2010-03-22
  Modified by Paul Stoffregen <paul@pjrc.com> June 2010
*/
   
#include <PS2Keyboard.h>
#include "pitches.h"

const int DataPin = 8;
const int IRQpin =  3;

PS2Keyboard keyboard;

// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3,NOTE_G3, NOTE_A3, NOTE_G3,0, NOTE_B3, NOTE_C4};

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4,4,4,4,4 };
  
void playTone(int note, int duration)
{
    int noteDuration = 1000/duration;
    tone(5, note,noteDuration);

    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);

    noTone(5);
}

void setup() {
  delay(1000);
  keyboard.begin(DataPin, IRQpin);
  
  for (int thisNote = 0; thisNote < 8; thisNote++) {

    // to calculate the note duration, take one second
    // divided by the note type.
    //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
    int noteDuration = 1000/noteDurations[thisNote];
    tone(5, melody[thisNote],noteDuration);

    // to distinguish the notes, set a minimum time between them.
    // the note's duration + 30% seems to work well:
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    // stop the tone playing:
    noTone(5);
  }
  
  Serial.begin(9600);
  Serial.println("Keyboard Test:");
}

void loop() {
  if (keyboard.available()) {
    
    // read the next key
    char c = keyboard.read();
    
    // check for some of the special keys
    if (c == 'a') {
      playTone(NOTE_C3, 4);
    } else if (c == 's') {
      playTone(NOTE_D3, 4);
    } else if (c == 'd') {
      playTone(NOTE_E3, 4);
    } else if (c == 'f') {
      playTone(NOTE_F3, 4);
    } else if (c == 'g') {
      playTone(NOTE_G3, 4);
    } else if (c == 'h') {
      playTone(NOTE_A3, 4);
    } else if (c == 'j') {
      playTone(NOTE_B3, 4);
    } else if (c == 'k') {
      playTone(NOTE_C4, 4);
    } else if (c == 'l') {
      playTone(NOTE_D4, 4);
      
    } else if (c == 'w') {
      playTone(NOTE_CS3, 4);
    } else if (c == 'e') {
      playTone(NOTE_DS3, 4);
    } else if (c == 't') {
      playTone(NOTE_FS3, 4);
    } else if (c == 'y') {
      playTone(NOTE_GS3, 4);
    } else if (c == 'u') {
      playTone(NOTE_AS3, 4);
      
    } else {
      
    }
  }
}
