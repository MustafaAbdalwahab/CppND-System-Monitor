#include <cmath>
#include <string>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) {
  int hh = seconds / 3600;
  int mm = (seconds % 3600);  //(3.6 - 3) = .6
  int ss = (mm % 60);

  return std::to_string(hh) + ":" + std::to_string((int)(mm / 60)) + ":" +
         std::to_string(ss);
}