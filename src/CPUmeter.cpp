#include "Arduino.h"
#include "CPUmeter.h"
#include "limits.h"

//////////////////////////////////////////////////////////
CPUmeter::CPUmeter(void)
{
  resetStats();
  deadline = 0;
  sampleInterval = defaultSampleInterval;
  worstDelay = 0;
};

//////////////////////////////////////////////////////////
void CPUmeter::update(void)
{
  recentCase = loopCount;
  loopCount = 0;
  bestCase = max(bestCase, recentCase);
  worstCase = min(worstCase, recentCase);
}

//////////////////////////////////////////////////////////
void CPUmeter::loopUpdate(void)
{
  loopCount++;

  auto nowMicros = micros();

  if(firstLoopUpdate) {
    // don't trust microsSinceLastLoop yet
  } else {
    auto loopTimePassed = nowMicros - microsSinceLastLoop;
    worstDelay = max(worstDelay, loopTimePassed);
    microLoopBest = min(microLoopBest, loopTimePassed);
    microLoopWorst = max(microLoopWorst, loopTimePassed);
    if ( (deadline) && (loopTimePassed > deadline))
    {
      // loop stalled enough to cause concern
      deadlinesMissed++;
    }
  }
  
  auto nowMillis = millis();
  if (nowMillis - timeSinceLastUpdate > sampleInterval)
  {
    update(); // accumulate stats
    timeSinceLastUpdate = nowMillis;
  }

  microsSinceLastLoop = nowMicros;
  firstLoopUpdate = false;
};

//////////////////////////////////////////////////////////
void CPUmeter::longReportTo(Stream & client)
{
  // more loops is lower CPU load absorbed by other tasks
  auto seconds = (sampleInterval / 1000.);

  client.print(F("LOOP RATE Best "));
  client.print((int) (bestCase / seconds));
  client.print(F(" Worst "));
  client.print((int) (worstCase / seconds));
  client.print(F(" current "));
  client.print( (int) (recentCase / seconds));
  client.println(F(" loops/sec "));

  client.print(F(" Worst delay "));
  client.print( getWorstDelay() );
  client.println(F(" microsec"));

  client.print(F(" Loop time best: "));
  client.print(microLoopBest);
  client.print(F(" worst: "));
  client.print(microLoopWorst);
  client.println(F(" microsec"));

  if (deadline)
  {
    client.print(F(" Total "));
    client.print( deadline/1000 );
    client.print(F(" ms deadlines missed: "));
    client.println( getDeadlinesMissed() );
  }

  reportTo(client);
}

//////////////////////////////////////////////////////////
int CPUmeter::getPercentLoad(void)
{
  int percentCPU = (int) (100. *(bestCase - recentCase) / (double)bestCase);
  return percentCPU;
};

//////////////////////////////////////////////////////////
int CPUmeter::getWorstDelay(void)
{
  return worstDelay;
};

//////////////////////////////////////////////////////////
int CPUmeter::getDeadlinesMissed(void)
{
  return deadlinesMissed;
};

//////////////////////////////////////////////////////////
void CPUmeter::reportTo(Stream & client)
{
  client.print(F("CPU load about "));
  client.print( getPercentLoad() );
  client.println(F("%"));
};

//////////////////////////////////////////////////////////
void CPUmeter::resetStats(void)
{
  bestCase = LONG_MIN;
  worstCase = LONG_MAX;
  recentCase = 0;
  loopCount = 0;

  deadlinesMissed = 0;
  timeSinceLastUpdate = millis();
  microsSinceLastLoop = micros();

  microLoopBest = LONG_MAX;
  microLoopWorst = 0;
  firstLoopUpdate = true;
};

//////////////////////////////////////////////////////////
void CPUmeter::setLoopDeadline(int newDeadline)
{
  deadline = newDeadline*1000L; // millisec to microsec
  resetStats();
};

//////////////////////////////////////////////////////////
void CPUmeter::setSampleInterval(int newRate)
{
  sampleInterval = newRate;
  resetStats();
};
