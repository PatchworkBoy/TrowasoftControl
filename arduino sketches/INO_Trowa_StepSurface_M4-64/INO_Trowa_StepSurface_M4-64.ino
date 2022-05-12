/*
  This example shows basic usage of the
  MultiTrellis object controlling an array of
  NeoTrellis boards, controlling Trowasoft multiSeq for VCVRack via
  MIDI over USB.

  As is, this example shows use of 4 NeoTrellis boards
  connected together.

  Any noteOn on ch16 will advance column highlight.
  Any noteOn on ch15 will advance row highlight.
  Any noteOn on ch14 will reset row / column highlight to pos 0.
  Ch13 specifically expects notes incoming from INO_Trowa_Transport_M4-32
  to switch Channel in multiSeq.
  Any noteOn on other channels will illuminate associated key.

  See accompanying .vcv example file.
*/
#include <memory>
#include <map>
#include <Arduino.h>
#include <Adafruit_NeoTrellis.h>
#include <MIDIUSB.h>

// Variable that holds the current position in the sequence.
uint32_t previous = 0;
uint32_t color = 0;
int row = 0;
int lastrow = 0;
int col = 0;
int lastcol = 0;
int currentColor = -1;

// Array of midi-note values to be mapped top left to bottom right - C0 thru D#5 = 24 thru 87
byte note_sequence[] = {
  24, 25, 26, 27, 28, 29, 30, 31, 32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64, 65, 66, 67, 68, 69, 70, 71, 72, 73, 74,
  75, 76, 77, 78, 79, 80, 81, 82, 83, 84, 85, 86, 87
};

// Array of midi-cc IDs to be mapped top left to bottom right (unused at the mo)
byte control_sequence[] = {
  24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63,
  64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83, 84, 85, 86, 87
};

// Array to store individual led states
int state[] = {
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0,
  0,0,0,0,0,0,0,0
};

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys
#define maincolor seesaw_NeoPixel::Color(0,0,0) //our 'inactive' background color
#define RED seesaw_NeoPixel::Color(155,10,10);
#define ORANGE seesaw_NeoPixel::Color(155,80,10);
#define YELLOW seesaw_NeoPixel::Color(155,155,30);
#define GREEN seesaw_NeoPixel::Color(0,155,0);
#define CYAN seesaw_NeoPixel::Color(40,155,155);
#define BLUE seesaw_NeoPixel::Color(20,50,155);
#define PURPLE seesaw_NeoPixel::Color(100,20,155);
#define PINK seesaw_NeoPixel::Color(155,40,60);

//create a matrix of trellis panels
Adafruit_NeoTrellis t_array[Y_DIM / 4][X_DIM / 4] = {

  { Adafruit_NeoTrellis(0x30), Adafruit_NeoTrellis(0x2E) },
  { Adafruit_NeoTrellis(0x2F), Adafruit_NeoTrellis(0x31) }

};

//pass this matrix to the multitrellis object
Adafruit_MultiTrellis trellis((Adafruit_NeoTrellis *)t_array, Y_DIM / 4, X_DIM / 4);

// Input a value 0 to 255 to get a color value.
// The colors are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  if (WheelPos < 85) {
    return seesaw_NeoPixel::Color(WheelPos * 3, 255 - WheelPos * 3, 0);
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    return seesaw_NeoPixel::Color(255 - WheelPos * 3, 0, WheelPos * 3);
  } else {
    WheelPos -= 170;
    return seesaw_NeoPixel::Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  return 0;
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

//define a callback for key presses
TrellisCallback onKey(keyEvent evt) {
  auto const now = millis();
  auto const keyId = evt.bit.NUM;
  bool isReleased;

  switch (evt.bit.EDGE)
  {
    case SEESAW_KEYPAD_EDGE_RISING:
      trellis.setPixelColor(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, X_DIM * Y_DIM, 0, 255))); //on rising
      noteOn(2, note_sequence[evt.bit.NUM], 127);
      break;
    case SEESAW_KEYPAD_EDGE_FALLING:
      color = maincolor;
      trellis.setPixelColor(evt.bit.NUM, color); //off falling
      noteOff(2, note_sequence[evt.bit.NUM], 0);
      break;
  }
  MidiUSB.flush();
  return nullptr;
}

void setup() {

  Serial.begin(31250);

  if (!trellis.begin()) {
    Serial.println("failed to begin trellis");
    while (1) delay(1);
  }

  Serial.print("Init...");

  /* the array can be addressed as x,y or with the key number */
  for (int i = 0; i < Y_DIM * X_DIM; i++) {
    trellis.setPixelColor(i, Wheel(map(i, 0, X_DIM * Y_DIM, 0, 255))); //addressed with keynum
    trellis.show();
    delay(20);
  }

  for (int y = 0; y < Y_DIM; y++) {
    for (int x = 0; x < X_DIM; x++) {
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, onKey);
      trellis.setPixelColor(x, y, maincolor); //addressed with x,y
      trellis.show(); //show all LEDs
      delay(20);
    }
  }
  for (int i = 0; i < Y_DIM * X_DIM; i++) {
    state[i] = maincolor;
  }
  Serial.println(" Ready!");
}

