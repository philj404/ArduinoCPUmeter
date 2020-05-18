//////////////////////////////////////////////////////////
class CPUmeter
{
  public:
    CPUmeter(void);
    void anotherLoop(void);

    void longReport(class Stream & client);
    void report(class Stream & client);

    static const int defaultUpdateRate = 5000; // millisec
    void setUpdateRate(int newRate);
    void setDeadline(int newDeadline);

    void resetStats(void);

  private:
    void update(void);

    long loopCount;
    long updateRate;
    
    long recentCase;
    long bestCase;
    long worstCase;

    long deadline;
    long deadlinesMissed;
    long worstDelay;

    unsigned long timeSinceLastUpdate;
    unsigned long timeSinceLastLoop;
};

extern CPUmeter cpuMeter;
