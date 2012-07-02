#ifndef __IPV6TIMER_H__
#define __IPV6TIMER_H__

extern "C"{
  #include "timer.h"
}

class IPv6Timer{
  private:
    struct timer _timer;
    
  public:
    IPv6Timer();
    void set(clock_time_t interval);//In milliseconds
    void reset();
    void restart();
    bool expired();
    clock_time_t remaining(); 
    
};

#endif
