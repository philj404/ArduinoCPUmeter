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
      m_period(period), m_previous(0)
    {};

    bool ready(void)  // true when it's time to do something -- resets
    {
      auto currentTime = millis();
      bool isReady = (m_period > 0) && (currentTime - m_previous >= m_period);
      if (isReady)
      {
        //reset();
        m_previous = currentTime;
      }
      return isReady;
    }

    void period(long newPeriod) // negative period turns timer off
    {
      m_period = newPeriod;
    };

    void reset(void)
    {
      m_previous = millis();
    }
  private:
    long m_period;
    unsigned long m_previous;
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
        bestCase = max(bestCase, loopCount);
        worstCase = min(worstCase, loopCount);
        report(loopCount);
        loopCount = 0;
      }
    }
    void report(long loopsToScale)
    {
      // more loops is lower CPU load absorbed by other tasks
      int percentCPU = (int) (100. *(bestCase - loopsToScale) / (double)bestCase);
      auto seconds = (updateRate / 1000.);

      Serial << "Best " << bestCase / seconds << " loops/sec; ";
      Serial << "Worst " << worstCase / seconds << " loops/sec " << endl;
      Serial << "current " << loopsToScale / seconds << " loops/sec " ;
      Serial << "CPU load at " << percentCPU << "%" << endl;
    };

  private:
    TimerPKJ timer;
    long bestCase;
    long worstCase;
    long loopCount;
    static const int updateRate = 5000; // millisec
};

CPUmeter cpuMeter;

//////////////////////////////////////////////////////////
// waste a variable amount of time.
void wasteSomeTime(void)
{
  static int percentLoad = 0;
  static const int timeSlice = 10;
  static TimerPKJ importantWork(timeSlice);
  static TimerPKJ loadAdjuster(15000);

  if (importantWork.ready())
  {
    if (random(1, 100) <= percentLoad)
    {
      delay(timeSlice);
    }
  }
  
  if (loadAdjuster.ready())
  {
    percentLoad -= 10; // change load 10%
    if (percentLoad < 0)
      percentLoad = 100; // wrap into 0-100%
      Serial << "Load should be " << percentLoad << "%" << endl;
  }
}

//////////////////////////////////////////////////////////
void setup() {
  // put your setup code here, to run once:
  Serial.begin(115200);
}

//////////////////////////////////////////////////////////
void loop() {
  // put your main code here, to run repeatedly:

  wasteSomeTime();
  cpuMeter.update();
}
