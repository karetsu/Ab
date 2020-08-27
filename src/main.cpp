/* some kind of project description  */

// -- head ----------------------------------------------------------------------
#include "MIDIUSB.h"

#define NUM_BUTTONS 24

// -- variables -----------------------------------------------------------------
// this array is a map between button location and IO pin
// rather than an array of arrays, line breaks correspond to physical location

// pins 20-43 are the first 24 pins on the right half of the board and keep all
// of the button on inputs collocated
const uint8_t buttons[NUM_BUTTONS] = { 20, 21, 22, 23
                                     , 24, 25, 26, 27
                                     , 28, 29, 30, 31
                                     , 32, 33, 34, 35
                                     , 36, 37, 38, 39
                                     , 40, 41, 42, 43
                                     };

// we don't need all 54 inputs so group the mode inputs away from the buttons
// TODO: abstract out a hard coded number of modes?
const uint8_t mode[4] { 0, 1, 2, 3 };
// the mode corresponds to the numeric value of the first note within the mode

// TODO: what/where to put LED's, can they simply be wired into place on buttons

uint8_t notesTime[NUM_BUTTONS];
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;
uint8_t activeMode = 0x00;
uint8_t previousMode = 0x00;

// TODO: make this shift the values by NUM_BUTTONS with the mode change;
uint8_t notePitches[NUM_BUTTONS];


// -- functions -----------------------------------------------------------------

// standard arduino setup and active loop
void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    pinMode(buttons[i], INPUT_PULLUP); // reduces the need for resistors
  }
}

void loop() {
  readButtons();
  playNotes();
}


// application specific actions

void controlChange(byte channel, byte control, byte value) {
  // First parameter is the event type (0x0B = control change).
  //  Second parameter is the event type, combined with the channel.
  //  Third parameter is the control number number (0-119).
  //  Fourth parameter is the control value (0-127).
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}


void readButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      bitWrite(pressedButtons, i, 1);
      delay(50); // TODO: check this value
    } else bitWrite(pressedButtons, i, 0);
  }
    // TODO: add in the reading of mode presses which should update the notes
    // TODO: LED stuff, should change colour on button press and be 'flavoured'
}

void playNotes()
{
  for (int i = 0; i < NUM_BUTTONS; i++)
  {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i))
    {
      if (bitRead(pressedButtons, i))
      {
        bitWrite(previousButtons, i , 1);
        noteOn(0, notePitches[i], intensity);
        MidiUSB.flush();
      }
      else
      {
        bitWrite(previousButtons, i , 0);
        noteOff(0, notePitches[i], 0);
        MidiUSB.flush();
      }
    }
  }
}


// -- both following functions --------------------------------------------------
// First parameter is the event type (0x09 = note on, 0x08 = note off).
// Second parameter is note-on/note-off, combined with the channel.
// Channel can be anything between 0-15. Typically reported to the user as 1-16.
// Third parameter is the note number (48 = middle C).
// Fourth parameter is the velocity (64 = normal, 127 = fastest).

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}
