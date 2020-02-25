#include "processor.h"
#include <cassert>
#include <chrono>
#include <thread>
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() {
  float CPU_Percentage;

  std::string cpu_name;
  int user, nice, system, idle, iowait, irq, softirq, steal, guest, guest_nice;
  int total;

  std::vector<std::string> cpus = LinuxParser::CpuUtilization();

  std::string cpu = cpus.front();

  std::istringstream linestream(cpu);
  linestream >> cpu_name >> user >> nice >> system >> idle >> iowait >> irq >>
      softirq >> steal >> guest >> guest_nice;

  total = user + nice + system + idle;

  CPU_Percentage = (1.0 - (idle - prev_idle) * 1.0 / (total - prev_total));

  prev_idle = (idle + prev_idle) / 2;
  prev_total = (total + prev_total) / 2;

  return CPU_Percentage;
}