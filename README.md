Visual MIDI CC
==============

A vi-like MIDI continuous controller
------------------------------------

[demo](demo.gif)

Features
 - sends 128 different midi cc values 
 - one vi-style key recording register for on-the-fly macros (more registers coming)

Installation
 - Install SendMidi - Thanks Geert!
   - [SendMidi](https://github.com/gbevin/sendmidi)
 - Building
   - `make main`

Setup
 - Make a virtual midi instrument.
   - MacOS
     - open Audio\ Midi\ Setup.app
     - Window -> Show MIDI Studio
     - open "IAC Driver"
     - check "Device is online"

Usage
 - `builds/main <absolute path to SendMidi binary> <fader height>`
    - eg `builds/main /usr/bin/sendmidi 20`
 - Keys
   - Navigation: h, j, k, l
   - Move a fader: J, K
   - Command line: :
   - Quit command: :q

