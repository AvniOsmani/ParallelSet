#ifndef MY_TIMER_H
#define MY_TIMER_H

#include<chrono>

typedef long int myint;
typedef double mydouble;

class Timer{
private:
  myint timerStart;
  myint timerEnd;
  myint timerInProgress;
public:
  Timer(const myint & =0);
  void start();
  void end();
  mydouble getTime();
};

#endif
