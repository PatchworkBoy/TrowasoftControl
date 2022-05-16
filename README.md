# TrowasoftControl
Set of Arduino sketches for Neotrellis 8x8 Feather M4 Express &amp; NeoTrellisM4 (4x8) for use specifically with Trowasoft multiSeq module for VCVRack2, with accompanying Rack file. 

Arduino sketches in C++ using regular Arduino stack and MIDIUSB (https://github.com/arduino-libraries/MIDIUSB), as seems to be the most responsive & latency free method of doing it.

* NeoTrellis 8x8 Feather M4 Express: https://www.adafruit.com/product/1929
* NeoTrellis 4x8 M4: https://www.adafruit.com/product/4020
* VCVRack2: https://vcvrack.com/Rack#get
* Stoermelder PACK-ONE: https://github.com/stoermelder/vcvrack-packone
* Trowasoft-VCV: https://github.com/j4s0n-c/trowaSoft-VCV
* Video demo: https://www.youtube.com/watch?v=a8XQVMCNVKE

![Adafruit NeoTrellis 8x8 and NeoTrellisM4](https://github.com/PatchworkBoy/TrowasoftControl/blob/3362476d3a1776453b7b238ee099932958ca3c74/media/8x8%20and%204x8.jpg)

![VCVRack Patch](https://github.com/PatchworkBoy/TrowasoftControl/blob/959118bad4653d588e2d4e5e872be8c08393e04d/media/vcvrack.jpg)


Now with pre-mapped MixMode for MindMeld 16ch with Auxspander... and Shapemaster triggers.
Sequencer mode (default) - left 2x columns choose Channel 1-16 in Trowasoft MultiSeq. Right column chooses editing pattern 1-8. Column 3 sets the playing pattern 1-8.

Hold upper left 2 keys to enter MixMode. Hold lower left 2 keys to exit MixMode.

![Transport Control](https://github.com/PatchworkBoy/TrowasoftControl/blob/2d93fae6dd7e0de539dddfe12de9ac80c5b626c4/media/TransportControl.jpg)

In MixMode, Red rows = MUTE, Green rows = SOLO, Orange row = TRIGGERS (hooked up to Shapemaster)

![Trellis Surface](https://github.com/PatchworkBoy/TrowasoftControl/blob/2d93fae6dd7e0de539dddfe12de9ac80c5b626c4/media/MixControl.jpg)

The final 2 keys on Row 7, unlit, are RESET & RUN.