void loop() {
  trellis.show();
  trellis.read();
  midiEventPacket_t rx;
  do {
    rx = MidiUSB.read();
    if (rx.header != 0) {
      auto type1 = rx.header & 0x0F;
      auto type2 = rx.byte1 & 0xF0;
      uint8_t note = rx.byte2;
      auto key = note - 24;
      uint8_t vel = rx.byte3;
      uint8_t c = (rx.byte1 & 0x0F) + 1; // channel
      switch (c) {
        case 16:
          lightCol(col);
          col = col < (X_DIM - 1) ? col + 1 : 0;
          break;
        case 15:
          lightRow(row);
          row = row < (Y_DIM - 1) ? row + 1 : 0;
          break;
        case 14:
          reset();
          break;
        case 13:
          setColor(key);
          break;
        default:
          if(key>=0 & key<=63){
            if (type1 == 0x08 && type2 == 0x80) {
              trellis.setPixelColor(key, maincolor);
              state[key]=0x0;
            }
            if (type1 == 0x09 && type2 == 0x90) {
              if (rx.byte3) {
                  if(currentColor == -1) {
                    trellis.setPixelColor(key, Wheel(key*3.9));
                    state[key]=Wheel(key*3.9);
                  } else {
                    trellis.setPixelColor(key, currentColor);
                    state[key]=currentColor;
                  }
              } else {
                trellis.setPixelColor(key, maincolor);
                state[key]=maincolor;
              }
            }
          }
          break;
      }
    }
  } while (rx.header != 0);
}

// CHANGES ACTIVE COLOR BASED ON TROWASOFT TRIGSEQ CH COLORS
void setColor(int key) {
  Serial.println(key);
  switch (key) {
    case 28:
      currentColor = RED;
      break;
    case 29:
      currentColor = ORANGE;
      break;
    case 30:
      currentColor = YELLOW;
      break;
    case 31:
      currentColor = GREEN;
      break;
    case 32:
      currentColor = CYAN;
      break;
    case 33:
      currentColor = BLUE;
      break;
    case 34:
      currentColor = PURPLE;
      break;
    case 35:
      currentColor = PINK;
      break;
    case 36:
      currentColor = RED;
      break;
    case 37:
      currentColor = ORANGE;
      break;
    case 38:
      currentColor = YELLOW;
      break;
    case 39:
      currentColor = GREEN;
      break;
    case 40:
      currentColor = CYAN;
      break;
    case 41:
      currentColor = BLUE;
      break;
    case 42:
      currentColor = PURPLE;
      break;
    case 43:
      currentColor = PINK;
      break;
    default:
      break;
  }
  
}

void lightRow(int row) {
  if (lastrow != row) {
    dimRow(lastrow);
  }
  for (int i = (row * X_DIM); i < ((row * X_DIM) + X_DIM); i++) {
    if (state[i]!=0x0 && state[i]!=0){
      trellis.setPixelColor(i, state[i]);
    } else {
      trellis.setPixelColor(i, seesaw_NeoPixel::Color(10, 0,0));
    }
  }
  lastrow = row;
}
void dimRow(int row) {
  for (int i = (row * X_DIM); i < ((row * X_DIM) + X_DIM); i++) {
    trellis.setPixelColor(i, state[i]);
  }
}
void lightCol(int col) {
  dimCol(lastcol);
  for (int i = col; i < Y_DIM * X_DIM; i++) {
    if (state[i]!=0x0 && state[i]!=0){
      if ((i % 4) == 0) { // assuming 4/4 timing
        trellis.setPixelColor(i, state[i]);
      } else {
        trellis.setPixelColor(i, state[i]);
      }
    } else {
      trellis.setPixelColor(i, seesaw_NeoPixel::Color(30,30,30));
    }
    
    
    i = i + (Y_DIM - 1);
  }
  lastcol = col;
  //trellis.show();
}
void dimCol(int col) {
  for (int i = col; i < X_DIM * Y_DIM; i++) {
    trellis.setPixelColor(i, state[i]);
    i = i + (Y_DIM - 1);
  }
  //trellis.show();
}

void allOff() {
  for (int i = 0; i < Y_DIM * X_DIM; i++) {
    trellis.setPixelColor(i, maincolor);
  }
  trellis.show();
}

void reset() {
  col = 0;
  row = 1;
  allOff();
}
