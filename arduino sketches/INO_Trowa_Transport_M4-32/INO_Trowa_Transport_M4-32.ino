// Trellis M4 MIDI Keypad CC
// sends 32 notes, pitch bend & a CC from accelerometer tilt over USB MIDI
// Specifically designed to use with Stoermelder's MIDI-CAT for VCVRack
// to control Channel (keys 17-32), Play Pattern (9-16) and Edit Pattern (1-8)
// of Trowasoft multiSeq via Impromptu Pads. Use MIDI-CAT to map to the pads. Set CV output per pad
// See accompanying .vcv example file.

#include <Adafruit_Sensor.h>
#include <Adafruit_ADXL343.h>
#include <Adafruit_NeoTrellisM4.h>

#define MIDI_CHANNEL     0  // default channel # is 0
// Set the value of first note, C is a good choice. Lowest C is 0.
// 36 is a good default. 48 is a high range. Set to 24 for a bass machine.
#define FIRST_MIDI_NOTE 36

Adafruit_ADXL343 accel = Adafruit_ADXL343(123, &Wire1);

int xCC = 1;  //choose a CC number to control with x axis tilting of the board. 1 is mod wheel, for example.

int last_xbend = 0;
int last_ybend = 0;
int r, g, b = 0;

Adafruit_NeoTrellisM4 trellis = Adafruit_NeoTrellisM4();

void setup(){
  Serial.begin(115200);
  //while (!Serial);
  Serial.println("MIDI keypad & pitchbend!");
  
  randomSeed(analogRead(0));
  r = random(0,256);
  g = random(0,256);
  b = random(0,256);
  
  trellis.begin();
  trellis.setBrightness(80);

  // USB MIDI messages sent over the micro B USB port
  Serial.println("Enabling MIDI over USB");
  trellis.enableUSBMIDI(true);
  trellis.setUSBMIDIchannel(MIDI_CHANNEL);
  
  if(!accel.begin()) {
    Serial.println("No accelerometer found");
    while(1);
  }

  theaterChase(trellis.Color(r,g,b),50);
  Serial.println("Ready!");
}
void nOff(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t nOff = {0x08, 0x80 | channel, pitch, velocity};
  MidiUSB.sendMIDI(nOff);
}
void nOn(byte channel, byte pitch, byte velocity) {
  midiEventPacket_t nOn = {0x09, 0x90 | channel, pitch, velocity};
  MidiUSB.sendMIDI(nOn);
}
void loop() {
  // put your main code here, to run repeatedly:
  trellis.tick();  
  while (trellis.available()){
    keypadEvent e = trellis.read();
    if (trellis.isPressed(24) && trellis.isPressed(16)){
      Serial.println("MENU COMBO PRESSED");
      trellis.noteOn(127, 64);
      delay(5);
      nOff(1, 127, 64);
    } else if (trellis.isPressed(23) && trellis.isPressed(31)){
      trellis.noteOn(126, 64);
      delay(5);
      nOff(1, 126, 64);
    } else {
      int key = e.bit.KEY;
      if (e.bit.EVENT == KEY_JUST_PRESSED) {
        if (key >= 16) {
          allOff();
        } else if (key >= 8) {
          playOff();
        } else if (key >= 0) {
          editOff();
        }
        trellis.show();
        trellis.setPixelColor(key, Wheel(key*8));
        trellis.noteOn(FIRST_MIDI_NOTE+key, 64);
        delay(5);
        nOff(1, FIRST_MIDI_NOTE+key, 64);
      }
      else if (e.bit.EVENT == KEY_JUST_RELEASED) {
        // ignore!
      }
    }
  }

  // Check for accelerometer
  sensors_event_t event;
  accel.getEvent(&event);
  int xbend = 0;
  int ybend = 0;

  if (abs(event.acceleration.y) < 2.0) {  // 2.0 m/s^2
    ybend = 8192; // 8192 means no bend
  } else {
    if (event.acceleration.y > 0) {
      ybend = ofMap(event.acceleration.y, 2.0, 10.0, 8192, 0, true);  // 2 ~ 10 m/s^2 is downward bend
    } else {
      ybend = ofMap(event.acceleration.y, -2.0, -10.0, 8192, 16383, true);  // -2 ~ -10 m/s^2 is upward bend
    }
  }
  if (ybend != last_ybend) {
    trellis.pitchBend(ybend);
    last_ybend = ybend;
  }

  if (abs(event.acceleration.x) < 2.0) {  // 2.0 m/s^2
    xbend = 0;
  } else {
    if (event.acceleration.x > 0) {
      xbend = ofMap(event.acceleration.x, 2.0, 10.0, 0, 127, true);  // 2 ~ 10 m/s^2 is upward bend
    } else {
      xbend = ofMap(event.acceleration.x, -2.0, -10.0, 0, 127, true);  // -2 ~ -10 m/s^2 is downward bend
    }
  }
  if (xbend != last_xbend) {
//    Serial.print("X mod: "); Serial.println(xbend);
    trellis.controlChange(xCC, xbend);  //xCC is set at top of sketch. e.g., CC 1 is Mod Wheel
    last_xbend = xbend;
  }

  trellis.sendMIDI(); // send any pending MIDI messages

  delay(10);
}

void theaterChase(uint32_t c, uint8_t wait) {
  for (int j=0; j<10; j++) {  //do 10 cycles of chasing
    for (int q=0; q < 3; q++) {
      for (uint16_t i=0; i < trellis.numPixels(); i=i+3) {
        trellis.setPixelColor(i+q, c);    //turn every third pixel on
      }
      trellis.show();

      delay(wait);

      for (uint16_t i=0; i < trellis.numPixels(); i=i+3) {
        trellis.setPixelColor(i+q, 0);        //turn every third pixel off
      }
    }
  }
}

void allOff() {
  for (uint16_t i=16; i < trellis.numPixels(); i++) {
    trellis.setPixelColor(i, 0x0);    //turn 17-32 off
  }
}
void playOff() {
  for (uint16_t i=8; i < 16; i++) {
    trellis.setPixelColor(i, 0x0);    //turn 9-16 off
  }
}
void editOff() {
  for (uint16_t i=0; i < 8; i++) {
    trellis.setPixelColor(i, 0x0);    //turn 1-8 off
  }
}


// floating point map
float ofMap(float value, float inputMin, float inputMax, float outputMin, float outputMax, bool clamp) {
    float outVal = ((value - inputMin) / (inputMax - inputMin) * (outputMax - outputMin) + outputMin);

    if (clamp) {
      if (outputMax < outputMin) {
        if (outVal < outputMax)  outVal = outputMax;
        else if (outVal > outputMin)  outVal = outputMin;
      } else {
        if (outVal > outputMax) outVal = outputMax;
        else if (outVal < outputMin)  outVal = outputMin;
      }
    }
    return outVal;

}

// Input a value 0 to 255 to get a color value.
// The colours are a transition r - g - b - back to r.
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return trellis.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return trellis.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return trellis.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}
