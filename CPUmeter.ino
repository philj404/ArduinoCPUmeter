// CPUmeter
//  Measure approximate CPU load
//  Sees how many times loop() can execute compared to historic best times
//
// Phil Jansen 3apr2020
//
#include "Streaming.h"
#include "limits.h"

//////////////////////////////////////////////////////////
class TimerPKJ
{
  public:
    TimerPKJ(long period):  // in millisec
      m_period(period), m_overshoot(0)
    {
      m_previous = millis();
      //m_previous = 0;
    };

    bool ready(void)  // true when it's time to do something -- resets
    {
      auto currentTime = millis();
      bool isReady = (m_period > 0) && (currentTime - m_previous >= m_period);
      if (isReady)
      {
        reset();
        //m_previous = currentTime;
      }
      return isReady;
    }

    void period(long newPeriod) // negative period turns timer off
    {
      m_period = newPeriod;
    };

    // reset timeout
    void reset(void)
    {
      auto currentTime = millis();
      m_overshoot = currentTime - m_previous;
      m_previous = currentTime;
    }

    long overshoot(void) // is task running late?
    {
      return m_overshoot;
    };

  private:
    long m_period;
    unsigned long m_previous;
    long m_overshoot;
};

//////////////////////////////////////////////////////////
class CPUmeter
{
  public:
    CPUmeter(void): timer(updateRate)
    {
      bestCase = LONG_MIN;
      worstCase = LONG_MAX;
      loopCount = 0;
    };
    void update(void)
    {
      loopCount++;
      if (timer.ready())
      {
        recentCase = loopCount;
        loopCount = 0;
        bestCase = max(bestCase, recentCase);
        worstCase = min(worstCase, recentCase);
        //report(loopCount);
      }
    }

    void longReport(void)
    {
      // more loops is lower CPU load absorbed by other tasks
      auto seconds = (updateRate / 1000.);

      Serial << F("Best ") << (int) (bestCase / seconds)
             << F(" Worst ") << (int) (worstCase / seconds)
             << F(" current ") << (int) (recentCase / seconds)
             << F(" loops/sec ") << endl;
      report();
    }
    void report(void)
    {
      int percentCPU = (int) (100. *(bestCase - recentCase) / (double)bestCase);
      Serial << F("CPU load about ") << percentCPU << F("%") << endl;
    };

  private:
    TimerPKJ timer;
    long recentCase;
    long bestCase;
    long worstCase;
    long loopCount;
    static const int updateRate = 5000; // millisec
};

CPUmeter cpuMeter;
TimerPKJ reporter(10000);

//////////////////////////////////////////////////////////
// Waste a variable amount of time.
// This is a well-behaved task
// -- it sometimes has nothing to do so finishes quickly
// -- it yields "often enough"
//
void wasteSomeTime(void)
{
  static int percentLoad = 0; // start with a low load
  static const int timeSlice = 10;
  static TimerPKJ importantWork(timeSlice);
  static TimerPKJ loadAdjuster(15000);

  if (importantWork.ready())
  {
    if (random(0, 99) <= percentLoad)
    {
      delay(timeSlice); // simulated important thing done!
    }
  }

  if (loadAdjuster.ready())
  {
    percentLoad -= 10; // adjust load
    if (percentLoad < 0)
      percentLoad = 100; // wrap into 0-100%
    Serial << F("Load set to ") << percentLoad << F("%") << endl;
  }
}

//////////////////////////////////////////////////////////
// perform a "marginally long" maintenance task
// Note that this may be long enough to (for example) make audio/video stutter
//
void maintenance(void)
{
  static TimerPKJ springCleaning(37000);

  if (springCleaning.ready())
  {
    Serial << F("archiving logs...");
    delay(150); // simulated large maintenance task
  }
}

//////////////////////////////////////////////////////////
// starvationCheck() -- are tasks starving a little?
// helps determine if there's "enough" margin to meet
// your requirements
//
// NOTE: This won't run/warn you if the task loop just blocks/halts
//
void starvationCheck(void)
{
  static TimerPKJ deadline(20); // response becoming "sluggish"?

  if (deadline.ready())
  {
    // detecting sluggish response..
    Serial << F("Refresh is LATE by ") << deadline.overshoot() << F("ms!") << endl;
    // TODO:
    // There may be a serious problem.
    // Turn off motors, set brakes etc.
  }
  deadline.reset();
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
  Serial <<
         F(
           "Running " __FILE__ ", Built " __DATE__
#ifdef SHOW_BUILDTIME
           ", " __TIME__
#endif
         )
         << endl;

  Serial << F("Ready.") << endl;
}

//////////////////////////////////////////////////////////
void loop() {

  // put your main code here, to run repeatedly:

  wasteSomeTime();  // useful stuff...
  maintenance();

  cpuMeter.update();  // measure performance
  starvationCheck();  // stutter warning

  if (reporter.ready())
  {
    cpuMeter.longReport();
    //cpuMeter.report();
  }
}
