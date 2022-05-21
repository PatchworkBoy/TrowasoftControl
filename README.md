# TrowasoftControl
Set of Arduino sketches for Neotrellis 8x8 Feather M4 Express &amp; NeoTrellisM4 (4x8) for use specifically with Trowasoft multiSeq module for VCVRack2, with accompanying Rack file. 

Arduino sketches in C++ using regular Arduino stack and MIDIUSB (https://github.com/arduino-libraries/MIDIUSB), as seems to be the most responsive & latency free method of doing it.

UPDATE: The NEXT revision (in progress!) will do away with necessity for a NeoTrellisM4 (4x8), altho it's still my personal preferred method for controlling the 8x8, and also brings 8x pages (mapped to MIDI Chs 1-8) for the 8x8, to be expanded to 15 pages (mapped to Ch1-15) in the future. With all menu/page control for the 8x8 taking place solely on Ch16.

* Video demo: https://www.youtube.com/watch?v=a8XQVMCNVKE (pre-MixMode addition)
* Video demo2: https://www.youtube.com/watch?v=XsOMpJZBjNU (post-MixMode addition)



![Adafruit NeoTrellis 8x8 and NeoTrellisM4](https://github.com/PatchworkBoy/TrowasoftControl/blob/3362476d3a1776453b7b238ee099932958ca3c74/media/8x8%20and%204x8.jpg)

![VCVRack Patch](https://github.com/PatchworkBoy/TrowasoftControl/blob/959118bad4653d588e2d4e5e872be8c08393e04d/media/vcvrack.jpg)


Now with additional pre-mapped MixMode for MindMeld MixMaster and Auxspander... and Shapemaster triggers.


# Neotrellis M4

* NeoTrellis 4x8 M4: https://www.adafruit.com/product/4020 (Still available as separate parts; board, case, polymer keypads, from The Pi Hut)
...in portrait mode with connector to the right.

Sequencer mode (default)...
* Cols 1 & 2 choose Channel 1-16 in Trowasoft MultiSeq (1-8 top to bottom on Col1, 9-16 top to bottom on Col2) and displays on NeoTrellis 8x8. 
* Col 3 sets the currently playing pattern from 1-8.
* Col 4 sets the pattern you're editing on the NeoTrellis 8x8.

MixMode...
* Simultaneously press the top keys of Col1 & Col2 to enter MixMode. 
* Simultaneously press the bottom keys of Col1 & Col2 to exit MixMode - you'll probably end up on Ch9 or 16 of the sequencer depending which key went down first. Just tap to whatever channel you want...

![Transport Control](https://github.com/PatchworkBoy/TrowasoftControl/blob/2d93fae6dd7e0de539dddfe12de9ac80c5b626c4/media/TransportControl.jpg)


# Neotrellis 8x8 Feather M4 Express

* NeoTrellis 8x8 Feather M4 Express: https://www.adafruit.com/product/1929

Driving the 8x8 in Seq mode is fairly obvious, with the caveat that you occasionally need to double tap buttons to get them to toggle due to something quirky between Trowasoft's sequencer display and Stoermelder's MIDI-CAT. This happens with any MIDI controller - it's not due to anything in the Neotrellis sketches.

In MixMode: Red rows = MUTE, Green rows = SOLO, Orange row = TRIGGERS (hooked up to Shapemaster), and everything should toggle on first press without problems.

* Rows 1 - 4: Channels 1-16 of MixMaster
* Rows 5 & 6: Groups 1-4 of MixMaster, Aux Returns 1-4 of Auxspander
* Row 7: MixMaster Master Mute (1), Dim (2), Mono (3), Impromptu Clocked Reset (7) and Run (8). 4-6 are free to remap to whatever.
* Row 8: Shapemaster triggers 1 thru 8. 1 & 2 mapped to VCF. 3 - 5 mapped to BGA VCAs for 8th / 16th / 32nd beat chopper effect.
* (NB: Shapemaster PRO lets you hook it to your clock and sync it up)

![Trellis Surface](https://github.com/PatchworkBoy/TrowasoftControl/blob/2d93fae6dd7e0de539dddfe12de9ac80c5b626c4/media/MixControl.jpg)


# Required Software

* VCVRack2: https://vcvrack.com/Rack#get (+ Fundamentals)
* Stoermelder PACK-ONE Dev Builds: https://github.com/stoermelder/vcvrack-packone/releases/tag/Nightly
* Trowasoft-VCV: https://github.com/j4s0n-c/trowaSoft-VCV/releases/tag/v2.0.4-alpha
* MindMeld: https://library.vcvrack.com/?query=&brand=MindMeld&tag=&license=
* Impromptu Clocked: https://library.vcvrack.com/ImpromptuModular/Clocked
* Impromptu CV-Pad: https://library.vcvrack.com/ImpromptuModular/Cv-Pad
* SV Modular Drums: https://library.vcvrack.com/?query=drum&brand=SV+Modular&tag=&license=
* Count Modula Chances: https://library.vcvrack.com/?query=Chances&brand=Count+Modula&tag=&license= 
* Bog Audio Mix2: https://library.vcvrack.com/?query=Mix2&brand=Bogaudio&tag=&license=
* Befaco KickALL: https://library.vcvrack.com/Befaco/Kickall
* AS Reverb Stereo FX: https://library.vcvrack.com/?query=Reverb+Stereo&brand=AS&tag=&license=
* Inklen Cable Colour Key: https://library.vcvrack.com/?query=&brand=Inklen&tag=&license=


# IMPORTANT
Do these steps FIRST before opening the vcvrack sample file to avoid losing the pre-done MIDI Mappings.

* Subscribe to MindMelder, Impromptu, SV Modular, Count Modula, Bog Audio, Befaco, AS and Inklen plugins from the VCV Library - https://library.vcvrack.com
* Get the Stoermelder Dev build for your platform from links at bottom of the page at Stoermelder PACK-ONE v2-Dev repo link above. Place the .vcvplugin file in Rack/plugins (on MacOS, usually ~/Documents/Rack/plugins ... not sure about Windows - sorry!)
* Get the Trowasoft dev build for your platform from the link above. Extract the zip. Place the extracted folder in Rack/plugins (on MacOS, usually ~/Documents/Rack/plugins ... not sure about Windows - sorry!)
* Launch VCVRack... Library > Update All

Flash your Neotrellii and plug 'em in.

NOW open the vcvrack sample file!
