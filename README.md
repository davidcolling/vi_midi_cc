Visual MIDI CC
==============

A vi-like MIDI continuous controller
------------------------------------

Features
    * sends 128 different midi cc values 
    * one vi-style key recording register for on-the-fly macros (more registers coming)

Installation
    * Dependency
        - [SendMidi](https://github.com/gbevin/sendmidi)
    * Building
        - `make main`

Setup
    * Make a virtual midi instrument.
        * MacOS
            - open Audio\ Midi\ Setup.app
            - Window -> Show MIDI Studio
            - open "IAC Driver"
            - check "Device is online"

Usage
    * `builds/main <absolute path to SendMidi binary> <fader height>`


