# BITBA555
BITBA555 is an open source/open hardware hybrid analog/digital monophonic synthesizer with a digital oscillator section and analog filter, amplifier, and envelope generator sections. It also features a fully 3D printable enclosure with snap-fit so that no additional hardware is necessary.

The ethos of BITBA555 is to be as close to 100% open source and DIY as possible. All code versions, hardware schematic versions, simulation files, board files, and even 3D printed enclosure designs will be shared on this page. In order to make digesting this information more accessible, a YouTube tutorial series about the design process has been created as well.

![Schematic](/BITBA555_tutorial5/Hardware/BITBA555_tutorial5_schem.png)
![DIY PCB2](/PCB_2.JPG)
![3D Printed Enclosure](/EnclosureCollage.JPG)

### Episode 000 - Introduction
https://www.youtube.com/watch?v=HbYsLG5ng4g

### Episode 001 - BITBA555 Peripheral Setup
https://www.youtube.com/watch?v=4lw_u7kjT1c

### Episode 002 - BITBA555 Digital Oscillator Finished
https://www.youtube.com/watch?v=C3nDFvrVApo

### Episode 003 - BITBA555 Analog Filter and Power Supply
https://www.youtube.com/watch?v=efnBC_zZduA

### Episode 004 - BITA555 Analog Envelope Generator and Amplifier
https://www.youtube.com/watch?v=gevD3INR1kk

### Episode 005 - BITBA555 Board Layout and Fabrication
https://www.youtube.com/watch?v=fseVMmLj8Y8

### Episode 007 - BITBA555 Enclosure Design
https://www.youtube.com/watch?v=aarRZwyo6pU

## BITBA555 Features
* Digital oscillator section with sine, triangle, square, and sawtooth wave output
  * Dynamic programming techniques utilized to allow for underpowered ATmega328p to oscillate efficiently
* MIDI input and output with input support for MIDI note on and off messages
* Velocity sensitivity on wave amplitude for MIDI note on messages
* 4-pole analog OTA-based low-pass filter with cutoff and resonance control
  * Filter self oscillates at max resonance
* Analog OTA-based amplifier section with gain control
* Analog ADSR envelope generator 
* Fully 3D printed enclosure

To hear BITBA555 in a song: https://soundcloud.com/theroomdisconnect/bitbac1d/s-NRedz
