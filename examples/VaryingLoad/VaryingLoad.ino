
// VaryingLoad.ino
//  Measure approximate CPU load
//  Sees how many times loop() can execute compared to historic best times
//
// Phil Jansen 19may2020
//
#include "CPUmeter.h"

CPUmeter cpuMeter;

///////////////////////////////////////////////////////////////////////////
// SimpleTimer
// timer which can trigger at regular intervals.
//
// (If you need something fancier, the arduino-timer library is nice).
//
class SimpleTimer
{
  public:
    SimpleTimer(long interval)
    {
      lastUpdate = millis();
      updateInterval = interval;
    };

    bool timeIsUp(void)
    {
      bool ready = false;
      auto now = millis();
      if (now - lastUpdate > updateInterval)
      {
        ready = true;
        lastUpdate = now; // time is up -- reset timer
      }
      return ready;
    };

    bool notReady(void)
    {
      return !timeIsUp();
    };

  private:
    long lastUpdate;
    long updateInterval;
};

//////////////////////////////////////////////////////////
// Simulated time-consuming tasks.
//
//////////////////////////////////////////////////////////
// Waste a variable amount of time.
// This is a well-behaved task
// -- it sometimes has nothing to do so finishes quickly
// -- it yields "often enough"
//
static int percentLoad = 0; // start with a low load
static const int timeSlice = 10;  // millisec
//
void checkWasteSomeTime(void)
{
  static SimpleTimer timeWaster(timeSlice);
  if (timeWaster.notReady())
    return;

  if (random(0, 99) <= percentLoad)
  {
    delay(timeSlice); // simulated important thing done!
  }
}

//////////////////////////////////////////////////////////
// changes how busy wastSomeTime() is
//
void checkAdjustLoad(void)
{
  static SimpleTimer loadAdjuster(15000);
  if (loadAdjuster.notReady())
    return;

  percentLoad -= 10; // adjust load
  if (percentLoad < 0)
    percentLoad = 100; // wrap into 0-100%
  Serial.print(F("Load set to "));
  Serial.print(percentLoad);
  Serial.println(F("%"));
}


//////////////////////////////////////////////////////////
// send a CPU loading summary to <Serial>
//
void checkMeterLongReport(void)
{
  static SimpleTimer reportInterval(10000);
  if (reportInterval.notReady())
    return;

  cpuMeter.longReportTo(Serial);
}

//////////////////////////////////////////////////////////
void checkToggleLED()
{
  static SimpleTimer ledInterval(1000);
  if (ledInterval.notReady())
    return;

  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN)); // toggle the LED
}

//////////////////////////////////////////////////////////
void showID(void)
{
  // Forgot what sketch was loaded to this board?
  Serial.println(
    F(
      "Running " __FILE__ ", Built " __DATE__
#ifdef SHOW_BUILDTIME
      ", " __TIME__
#endif
    ));
}

//////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
  while (!Serial)
    ; // for Leonardo USB...

  showID();

  pinMode(LED_BUILTIN, OUTPUT); // set LED pin to OUTPUT

  cpuMeter.setSampleInterval(1000);

  Serial.println( F("Ready."));
}

//////////////////////////////////////////////////////////
void loop() {

  // put your main code here, to run repeatedly:
  cpuMeter.loopUpdate();

  checkToggleLED();
  checkWasteSomeTime();
  checkAdjustLoad();
  checkMeterLongReport();
}
