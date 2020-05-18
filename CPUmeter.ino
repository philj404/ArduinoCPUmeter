
// CPUmeter
//  Measure approximate CPU load
//  Sees how many times loop() can execute compared to historic best times
//
// Phil Jansen 29apr2020
//
//#include "Streaming.h"
//#include "limits.h"

#include <timer.h>
auto timer = timer_create_default(); // create a timer with default settings

#include "CPUmeter.h"


bool updateMeter(void)
{
  cpuMeter.update();
  return true; // repeat
}

//////////////////////////////////////////////////////////
// Waste a variable amount of time.
// This is a well-behaved task
// -- it sometimes has nothing to do so finishes quickly
// -- it yields "often enough"
//
static int percentLoad = 0; // start with a low load
static const int timeSlice = 10;  // millisec

bool wasteSomeTime(void)
{
  if (random(0, 99) <= percentLoad)
  {
    delay(timeSlice); // simulated important thing done!
  }
  return true; // repeat
}

//////////////////////////////////////////////////////////
bool adjustLoad(void)
{
  percentLoad -= 10; // adjust load
  if (percentLoad < 0)
    percentLoad = 100; // wrap into 0-100%
  Serial.print(F("Load set to "));
  Serial.print(percentLoad);
  Serial.println(F("%"));
  return true; // repeat
}

//////////////////////////////////////////////////////////
// perform a "marginally long" maintenance task
// Note that this may be long enough to (for example) make audio/video stutter
//
bool maintenance(void)
{
  Serial.print(F("archiving logs..."));
  delay(150); // simulated large maintenance task
  return true; // repeat
}

//////////////////////////////////////////////////////////
// starvationCheck() -- are tasks starving a little?
// helps determine if there's "enough" margin to meet
// your requirements
//
// NOTE: This won't run/warn you if the task loop just blocks/halts
//
bool starvationCheck(void)
{
  static const int timeLimit = 50;
  static unsigned long previous = 0;
  auto current = millis();
  auto theDelay = current - previous;
  auto overshoot = theDelay - timeLimit;
  if (overshoot > 10)
  {
    // detecting sluggish response..
    Serial.print(F("Refresh is LATE by "));
    Serial.print(overshoot);
    Serial.println(F("ms!"));
    // TODO:
    // There may be a serious problem.
    // Turn off motors, set brakes etc.
  }
  previous = current;
  return true;
}

bool cpuMeterLongReport(void)
{
  cpuMeter.longReport();
  return true; // repeat
}

//////////////////////////////////////////////////////////
bool toggle_led(void *) {
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED
  return true; // repeat? true
}

//////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ; // for Leonardo USB...

  // Forgot what sketch was loaded to this board?
  //
  // Hint1: use the F() macro to keep const strings in FLASH and save RAM
  // Hint2: "Compiler " "catenates consecutive "
  //         "strings together"
  //         (can improve readability for very long strings)
  //
  Serial.println(
    F(
      "Running " __FILE__ ", Built " __DATE__
#ifdef SHOW_BUILDTIME
      ", " __TIME__
#endif
    ));

  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT

  // toggle_led() every sec
  timer.every(1000, toggle_led);

  timer.every(37000, maintenance);
  timer.every(15000, adjustLoad);
  timer.every(   10, wasteSomeTime);
  timer.every(10000, cpuMeterLongReport);
  timer.every(CPUmeter::updateRate, updateMeter);
  timer.every(   50, starvationCheck);  // stutter warning

  Serial.println( F("Ready."));
}

//////////////////////////////////////////////////////////
void loop() {

  // put your main code here, to run repeatedly:

  timer.tick();

  cpuMeter.anotherLoop();
}
