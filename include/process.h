#ifndef PROCESS_H
#define PROCESS_H

#include <string>
/*
Basic class for Process representation
It contains relevant attributes as shown below
*/
class Process {
 public:
  Process(int _pid);
  int Pid();                               // DONE: See src/process.cpp
  std::string User();                      // DONE: See src/process.cpp
  std::string Command();                   // DONE: See src/process.cpp
  float CpuUtilization();                  // TODO: See src/process.cpp
  std::string Ram();                       // DOEN: See src/process.cpp
  long int UpTime();                       // DONE: See src/process.cpp
  bool operator<(Process const& a) const;  // TODO: See src/process.cpp

  // TODO: Declare any necessary private members
 private:
  int pid{0};
  float ram{0};
  float uptime{0};
  float cpu{0};
};

#endif