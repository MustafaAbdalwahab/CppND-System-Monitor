#include <string>
#include <sstream>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
// REMOVE: [[maybe_unused]] once you define the function
string Format::ElapsedTime(long seconds) { 

    float hh = float(seconds/3600.0f);
    float mm = ((hh - (int)hh)*60);
    int ss = ((mm - (int)mm)*100);

    std::ostringstream stringstream;
    stringstream << (int)hh << ":" << (int)mm << ":" << ss;
    return stringstream.str(); 
}