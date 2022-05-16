/*
  This example shows basic usage of the
  MultiTrellis object controlling an array of
  NeoTrellis boards, controlling Trowasoft multiSeq for VCVRack via
  MIDI over USB.

  As is, this example shows use of 4 NeoTrellis boards
  connected together.

  See accompanying .vcv example file.
*/
#include <algorithm>
#include <memory>
#include <map>
#include <Arduino.h>
#include <Adafruit_NeoTrellis.h>
#include <MIDIUSB.h>

#define Y_DIM 8 //number of rows of key
#define X_DIM 8 //number of columns of keys
#define t_size Y_DIM*X_DIM
#define t_length t_size-1

#define maincolor seesaw_NeoPixel::Color(0,0,2) //our 'inactive' seq background color
#define RED seesaw_NeoPixel::Color(155,10,10)
#define ORANGE seesaw_NeoPixel::Color(155,80,10)
#define YELLOW seesaw_NeoPixel::Color(155,155,30)
#define GREEN seesaw_NeoPixel::Color(0,155,0)
#define CYAN seesaw_NeoPixel::Color(40,155,155)
#define BLUE seesaw_NeoPixel::Color(20,50,155)
#define PURPLE seesaw_NeoPixel::Color(100,20,155)
#define PINK seesaw_NeoPixel::Color(155,40,60)
#define ROW_RED seesaw_NeoPixel::Color(10,0,0)
#define W5 seesaw_NeoPixel::Color(5,5,5)
#define W30 seesaw_NeoPixel::Color(30,30,30)
#define W100 seesaw_NeoPixel::Color(100,100,100)
#define mixcolor W5 //our 'inactive' mindmeld background color
#define mixRED seesaw_NeoPixel::Color(5,2,2)
#define mixORANGE seesaw_NeoPixel::Color(25,10,2)
#define mixGREEN seesaw_NeoPixel::Color(0,5,0)

// Variables that holds the current position in the sequence for different elements
// plus a few globals
uint32_t previous = 0;
uint32_t color = 0;
int row = 0;
int lastrow = 0;
int col = 0;
int lastcol = 0;
int cell = 0;
int currentColor = -1;
int r, g, b = 0;
byte channel = 0; // base MIDI channel to talk on. 0 = 1;
byte mm_channel = channel+1; // 2
byte mm_active = 0;

