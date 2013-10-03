#ifndef PROFILER_H_
#define PROFILER_H_

#include <sys/time.h>
#include <iostream>

#define PROFILE_GATED

#ifdef PROFILE_GATED
extern "C" {
  extern volatile int profile_target_threaded;
}  // extern "C"
#endif

class CProfiler;

class CProfilerSavedState {
public:
  CProfilerSavedState(CProfiler &p);
  ~CProfilerSavedState();
private:
  CProfiler *profiler;
  struct timeval told;
  unsigned int index;
};

class CProfiler {
public:
  unsigned int GetIndex(void) {
    return index;
  }
  void Reset(unsigned int new_index = 0) {
    index = new_index;
    gettimeofday(&told, NULL);
  }
  void Sample(void) {
    gettimeofday(&tnew, NULL);
#ifdef PROFILE_GATED
    if (profile_target_threaded)
#endif
      interval[index++] += tnew.tv_usec - told.tv_usec + 1000000 * (tnew.tv_sec - told.tv_sec);
    gettimeofday(&told, NULL);
    if (index > max_index)
      max_index = index;
  }
  CProfiler(const char *name) : name(name) {}
  ~CProfiler(void) {
    std::cout << name << " results..." << std::endl;
    unsigned int total = 0;
    unsigned int i;
    for (i = 0; i < max_index; i++) {
      total += interval[i];
//      std::cout << interval[i] << std::endl;
    }
    std::cout << "Total: " << total << std::endl;
    const char *sep = "";
    for (i = 0; i < max_index; i++) {
      unsigned int perthou = total == 0 ? 0 : (1000ull * interval[i]) / total;
      std::cout << sep << (perthou / 10) << "." << (perthou % 10) << "%";
      sep = " ";
    }
    std::cout << std::endl;
  }
  friend CProfilerSavedState::CProfilerSavedState(CProfiler &p);
  friend CProfilerSavedState::~CProfilerSavedState();
private:
  std::string name;
  struct timeval told;
  struct timeval tnew;
  unsigned int interval[100];
  unsigned int index;
  unsigned int max_index;
};

inline CProfilerSavedState::CProfilerSavedState(CProfiler &p) {
  profiler = &p;
  told = p.told;
  index = p.index;
}

inline CProfilerSavedState::~CProfilerSavedState() {
  profiler->told = told;
  profiler->index = index;
}

#endif /* PROFILER_H_ */
