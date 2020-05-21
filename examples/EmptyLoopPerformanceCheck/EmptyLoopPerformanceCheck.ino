// EmptyLoopPerformanceCheck.ino
// A trivial sketch which shows best-case loop() refresh performance.
//
// philj 20may2020
//
#include "CPUmeter.h"
CPUmeter meter;

// xxxxx (nearly) empty loops/sec seems to be about as good as you can get
// 31470 loops/sec with an Arduino UNO, 16MHz.
// 47400 loops/sec with an Arduino 33 Nano BLE.

////////////////////////////////////////////////////////////////////////////////
void showID(void)
{
  // Forgot what sketch was loaded to this board?
  Serial.println(
    F( "Running " __FILE__ ",\nBuilt " __DATE__ ));
}

////////////////////////////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ; // for Leonardo USB...

  showID();
}

////////////////////////////////////////////////////////////////////////////////
//Print out a summary every once in a while
void checkMeterSummary(void)
{
  static auto lastSummary = millis();
  const int updateRate = 10000;
  auto now = millis();
  if (now - lastSummary > updateRate)
  {
    lastSummary = now;
    meter.longReportTo(Serial);
  }
}

////////////////////////////////////////////////////////////////////////////////
// An almost-empty loop to measure best-case update rate.
// With so little "real work" in the loop, the meter update
// and summary overhead becomes significant
//
void loop() {

  meter.loopUpdate();
  checkMeterSummary();

}