// Array of midi-note values to be mapped top left to bottom right - C0 thru D#5 = 24 thru 87
byte note_sequence[] = {
  24, 25, 26, 27, 28, 29, 30, 31,
  32, 33, 34, 35, 36, 37, 38, 39,
  40, 41, 42, 43, 44, 45, 46, 47,
  48, 49, 50, 51, 52, 53, 54, 55,
  56, 57, 58, 59, 60, 61, 62, 63,
  64, 65, 66, 67, 68, 69, 70, 71,
  72, 73, 74, 75, 76, 77, 78, 79,
  80, 81, 82, 83, 84, 85, 86, 87
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

// Array to store individual led states for trowasoft
int state[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
// Array to store individual key states for trowasoft
int keystate[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
// Array to store individual led states for mindmeld
int mm_state[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};
// Array to store individual key states for mindmeld
int mm_keystate[] = {
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0,
  0, 0, 0, 0, 0, 0, 0, 0
};

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

uint32_t WheelDim(byte WheelPos, uint8_t maxBright) {
  WheelPos = 255 - WheelPos;
  uint16_t r = 0, g = 0, b = 0;

  if (WheelPos < 85) {
    r = 255 - WheelPos * 3;
    b = WheelPos * 3;
  } else if (WheelPos < 170) {
    WheelPos -= 85;
    g = WheelPos * 3;
    b = 255 - WheelPos * 3;
  } else {
    WheelPos -= 170;
    r = WheelPos * 3;
    g = 255 - WheelPos * 3;
  }

  r = r * maxBright / 255;
  g = g * maxBright / 255;
  b = b * maxBright / 255;

  return seesaw_NeoPixel::Color(r, g, b);
}

void noteOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t noteOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOn);
}

void noteOff(byte channel, byte pitch, byte velocity) { // unused but available
  midiEventPacket_t noteOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(noteOff);
}

void controlChange(byte channel, byte control, byte value) { 
  midiEventPacket_t event = {0x0B, 0xB0 | channel, control, value};
  MidiUSB.sendMIDI(event);
}

void spc(byte i, int c){
  trellis.setPixelColor(i,c);
}

//define a callback for key presses
TrellisCallback onKey(keyEvent evt) {
  auto const now = millis();
  auto const keyId = evt.bit.NUM;
  if (mm_active == 1) {
    channel = mm_channel;
  } else {
    channel = 0;
  }
  switch (evt.bit.EDGE)
  {
    case SEESAW_KEYPAD_EDGE_RISING:
      spc(evt.bit.NUM, Wheel(map(evt.bit.NUM, 0, X_DIM * Y_DIM, 0, 255))); //on rising
      noteOn(channel, note_sequence[evt.bit.NUM], 127);
      if (mm_active == 1){
        mm_keystate[keyId] = 1;
      } else {
        keystate[keyId] = 1;
      }
      break;
    case SEESAW_KEYPAD_EDGE_FALLING:
      if (mm_active == 1){
        mm_keystate[keyId] = 0;
        color = mm_state[keyId];
        spc(evt.bit.NUM, color);
      } else {
        keystate[keyId] = 0;
        color = state[keyId];
        spc(evt.bit.NUM, color); //off falling
      }
      noteOn(channel, note_sequence[evt.bit.NUM], 0); // running state, using velocities rather than noteOff
      break;
  }
  MidiUSB.flush();
  return nullptr;
}

void baseMixerColors(int i) {
  if ((i>=0 && i<=7) || (i>=16 && i<=23) || (i>=32 && i<=39) || (i>=48 && i<=50)) {
    mm_state[i] = mixRED;
  } // ...for mixer
  if ((i>=8 && i<=15) || (i>=24 && i<=31) || (i>=40 && i<=47)) {
    mm_state[i] = mixGREEN;
  } // ...for mixer
  if (i>=56 && i<t_size) {
    mm_state[i] = mixORANGE;
  } // ...for mixer  
}

void activeMixerColors(int i) {
  if ((i>=0 && i<=7) || (i>=16 && i<=23) || (i>=32 && i<=39) || (i>=48 && i<=50)) {
    mm_state[i] = RED;
  } // ...for mixer
  if ((i>=8 && i<=15) || (i>=24 && i<=31) || (i>=40 && i<=47)) {
    mm_state[i] = GREEN;
  } // ...for mixer
  if (i>=56 && i<t_size) {
    mm_state[i] = ORANGE;
  } // ...for mixer  
}

void setup() {
  Serial.begin(31250);

  if (!trellis.begin()) {
    Serial.println("failed to begin trellis");
    while (1) delay(1);
  } else {
    Serial.println("Init..."); 
  }

  for (int y = 0; y < Y_DIM; y++) {
    for (int x = 0; x < X_DIM; x++) {
      //activate rising and falling edges on all keys
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_RISING, true);
      trellis.activateKey(x, y, SEESAW_KEYPAD_EDGE_FALLING, true);
      trellis.registerCallback(x, y, onKey);
    }
  }
  for (int i = 0; i < t_size; i++) { // build our grid inactive color maps
    state[i] = maincolor; // ...for sequencer
    baseMixerColors(i);
  }
  randomSeed(analogRead(0));
  r = random(0, 256);
  g = random(0, 256);
  b = random(0, 256);
  theaterChase(seesaw_NeoPixel::Color(r, g, b), 50);
  Serial.println("Ready!");
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
      uint8_t key = note - (note_sequence[0]);
      uint8_t vel = rx.byte3;
      uint8_t c = (rx.byte1 & 0x0F) + 1; // check MIDI channel and...
      switch (c) {
        case 16: // trowasoft clock comms incoming
          //Serial.print("Clock-like Event (inc Reset)");
          if (key == 12) {
            reset();
          } else if (key == 13) {
            lightCol(col);
            col = col < (X_DIM - 1) ? col + 1 : 0;
          } else if (key == 14) {
            lightRow(row);
            row = row < (Y_DIM - 1) ? row + 1 : 0;
          } else if (key == 15) {            
            lightCell();
          } else {
            break;
          }
          break;
        case 15: // mindmeld comms incoming
          //Serial.print("MindMeld: ");
          //Serial.println(key);
          mm_mode(key, type1, type2, vel);
          break;
        case 14: // palette/overlay picker
          //Serial.print("TrowaControl: ");
          if (key == 103) {
            key=127;
            // enter mix mode
            mm_active = 1;
            mm_mode(key, type1, type2, vel);
            //Serial.println(key);
            
          } else if (key == 102) {
            key=126;
            // exit mix mode
            mm_active = 0;
            //Serial.println("Exit MixMode");
          } else {
            if (mm_active == 1)  {
              //Serial.print("MixMode: ");
              //Serial.println(mm_active);
            } else {
              //Serial.println(key);
              setColor(key);
            }
          }
          break;
        default:
          if (mm_active == 0) {
            if (key >= 0 & key <= 63) {
              if (type1 == 0x08 && type2 == 0x80) {
                spc(key, maincolor);
                state[key] = maincolor;
              }
              if (type1 == 0x09 && type2 == 0x90) {
                if (rx.byte3) {
                  if (currentColor == -1) {
                    spc(key, Wheel(key * 3.9));
                    state[key] = Wheel(key * 3.9);
                  } else {
                    spc(key, currentColor);
                    state[key] = currentColor;
                  }
                } else {
                  spc(key, maincolor);
                  state[key] = maincolor;
                }
              }
              if (type1 == 0x0B && type2 == 0xB0) {
                //Serial.println("ERR: CC received - expected NoteOn");
              }
            }
          }
          break;
      }
    }
  } while (rx.header != 0);
}

