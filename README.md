# 4 Digit 7 Segment Clock Project

This is an initial design; we can of course do other things, like using 4
indpendent 7 segment displays, using transistors or mosfets to control the
drain pins, et cetera. This is just an idea I have based on other things I
have built in the past.

## Real Time Clock

I added a DS1307 as a real time clock. We can use
[Kevin's RTC library](https://github.com/kgmossey/RTC_DS1307) to drive it.
I have not hooked up the `SQW/OUT` pin, but we might want to add a breakout
header/pin for it for future expansion.

## Driving the display

The idea here is to drive a 4 digit 7 segment display from an Arduino.

The display I have in mind is
[LTC2723](https://www.mouser.com/catalog/specsheets/lite-on_lites01094-1.pdf)
It has 4 digits, but we will only enable 1 at a time, rapidly shifting between
them to use persistence-of-vision to give the illusion of all of them being
lit at the same time. I've used this technique before and it works well.

For each didgit, the display has 8 data lines (1 for each segment, plus
another one for a decimal point that we probably won't use). We'll use an 8
bit shift register [SN74HC595N](https://www.ti.com/lit/ds/symlink/sn74hc595.pdf)
to control power to each segment.

Additionally, we'll use another shift register
[TPIC6595](https://www.ti.com/lit/ds/symlink/tpic6595.pdf) to control the
ground pins for each digit. There are 5 drain pins to consider, one for each
of the digits, and another one that controls a colon ':' and degree '°'.

The 2 shift registers are connected in series, so the overflow from the
SN74HC595N flows into the TPIC6595. For example, to display the value '3' on
the second digit, we would need to shift 2 bytes of data through the shift
register:

* first byte: selecting the drain pin on the TPIC6595. Since we want the 2nd
  digit to be selected, we use
```
                  +------ drain 4 (colon and degree)
                  |+----- drain 3 (fourth digit)
                  ||+---- drain 2 (third digit)
                  |||+--- drain 1 (second digit)
                  ||||+-- drain 0 (first digit)
                 VVVVVV
      binary 0b00000010
```

  at this point, the first byte will be in the SN74HC595N register, which
  isn't where we want it, but we're not done yet

* second byte: selecting the segments on the SN74HC595N. We want to light up
  segments A, B, C, D and G (refer to the datasheet), so we use
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

  once this byte is shifted in, the first byte will have been completely
  shifted to the TPIC6595 (where we want it) and the second byte will be in
  the SN74HC595N (also where we want it).

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
