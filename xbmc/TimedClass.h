#ifndef TIMEDCLASS_H_
#define TIMEDCLASS_H_

#include <sys/time.h>
#include <iostream>

#ifndef TIMEDCLASS_DECLARE_SCOPE
extern
#endif
__thread bool TimedClassInScope;


template <typename T>
class CTimedClassReport {
public:
  ~CTimedClassReport() {
    std::cout << __PRETTY_FUNCTION__ << " class construction: " << total << std::endl;
  }
  static unsigned int total;
};

/* This class is required to be the first base class of the class being timed */

template <typename T>
class CTimedClass {
public:
  CTimedClass(void) {
    gettimeofday(&tstart, NULL);
  }
  void FinishedConstructing(void) {
    if (TimedClassInScope) {
      struct timeval now;
      gettimeofday(&now, NULL);
      info.total += now.tv_usec - tstart.tv_usec + 1000000 * (now.tv_sec - tstart.tv_sec);
    }
  }
private:
  struct timeval tstart;
  static CTimedClassReport<T> info;
};


#endif /* TIMEDCLASS_H_ */
