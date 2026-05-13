#ifndef FOUR_DIGIT_SEVEN_SEGMENT_DISPLAY_H
#define FOUR_DIGIT_SEVEN_SEGMENT_DISPLAY_H

#include "Arduino.h"

/*

 Wiring diagram:

 depends on whether you want data to flow like:
     SN74HC595 -> TPIC6B595 (_DATAFLOW_SN74HC595_TO_TPIC6B595)
  or
     TPIC6B595 -> SN74HC595 (_DATAFLOW_TPIC6B595_TO_SN74HC595)

 SN74HC595 B/1    -> RES -> BLQ39A42 B/16 LTC2723 B/15
 SN74HC595 C/2    -> RES -> BLQ39A42 C/13 LTC2723 C/2
 SN74HC595 D/3    -> RES -> BLQ39A42 D/3  LTC2723 D/6
 SN74HC595 E/4    -> RES -> BLQ39A42 E/5  LTC2723 E/5
 SN74HC595 F/5    -> RES -> BLQ39A42 F/11 LTC2723 F/16
 SN74HC595 G/6    -> RES -> BLQ39A42 G/15 LTC2723 G/7
 SN74HC595 H/7    -> RES -> BLQ39A42 DP/7 LTC2723 DP/3
 SN74HC595 GND/8  -> GND
 SN74HC595 CLR/10 -> VCC
 SN74HC595 SRK/11 -> clock_pin
 SN74HC595 RCK/12 -> latch_pin
 SN74HC595 OE/13  -> GND

 for data flow TPIC6B595 -> SN74HC595
   SN74HC595 SER/14 -> TPIC6B595 SER_OUT/18
 else
   SN74HC595 SER/14 -> data_pin

 SN74HC595 A/15   -> RES -> BLQ39A42 A/14  LTC2723 A/13
 SN74HC595 VCC/16 -> VCC

 TPIC6B595 VCC/2      -> VCC

 for data flow TPIC6B595 -> SN74HC595
   SN74HC595 QH/9 -> TPIC6B595 SER/3
 else
   TPIC6B595 SER/3      -> data_pin

 TPIC6B595 D0/4       -> BLQ39A42 D0/1 LTC2723 D0/1
 TPIC6B595 D1/5       -> BLQ39A42 D1/2 LTC2723 D1/14
 TPIC6B595 D2/6       -> BLQ39A42 D2/6 LTC2723 D2/11
 TPIC6B595 D3/7       -> BLQ39A42 D3/8 LTC2723 D3/8
 TPIC6B595 CLR/8      -> VCC
 TPIC6B595 G/9        -> GND
 TPIC6B595 RCK/12     -> latch_pin
 TPIC6B595 SRK/13     -> clock_pin
 TPIC6B595 D4/14      -> BLQ39A42 DC/4  LTC2723 DL/12
 TPIC6B595 D5/15      -> BLQ39A42 DD/10 nc
 TPIC6B595 SER_OUT/18 -> SN74HC595 SER/14

 BLQ39A42 COL/12 -> VCC
 BLQ39A42 DEG/9  -> VCC

       SN74HC595
        ________
  B   --| 1 16 |-- VCC
  C   --| 2 15 |-- A
  D   --| 3 14 |-- SER
  E   --| 4 13 |-- OE
  F   --| 5 12 |-- RCK
  G   --| 6 11 |-- SCK
  H   --| 7 10 |-- CLR
  GND --| 8  9 |-- CARRY
        --------

       TPIC6B595
         ________
  NC   --| 1 20 |-- NC
  VCC  --| 2 19 |-- GND
  SER  --| 3 18 |-- OUT
  D0   --| 4 17 |-- D7
  D1   --| 5 16 |-- D6
  D2   --| 6 15 |-- D5
  D3   --| 7 14 |-- D4
  CLR  --| 8 13 |-- SRK
  G    --| 9 12 |-- RCK
  GND  --|10 11 |-- GND
         --------

           A
          ===
       F ||G || B
          ===
       E ||  || C
          ===  0
           D    DP

       BLQ39A42
        ________
   D0 --| 1 16 |-- B
   D1 --| 2 15 |-- G
   D  --| 3 14 |-- A
   DC --| 4 13 |-- C
   E  --| 5 12 |-- COL
   D2 --| 6 11 |-- F
   DP --| 7 10 |-- DD
   D3 --| 8  9 |-- DEG
        --------

       LTC2723
        ________
   D0 --| 1 16 |-- F
 C/L3 --| 2 15 |-- B/L2
   DP --| 3 14 |-- D1
   nc --| 4 13 |-- A/L1
    E --| 5 12 |-- DL
    D --| 6 11 |-- D2
    G --| 7 10 |-- no pin
   D3 --| 8  9 |-- nc
        --------

 http://www.ti.com/lit/ds/symlink/tpic6b595.pdf
 http://www.ti.com/lit/ds/symlink/sn74hc595.pdf
 http://www.adafruit.com/datasheets/BL-Q39A-42.PDF
 https://www.mouser.com/datasheet/2/239/lite-on_lites01094-1-1737181.pdf

*/

