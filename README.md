# CPUmeter
Empirically determine "cpu load" by comparing current loop() update rate against the historical best case.

This is a little code snippet which might be useful elsewhere...

### Usage
Include the library:
```cpp
#include <CPUmeter.h>
CPUmeter meter;
```

Inside setup(), create your output Stream.  For example, use the Serial Stream:
```cpp
void setup() {
  ...
  Serial.begin(115200);
  ...
}
```

Each time loop() runs, update the meter:
```cpp
void loop() {
  ...
  meter.loopUpdate();
  ...
}
```

When you want to learn about CPU loading, send a report to the Stream.
```cpp
  ...
  // short summary
  meter.reportTo(Serial);

  // more detail
  meter.longReportTo(Serial);
  ...
```

### Example reports

```meter.reportTo(Serial)``` might report

```
CPU load about 22%
```

```meter.longReportTo(Serial)``` might report

```
Best 24149 Worst 2059 current 18808 loops/sec
 Worst delay 159 ms
CPU load about 22%
```

### Simple access to summaries
Some people may want to format their own summary.  Here are some helper methods.

```int meter.getPercentLoad()``` returns the most recent percent CPU load measurment.

```int meter.getWorstDelay()``` returns the slowest loop repetition time found.

```int meter.getDeadlinesMissed()``` returns the number of times the deadline was missed so far

### Configuration
```meter.setSampleInterval(int millisec)```
The default meaurement rate counts loops every 5 seconds.  If you need a 2 second rate, you can set it with ```setSampleInterval(2000);```

```meter.resetStats()```
This resets the meter's collected statistics and starts accumulating all over again.  It might be useful if you want to have a fresh start without resetting the sketch, or skip past a known startup delay.

```meter.setLoopDeadline(int millisec)```
The CPUmeter can also see if loop() takes "too long" to repeat.  This could cause music to stutter, for example.  Setting a loop deadline starts checking for that excessive delay.
Setting a loop deadline adds an extra line to the report.  For example, after calling ```cpuMeter.setLoopDeadline(50)``` might add this line to the long report:
```
   Total 50 ms deadlines missed: 4
```
