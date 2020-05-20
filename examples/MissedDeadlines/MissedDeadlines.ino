//  MissedDedlines.ino
//  See if loop() can run "often enough" to handle all the tasks it needs
//  to perform.
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
// perform a "marginally long" maintenance task
// Note that this may be long enough to (for example) make audio/video stutter
//
void checkMaintenance(void)
{
  static SimpleTimer maintenanceInterval(37000);
  if (maintenanceInterval.notReady())
    return;

  Serial.print(F("archiving logs..."));
  delay(150); // simulated large maintenance task
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

  cpuMeter.setSampleInterval(4000);
  cpuMeter.setLoopDeadline(50);

  Serial.println( F("Ready."));
}

//////////////////////////////////////////////////////////
void loop() {

  // put your main code here, to run repeatedly:
  cpuMeter.loopUpdate();

  checkToggleLED();
  checkMaintenance();
  checkMeterLongReport();
}
