# A Seven Segment Counter

## Description

In this project, I reused an old "seven segment display" salvaged from a microwave oven to build a counter. The project is based on an Arduino Nano. 

## How it works

The salvaged display is a made of four digits that can be individually selected by its cathode pin. Each segment is lit up by the corresponding anode pin. The display is in a common cathode arrangement. 

This means you can display only one digit at a time and thus multiplexing is required. 

The multi-plexing is achieved by selecting each digit's cathode in turn and for each digit, selecting the anodes of every segment to light up.  

The cathode is set to HIGH while the anodes are set to LOW. 
When a cathode is not in use, its pin is set to LOW.
When an anode is not in use, its pin is set to HIGH. 

When done fast enough (50Hz minimum), the persistence of vision let the user see all digits. 

## Drawback

In a first version of this project (check the initial commit of this repo), I used the digitalWrite function to drives the output pins. [But it was way to slow!](https://www.peterbeard.co/blog/post/why-is-arduino-digitalwrite-so-slow/).

So started using the PORT/DDR registers, which turned to be very fast. But I had to find a port that had at least seven available pins to connect all the anodes. 

PORTD was the only one having this configuration. Unfortunately, the first two pins (0 and 1) are also used for the UART opertion (TX/RX). 

Although I succeeded in re-purposing those pins as general purpose output pins, it seems they cannot sink enough current to light up a LED. So, the matching two segments do not light up. 

## References 
 - https://www.peterbeard.co/blog/post/why-is-arduino-digitalwrite-so-slow/
 - https://www.arduino.cc/en/Reference/PortManipulation
 - http://www.pighixxx.com/test/pinouts/boards/nano.pdf
 - http://ww1.microchip.com/downloads/en/DeviceDoc/Atmel-2545-8-bit-AVR-Microcontroller-ATmega48-88-168_Datasheet.pdf
 - https://www.arduino.cc/reference/en/language/functions/external-interrupts/attachinterrupt/ 

## Further improvements

- Use a 74HC595 multiplexer
- or drive the two segments connected on TX/RX with a small mosfet


