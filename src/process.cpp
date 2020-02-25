#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"
#include "process.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int _pid) : pid(_pid) {}

// DONE: Return this process's ID
int Process::Pid() { return pid; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() {
  cpu = LinuxParser::Cpu(pid);
  return cpu;
}

// DONE: Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid); }

// DONE: Return this process's memory utilization
string Process::Ram() {
  ram = std::stof(LinuxParser::Ram(pid));
  return std::to_string((int)ram) + " MB";
}

// DOEN: Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid); }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a) const {

  //TODO: add parameter to choose the feature of sort by default its the ram
  return ram > a.ram;
  //return cpu > a.cpu;
  //return pid > a.pid;
}