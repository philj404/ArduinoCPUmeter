//////////////////////////////////////////////////////////
class CPUmeter
{
  public:
    CPUmeter(void);
    void update(void);
    void anotherLoop(void);

    void longReport(void);

    void report(void);
    
    static const int updateRate = 5000; // millisec

  private:
    long recentCase;
    long bestCase;
    long worstCase;
    long loopCount;
};

extern CPUmeter cpuMeter;
