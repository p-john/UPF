#ifndef HARDBIT_TIME_UTIL
#define HARDBIT_TIME_UTIL

#include <chrono>

namespace time_util {

typedef std::chrono::high_resolution_clock Clock;

/*
 * Computes the time difference in seconds between start and end.
 */
inline double duration(Clock::time_point& start, Clock::time_point& end) {
  return std::chrono::duration_cast<std::chrono::duration<double>>(
      end - start).count();
}


} // namespace time_util

#endif // HARDBIT_TIME_UTIL
