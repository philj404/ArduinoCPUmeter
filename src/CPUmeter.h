#ifndef CPU_METER_H
#define CPU_METER_H
//////////////////////////////////////////////////////////
class CPUmeter
{
  public:
    CPUmeter(void);
    void loopUpdate(void);

    int getPercentLoad(void);
    int getWorstDelay(void);
    int getDeadlinesMissed(void);

    void longReportTo(class Stream & client);
    void reportTo(class Stream & client);

    static const int defaultSampleInterval = 5000; // ms between readings
    void setSampleInterval(int newInterval);
    void setLoopDeadline(int newDeadline);

    void resetStats(void);

  private:
    void update(void);

    long sampleInterval;
    long deadline;
        
    long timeSinceLastUpdate; // millisec
    long microsSinceLastLoop;	// microsec
    
    long loopCount;

    bool firstLoopUpdate;	// avoid loop startup issues
    long microLoopBest;
    long microLoopWorst;

    long recentCase;
    long bestCase;
    long worstCase;
    long deadlinesMissed;
    long worstDelay;
};

#endif
