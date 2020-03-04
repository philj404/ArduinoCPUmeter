// CPUmeter
//  Measure approximate CPU load
//  Sees how many times loop() can execute compared to historic best times
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

    long overshoot(void) {
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

      Serial << "Best " << (int) (bestCase / seconds)
             << " Worst " << (int) (worstCase / seconds)
             << " current " << (int) (recentCase / seconds)
             << " loops/sec " << endl;
      report();
    }
    void report(void)
    {
      int percentCPU = (int) (100. *(bestCase - recentCase) / (double)bestCase);
      Serial << "CPU load about " << percentCPU << "%" << endl;
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
    Serial << "Load set to " << percentLoad << "%" << endl;
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
    Serial << "archiving logs...";
    delay(150); // simulated maintenance
  }
}


//////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////
// are tasks starving a little?
// helps determine if there's "enough" margin to meet
// your requirements
//
// NOTE: This won't run/warn you if the task loop is completely blocked
//
void starvationCheck(void)
{
  static TimerPKJ deadline(20); // millisec to "sluggish" response

  if (deadline.ready())
  {
    // detecting sluggish response..
    Serial << "Refresh is LATE by " << deadline.overshoot() << "ms!" << endl;
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
  Serial << "Ready." << endl;
  Serial << "Running " << __FILE__ << endl;
}

//////////////////////////////////////////////////////////
void loop() {

  // put your main code here, to run repeatedly:

  wasteSomeTime();  // useful stuff...
  maintenance();

  cpuMeter.update();  // measure performance
  starvationCheck();  // performance warning

  if (reporter.ready())
  {
    cpuMeter.longReport();
    //cpuMeter.report();
  }
}
