#ifndef TRACE_H_
#define TRACE_H_

#include <iostream>

extern "C" {
  extern volatile int profile_target_threaded;
}  // extern "C"

class CTrace
{
  static size_t indent;
public:
  CTrace(const char *function_name)
  {
    if (profile_target_threaded)
    {
      int i;
      for (i = indent; i > 0; i--)
        std::cout << "  ";
      std::cout << function_name << std::endl;
    }
    indent++;
  }
  ~CTrace()
  {
    if (indent > 0)
      indent--;
  }
};

#endif /* TRACE_H_ */