#define A_SEG       1
#define B_SEG       2
#define C_SEG       4
#define D_SEG       8
#define E_SEG      16
#define F_SEG      32
#define G_SEG      64
#define COLON_SEGS (A_SEG + B_SEG)
#define DEGREE_SEG C_SEG


#if !(defined(_BLQ39A42) ^ defined(_LTC2723))
#error "you must #define exactly one of (_BLQ39A42, _LTC2723)"
#endif

#if !(defined(_DATAFLOW_SN74HC595_TO_TPIC6B595) ^ defined(_DATAFLOW_TPIC6B595_TO_SN74HC595))
#error "you must #define exactly one of (_DATAFLOW_SN74HC595_TO_TPIC6B595, _DATAFLOW_TPIC6B595_TO_SN74HC595)"
// _DATAFLOW_SN74HC595_TO_TPIC6B595 means:
//    serial data flows into SN74HC595 and then "overflows" to TPIC6B595
// _DATAFLOW_TPIC6B595_TO_SN74HC595 means:
//    serial data flows into TPIC6B595 and then "overflows" to SN74HC595
#endif

typedef struct {
  short digit;
  bool dp;
} DisplayDigit;

class FourDigitSevenSegmentDisplay {
 public:
  FourDigitSevenSegmentDisplay(
      int latch_pin, int clock_pin, int data_pin)
    : latch_pin_(latch_pin), clock_pin_(clock_pin),
      data_pin_(data_pin) {
    pinMode(latch_pin_, OUTPUT);
    pinMode(data_pin_, OUTPUT);
    pinMode(clock_pin_, OUTPUT);
  }

  void displayRaw(uint8_t digits[4], bool degree, bool colon) {
    for (int i = 0; i < 4; ++i) {
      uint8_t drain_value = 1 << i;
#ifdef _BLQ39A42
      if (colon) {
        drainCode |= 1 << 5;
      }
      if (degree) {
        drainCode |= 1 << 4;
      }
#endif  // _BLQ39A42
      write(digits[i],  drain_value);
      write(0, 0);
    }
#ifdef _LTC2723
    // Default: none are on.
    uint8_t display_value = 0;
    if (degree) {
      display_value = DEGREE_SEG;
      if (colon) {
        display_value += COLON_SEGS;
      }
    } else if (colon) {
      display_value = COLON_SEGS;
    }
    uint8_t drain_value = 1 << 4;
    write(display_value, drain_value);
    write(0, 0);
#endif  // _LTC2723
  }

  void display(DisplayDigit display_digits[4], bool degree, bool colon) {
    uint8_t digits[4];
    for (int i = 0; i < 4; ++i) {
      uint8_t digit = display_digits[i].digit;
      bool dp = display_digits[i].dp;
      digits[i] = getByteCodeForDigit(digit);
      if (dp) {
        digits[i] += 128;
      }
    }
    displayRaw(digits, degree, colon);
  }


  void display_int(uint16_t v, bool degree, bool colon, int dp_position = -1) {
    DisplayDigit digits[4];
    uint16_t remainder = v;
    if (v >= 1000) {
      uint8_t thousands = v / 1000;
      remainder = remainder - 1000 * thousands;
      digits[0].digit = thousands;
      digits[0].dp = dp_position == 1;
    } else {
      digits[0].digit = -1;
      digits[0].dp = false;
    }
    if (v >= 100) {
      uint8_t hundreds = remainder / 100;
      remainder = remainder - 100 * hundreds;
      digits[1].digit = hundreds;
      digits[1].dp = dp_position == 2;
    } else {
      digits[1].digit = -1;
      digits[1].dp = false;
    }
    if (v >= 10) {
      uint8_t tens = remainder / 10;
      remainder = remainder - 10 * tens;
      digits[2].digit = tens;
      digits[2].dp = dp_position == 3;
    } else {
      digits[2].digit = -1;
      digits[2].dp = false;
    }
    digits[3].digit = remainder;
    digits[3].dp = dp_position == 4;
    display(digits, degree, colon);
  }