void mm_mode(int k, byte t1, byte t2, int v){
  if (k >= 0 & k <= 63) {
    if (t1 == 0x08 && t2 == 0x80) {
      baseMixerColors(k);
      if (mm_active == 1) spc(k, mm_state[k]);
    }
    if ((t1 == 0x0B && t2 == 0xB0) || (t1 == 0x09 && t2 == 0x90)) {
      //Serial.print("NoteOn received: ");
      //Serial.println(k);
      if (v>0) {
        activeMixerColors(k);
        if (mm_active == 1) spc(k, mm_state[k]);
      } else {
        baseMixerColors(k);
        if (mm_active == 1) spc(k, mm_state[k]);
      }
    }
  } else if (k == 127) {
    for (int i = 0; i < t_size; i++) {
      spc(i,mm_state[i]);
    }   
  }
}

void setColor(uint8_t k) {
  if (mm_active == 0) {
    //Serial.print("Setting Color for: ");
    //Serial.print((uint8_t)k);
    //Serial.print(" to ");
    switch (k) {
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
      case 52:
        currentColor = W30;
        break;
      default:
        break;
    }
    //Serial.println(currentColor);
  }
}

void lightRow(int row) {
  if (mm_active == 0) {
    if (lastrow != row) {
      dimRow(lastrow);
    }
    for (int i = (row * X_DIM); i < ((row * X_DIM) + X_DIM); i++) {
      if (state[i] != maincolor) {
        spc(i, state[i]);
      } else {
        spc(i, ROW_RED);
      }
    }
    lastrow = row;
  }
}
void dimRow(int row) {
  for (int i = (row * X_DIM); i < ((row * X_DIM) + X_DIM); i++) {
    spc(i, state[i]);
  }
}

void lightCol(int col) {
  if (mm_active == 0) {
    dimCol(lastcol);
    for (int i = col; i < t_size; i++) {
      if (state[i] != maincolor) {
        if ((i % 4) == 0) { // assuming 4/4 timing
          spc(i, state[i]);
        } else {
          spc(i, state[i]);
        }
      } else {
        spc(i, W30);
      }
      i = i + (Y_DIM - 1);
    }
    lastcol = col;
  }
}

void dimCol(int col) {
  for (int i = col; i < t_size; i++) {
    spc(i, state[i]);
    i = i + (Y_DIM - 1);
  }
}

void lightCell() {
  if (mm_active == 0) {
    if (state[cell] != maincolor) {
      spc(cell, state[cell]);
    } else {
      spc(cell, W100);
    }
    if (cell - 1 >= 0) {
      spc(cell - 1, state[cell - 1]);
    } else {
      spc(t_length, state[t_length]);
    }
  }
  if (cell < t_length) { // we still update cell count even if mixmode is active
    cell = cell + 1;     // else it'll be out of sync with the sequencer
  } else {               // when we return to it.
    cell = 0;
  }
}

void allOff() {
  if (mm_active == 0) {
    for (int i = 0; i < t_size; i++) {
      spc(i, maincolor);
    }
    trellis.show();
  }
}

void reset() { // we also still allow resets if mixmode is active, again, to 
  col = 0;     // keep our cell ticker in sync
  row = 1;
  cell = 0;
  allOff();
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j = 0; j < 10; j++) { //do 10 cycles of chasing
    for (int q = 0; q < 3; q++) {
      for (uint16_t i = 0; i < t_size; i = i + 3) {
        spc(i + q, c); //turn every third pixel on
      }
      trellis.show();
      delay(wait);
      for (uint16_t i = 0; i < t_size; i = i + 3) {
        spc(i + q, maincolor); //turn every third pixel off
      }
    }
  }
}
