#include "IPv6Timer.h"

IPv6Timer::IPv6Timer(){
  
}

void IPv6Timer::set(clock_time_t interval){
  timer_set(&_timer, interval);
}

void IPv6Timer::reset(){
  timer_reset(&_timer);
}

void IPv6Timer::restart(){
  timer_restart(&_timer);
}

bool IPv6Timer::expired(){
  return timer_expired(&_timer) != 0;
}

clock_time_t IPv6Timer::remaining(){
  return timer_remaining(&_timer);
}