  void display_float(float f, bool degree, bool colon) {
    if (f < 0) {
      return;
    }
    float v = f;
    int dp_position = 4;
    while (v < 1000) {
      v *= 10;
      dp_position -= 1;
    }
    display_int((uint16_t) v, degree, colon, dp_position);
  }

  void write(uint8_t source_value, uint8_t drain_value) const {
    digitalWrite(latch_pin_, LOW);
#ifdef _DATAFLOW_SN74HC595_TO_TPIC6B595
    // serial data flows into SN74HC595
    shiftOut(data_pin_, clock_pin_, MSBFIRST, drain_value);
    shiftOut(data_pin_, clock_pin_, MSBFIRST, source_value);
#else
    shiftOut(data_pin_, clock_pin_, MSBFIRST, source_value);
    shiftOut(data_pin_, clock_pin_, MSBFIRST, drain_value);
#endif
    digitalWrite(latch_pin_, HIGH);
  }

  static int getByteCodeForDigit(int digit) {
    switch(digit) {
      case 0: return A_SEG + B_SEG + C_SEG + D_SEG + E_SEG + F_SEG;
      case 1: return B_SEG + C_SEG;
      case 2: return A_SEG + B_SEG + D_SEG + E_SEG + G_SEG ;
      case 3: return A_SEG + B_SEG + C_SEG + D_SEG + G_SEG;
      case 4: return B_SEG + C_SEG + F_SEG + G_SEG;
      case 5: return A_SEG + C_SEG + D_SEG + F_SEG + G_SEG;
      case 6: return A_SEG + C_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case 7: return A_SEG + B_SEG + C_SEG;
      case 8: return A_SEG + B_SEG + C_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case 9: return A_SEG + B_SEG + C_SEG + F_SEG + G_SEG;
      case 10: return A_SEG + B_SEG + C_SEG + E_SEG + F_SEG + G_SEG;
      case 11: return C_SEG + D_SEG + E_SEG +F_SEG + G_SEG;
      case 12: return D_SEG + E_SEG + G_SEG;
      case 13: return B_SEG + C_SEG + D_SEG + E_SEG + G_SEG;
      case 14: return A_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case 15: return A_SEG + E_SEG + F_SEG + G_SEG;

      default: return 0;
    }
  }

  static int getByteCodeForChar(char c) {
    switch(c) {
      case '0': return A_SEG + B_SEG + C_SEG + D_SEG + E_SEG + F_SEG;
      case '1': return B_SEG + C_SEG;
      case '2': return A_SEG + B_SEG + D_SEG + E_SEG + G_SEG ;
      case '3': return A_SEG + B_SEG + C_SEG + D_SEG + G_SEG;
      case '4': return B_SEG + C_SEG + F_SEG + G_SEG;
      case '5': return A_SEG + C_SEG + D_SEG + F_SEG + G_SEG;
      case '6': return A_SEG + C_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case '7': return A_SEG + B_SEG + C_SEG;
      case '8': return A_SEG + B_SEG + C_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case '9': return A_SEG + B_SEG + C_SEG + F_SEG + G_SEG;
      case 'A': case 'a': return A_SEG + B_SEG + C_SEG + E_SEG + F_SEG + G_SEG;
      case 'B': case 'b': return C_SEG + D_SEG + E_SEG +F_SEG + G_SEG;
      case 'C': case 'c': return D_SEG + E_SEG + G_SEG;
      case 'D': case 'd': return B_SEG + C_SEG + D_SEG + E_SEG + G_SEG;
      case 'E': case 'e': return A_SEG + D_SEG + E_SEG + F_SEG + G_SEG;
      case 'F': case 'f': return A_SEG + E_SEG + F_SEG + G_SEG;

      default: return 0;
    }
  }

 private:
  int latch_pin_;
  int clock_pin_;
  int data_pin_;
};

#endif //  FOUR_DIGIT_SEVEN_SEGMENT_DISPLAY_H

