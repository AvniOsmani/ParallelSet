#include "timer.h"

Timer::Timer(const myint & _inProgress){
  timerInProgress=_inProgress;
  if(timerInProgress==1){
    timerStart=std::chrono::high_resolution_clock::now().time_since_epoch().count();
  }
}
void Timer::start(){
  timerInProgress=1;
  timerStart=std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
void Timer::end(){
  timerInProgress=2;
  timerEnd=std::chrono::high_resolution_clock::now().time_since_epoch().count();
}
mydouble Timer::getTime(){
  if(timerInProgress!=2){
    return -1.0;
  }
  mydouble fR= mydouble(timerEnd)-mydouble(timerStart);
  fR/= 1000000.0;
  return fR;
}
