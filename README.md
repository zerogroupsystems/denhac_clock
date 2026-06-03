# 4 Digit 7 Segment Clock Project

This is an initial design; we can of course do other things, like using a single
4 digit 7 segment display (see previous commits in the project), using
transistors or mosfets to control the drain pins, et cetera. This is just an
idea I have based on other things I have built in the past.

## Real Time Clock

I added a DS1307 as a real time clock. We can use
[Kevin's RTC library](https://github.com/kgmossey/RTC_DS1307) to drive it.
I have not hooked up the `SQW/OUT` pin, but we might want to add a breakout
header/pin for it for future expansion.

## Driving the display

The idea here is to drive 4 individual digit 7 segment displays; I chose the
[XL-DA3161SRBC](https://mm.digikey.com/Volume0/opasdata/d220001/medias/docus/8903/XL-DA3161SRBC.pdf)
because it looked reasonable in terms of cost and simplicty. We'll drive all of
them from an Arduino; each digit will have a dedicated shift register
[SN74HC595N](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf) associated with it,
connected so that the overflow output from one leads to the input of the next.

The display should be configured like this (Note that it's certainly possible to
configure them in a different order, but this document assumes that we're doing
it this way):

```
            OVR      OVR     OVR
 DATA -> H1 --->  H2 ---> M1 ---> M2
```

Where

*  `H1` is the first digit of the hour
*  `H2` is the second digit of the hour
*  `M1` is the first digit of the minute
*  `M2` is the second digit of the minute
*  `DATA` is data out from the microcontroller
*  `OVR` is overflow out from the corresponding shift register, connected to
   the input of the following shift register

To specify a digit, we need to enable one or more of the segments on the
SN74HC595N. If we want to display the value '3', we need to light up segments
A, B, C, D and G (refer to the datasheet), so we use
```
                +-------- segment G
                |+------- segment F
                ||+------ segment E
                |||+----- segment D
                ||||+---- segment C
                |||||+--- segment B
                ||||||+-- segment A
                VVVVVVV
      binary 0b01001111
```

To display a time value, we need to shift in 4 bytes, one each for the first and
second digits of the hour and one each for the first and second digits of the
minutes. For the indicated shift register connections, we need to output digits
in the *REVERSE ORDER*; i.e. we need to output digits in this order:

```
  M2 M1 H2 H1
```

### Add a 'colon'

Many clocks include a colon `:` between the hour and minute; this could be a
nice addition and has some good usability benefits (e.g. we could toggle the
`:` on and off every second). We don't have a colon in this design, but we can
simulate one using a clever trick the clock kits we used for the soldering
classes employed.

Each segment includes a 'decimal point' (indicated with `DP` in the schematic)
in the lower right corner; we normally wouldn't want to enable them, but we can
use `DP` on the `H2` and `M1` digits to simulate a colon. The `H2` decimal point
is in its lower right corner, and if we rotate the M1 digit LED 180°, its
decimal point will be in the upper-left position; turning on the `DP` pins in
`H2` and `M1` will look like a `:`

The problem, of course, is that the `M1` digit is now "upside down" so the
normal pin enable bits we'd use will result in upside down digits. We can fix
this by either handling M1 digits differently in code, or we can connect them
differently on the board.

The existing schematic has `M1` in a "normal" orientation, so we can't simulate
a colon with it (TODO: let's flip it and decide whether we want to use custom
code or connections; it may depend on which thing we want to emphasize,
programming or PCB layout).

## Setting the clock

I added 2 momentary switches labled `SET` and `CHANGE`. They're connected to
the 2 interrupt pins available on R3 Arduino and compatible devices. I
haven't thought too much yet about how to use them, but I'm thinking
something like

* SET: press once to enable time-setting. If there are no more button presses
  in the next N(=10?) seconds, go back to regular operation. Press SET again
  to move to the next digit

* CHANGE: increment the current digit by 1, with some smart behavior so we
  don't overflow a reasonable value for it.

I thought these would be useful for teaching about how interrupts work. I
added RC debounce components too, instead of relying on software debounce, so
that we'd have some capacitors involved.
