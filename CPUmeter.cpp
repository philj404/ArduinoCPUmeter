#include "Arduino.h"
#include "CPUmeter.h"
#include "limits.h"

CPUmeter cpuMeter;

//////////////////////////////////////////////////////////
CPUmeter::CPUmeter(void)
{
  bestCase = LONG_MIN;
  worstCase = LONG_MAX;
  recentCase = 0;
  loopCount = 0;
};

void CPUmeter::update(void)
{
  recentCase = loopCount;
  loopCount = 0;
  bestCase = max(bestCase, recentCase);
  worstCase = min(worstCase, recentCase);
}
void CPUmeter::anotherLoop(void)
{
  loopCount++;
};

void CPUmeter::longReport(void)
{
  // more loops is lower CPU load absorbed by other tasks
  auto seconds = (updateRate / 1000.);

  Serial.print(F("Best "));
  Serial.print((int) (bestCase / seconds));
  Serial.print(F(" Worst "));
  Serial.print((int) (worstCase / seconds));
  Serial.print(F(" current "));
  Serial.print( (int) (recentCase / seconds));
  Serial.println(F(" loops/sec "));
  report();
}
void CPUmeter::report(void)
{
  int percentCPU = (int) (100. *(bestCase - recentCase) / (double)bestCase);
  Serial.print(F("CPU load about "));
  Serial.print(percentCPU);
  Serial.println(F("%"));
};
