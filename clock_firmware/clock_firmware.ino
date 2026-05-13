// From https://github.com/kgmossey/RTC_DS1307
#include <DS1307.h>

DS1307 rtc;

 // Use the LT2723 LED display
#define _LTC2723 
// Serial data flows through SN74HC595 and overflows to TPIC6B595
#define _DATAFLOW_SN74HC595_TO_TPIC6B595 
#include "FourDigitSevenSegmentDisplay.h"

#define LATCH_PIN  4
#define CLOCK_PIN  5
#define DATA_PIN   6

FourDigitSevenSegmentDisplay display(LATCH_PIN, CLOCK_PIN, DATA_PIN);

// Interrupt handler for the 'SET' button
void handle_set() {
  // TODO: add clock setting
}

// Interrupt handler for the 'CHANGE' button
void handle_change() {
  // TODO: add clock digit changing
}

void setup() {
  attachInterrupt(digitalPinToInterrupt(2), handle_set, RISING);
  attachInterrupt(digitalPinToInterrupt(3), handle_change, RISING);
  rtc.begin();

  pinMode(LED_BUILTIN, OUTPUT);
  Serial.begin(115200);
  uint32_t deadline = millis() + 2000;
  while (!Serial.available() && millis() < deadline) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(100);
    digitalWrite(LED_BUILTIN, LOW);
    delay(100);
  }
  Serial.println("clock ready");
}

void loop() {
  while(true) {
    // TODO: maybe add this to an interrupt handler for the clock? Toggle the colon every second?
    uint16_t value = rtc.hour* 100 +  // shift hour 2 decimal places to the left
                     rtc.minute;
    display.display(value, /*degree=*/false, /*colon=*/false);
    // TODO: print the time to the console?
  }
}
