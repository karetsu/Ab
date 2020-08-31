/* A♭ */
#include <Arduino.h>
#include "MIDI.h"

MIDI_CREATE_DEFAULT_INSTANCE();

#define NUM_BUTTONS 24
#define MODES 4


// -- variables -----------------------------------------------------------------
// this array is a map between button location and IO pin
// rather than an array of arrays, line breaks correspond to physical location

// pins 22-45 are the first 24 pins on the right half of the board and keep all
// of the button on inputs collocated
const uint8_t buttonsP[NUM_BUTTONS] = { 22, 23, 24, 25,
                                        26, 27, 28, 29,
                                        30, 31, 32, 33,
                                        34, 35, 36, 37,
                                        38, 39, 40, 41,
                                        42, 43, 44, 45 };
// the transposed version of the buttons is for landscape mode
const uint8_t buttonsL[NUM_BUTTONS] = { 22, 26, 30, 34, 38, 42,
                                        23, 27, 31, 35, 39, 43,
                                        24, 28, 32, 36, 40, 44,
                                        25, 29, 33, 37, 41, 45 };

const uint8_t modes[MODES] = { 50, 51, 52, 53 };


// TODO: what/where to put LED's, can they simply be wired into place on buttons
uint8_t pressedButtons = 0x00;
uint8_t previousButtons = 0x00;

uint8_t activeMode = 0;
uint8_t previousMode = 0;

// TODO: make this shift the values by NUM_BUTTONS with the mode change;
uint8_t notePitches[NUM_BUTTONS];
uint8_t buttons[NUM_BUTTONS];


// TODO: include an extra switch which is there to control orientation
//         - its worth noting here that the visual representations of the notes
//           in the included files is no longer representative and the array
//           it transposed
bool orientation=true; // true = portrait, false = landscape
bool prevOrientn=true;


// -- prototypes ----------------------------------------------------------------
void readButtons();
void readMode();
void playNotes();
void setNotes();
void readOrientation();


// -- functions -----------------------------------------------------------------
// application specific actions
void readButtons() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (digitalRead(buttons[i]) == LOW) {
      bitWrite(pressedButtons, i, 1);
    } else bitWrite(pressedButtons, i, 0); } }


void playNotes() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    if (bitRead(pressedButtons, i) != bitRead(previousButtons, i)) {
      if (bitRead(pressedButtons, i)) {
        bitWrite(previousButtons, i , 1);
        MIDI.sendNoteOn(notePitches[i], 127, 1);
      } else {
        bitWrite(previousButtons, i , 0);
        MIDI.sendNoteOff(notePitches[i], 127, 1); } } } }


void readMode() {
  for (int i = 0; i < MODES; i++) {
    if (digitalRead(modes[i]) == LOW) {
      bitWrite(activeMode, i, 1); };
      break; } }


void setNotes() {
  if (activeMode != previousMode) {
    for (int i = 0; i < NUM_BUTTONS; i++)
      notePitches[i] = 24*(activeMode+1) + i; } }


void readOrientation() {
  if (orientation != prevOrientn) {
    if (orientation == true) {
      for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = buttonsP[i];
      }
    } else {
      for (int i = 0; i < NUM_BUTTONS; i++) {
        buttons[i] = buttonsL[i]; } }; } }

// -- arduino -------------------------------------------------------------------
// standard arduino setup and active loop
void setup() {
  for (int i = 0; i < NUM_BUTTONS; i++) {
    buttons[i] = buttonsP[i];
    pinMode(buttons[i], INPUT_PULLUP); // reduces the need for resistors
  }

  for (int i = 0; i < MODES; i++) {
    pinMode(modes[i], INPUT_PULLUP);
  }

  setNotes();
  
  // create starting pitches
  MIDI.begin(MIDI_CHANNEL_OMNI);
}


void loop() {
  readOrientation();
  readMode();
  readButtons();
  playNotes();
}
