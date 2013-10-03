#ifndef NESTEDPROFILER_H_
#define NESTEDPROFILER_H_

#include <iostream>
#include <algorithm>
#include <string>
#include <vector>
#include <stdint.h>
#include <sys/time.h>

#define NESTEDPROFILE_GATED

#ifdef NESTEDPROFILE_GATED
extern "C" {
  extern volatile int profile_target_threaded;
}  // extern "C"
#endif

class CNestedProfileResult
{
  friend class CNestedProfileResults;
  friend class CNestedProfiler;
  std::string function;
  uint32_t time;
public:
  static bool Compare(const CNestedProfileResult &first, const CNestedProfileResult &second)
  {
    return first.time > second.time;
  }
};

class CNestedProfileResults
{
  friend class CNestedProfiler;
  std::vector<CNestedProfileResult> history;
  struct timeval start_time;
  size_t currently_measuring; // 0 for nothing, else index of current function + 1
public:
  ~CNestedProfileResults(void)
  {
    if (history.size() > 0)
    {
      std::sort(history.begin(), history.end(), CNestedProfileResult::Compare);
      std::cout << "Nested profiling results" << std::endl;
      uint32_t total = 0;
      size_t i;
      for (i = 0; i < history.size(); i++)
        total += history[i].time;
      for (i = 0; i < history.size(); i++)
        std::cout << "\"" << history[i].function << "\", " << history[i].time << ", " << ((double) history[i].time * 100. / total) << std::endl;
    }
  }
};

class CNestedProfiler
{
  static CNestedProfileResults results;
  size_t previously_measuring;
public:
  CNestedProfiler(const char *function)
  {
    struct timeval now;
    gettimeofday(&now, NULL);
    if (results.currently_measuring != 0)
#ifdef NESTEDPROFILE_GATED
      if (profile_target_threaded)
#endif
        results.history[results.currently_measuring-1].time += (now.tv_sec - results.start_time.tv_sec) * 1000000 + now.tv_usec - results.start_time.tv_usec;
    size_t i;
    for (i = 0; i < results.history.size(); i++)
      if (results.history[i].function == function)
        break;
    if (i == results.history.size())
    {
      CNestedProfileResult r;
      r.function = function;
      r.time = 0;
      results.history.push_back(r);
    }
    previously_measuring = results.currently_measuring;
    results.currently_measuring = i+1;
    gettimeofday(&results.start_time, NULL);
  }

  ~CNestedProfiler(void)
  {
    struct timeval now;
    gettimeofday(&now, NULL);
#ifdef NESTEDPROFILE_GATED
    if (profile_target_threaded)
#endif
      results.history[results.currently_measuring-1].time += (now.tv_sec - results.start_time.tv_sec) * 1000000 + now.tv_usec - results.start_time.tv_usec;
    results.currently_measuring = previously_measuring;
    results.start_time = now;
  }
};

#ifdef DEFINE_NESTEDPROFILE_STATICS
CNestedProfileResults CNestedProfiler::results;
#endif

#endif /* NESTEDPROFILER_H_ */
