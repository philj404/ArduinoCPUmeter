#include "Arduino.h"
#include "CPUmeter.h"
#include "limits.h"

CPUmeter cpuMeter;

//////////////////////////////////////////////////////////
CPUmeter::CPUmeter(void)
{
  resetStats();
  deadline = 0;
  updateRate = defaultUpdateRate;
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
void CPUmeter::anotherLoop(void)
{
  loopCount++;

  auto now = millis();
  auto loopTimePassed = now - timeSinceLastLoop;
  worstDelay = max(worstDelay, loopTimePassed);
  if ( (deadline) && (loopTimePassed > deadline))
  {
    // loop stalled enough to cause concern
    deadlinesMissed++;
  }
  
  if (now - timeSinceLastUpdate > updateRate)
  {
    update(); // accumulate stats
    timeSinceLastUpdate = now;
  }

  timeSinceLastLoop = now;
};

//////////////////////////////////////////////////////////
void CPUmeter::longReport(Stream & client)
{
  // more loops is lower CPU load absorbed by other tasks
  auto seconds = (updateRate / 1000.);

  client.print(F("Best "));
  client.print((int) (bestCase / seconds));
  client.print(F(" Worst "));
  client.print((int) (worstCase / seconds));
  client.print(F(" current "));
  client.print( (int) (recentCase / seconds));
  client.println(F(" loops/sec "));

  client.print(F(" Worst delay "));
  client.print( worstDelay );
  client.println(F(" ms"));

  if (deadline)
  {
    client.print(F(" Total "));
    client.print( deadline );
    client.print(F(" ms deadlines missed: "));
    client.println( deadlinesMissed );
  }

  report(client);
}
//////////////////////////////////////////////////////////
void CPUmeter::report(Stream & client)
{
  int percentCPU = (int) (100. *(bestCase - recentCase) / (double)bestCase);
  client.print(F("CPU load about "));
  client.print(percentCPU);
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
  timeSinceLastUpdate = timeSinceLastLoop = millis();
};

//////////////////////////////////////////////////////////
void CPUmeter::setDeadline(int newDeadline)
{
  deadline = newDeadline;
  resetStats();
};

//////////////////////////////////////////////////////////
void CPUmeter::setUpdateRate(int newRate)
{
  updateRate = newRate;
  resetStats();
};
