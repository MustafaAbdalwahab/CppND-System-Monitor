#include <dirent.h>
#include <ncurses.h>
#include <unistd.h>
#include <iostream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> kernel >> version;
  }
  return version;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() {
  float memtotal{0};
  float memfree{0};
  float memavailable{0};
  float Buffer{0};
  float Cached{0};

  std::ifstream stream(kProcDirectory + kMeminfoFilename);
  if (stream.is_open()) {
    std::string line;
    std::string key;
    while (1) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key;

      if (key == "MemTotal:") {
        linestream >> memtotal;
      } else if (key == "MemFree:") {
        linestream >> memfree;
      } else if (key == "MemAvailable:") {
        linestream >> memavailable;
      } else if (key == "Cached:") {
        linestream >> Cached;
        break;
      } else if (key == "Buffers:") {
        linestream >> Buffer;
      }
    }
  }

  return float((memtotal - (memfree + Buffer + Cached)) / memtotal);
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() {
  string uptime, idletime;
  string line;
  std::ifstream stream(kProcDirectory + kUptimeFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> idletime;
  }
  return std::stol(uptime);
}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> cpus;
  string line;
  std::ifstream stream(kProcDirectory + kStatFilename);
  if (stream.is_open()) {
    string temp;
    while (std::getline(stream, line)) {
      std::istringstream linestream(line);
      linestream >> temp;
      if (temp.find("cpu") != std::string::npos) {
        cpus.push_back(line);
      }
    }
  }
  return cpus;
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { return LinuxParser::Pids().size(); }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  std::ifstream stream(kProcDirectory + kStatFilename);
  string line;
  int running_processes = -1;
  if (stream.is_open()) {
    string name;
    while (true) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> name;
      if (name == "procs_running") {
        linestream >> running_processes;
        break;
      }
    }
  }
  return running_processes;
}

// DONE: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) {
  std::string cmd;
  std::ifstream stream(kProcDirectory + to_string(pid) + kCmdlineFilename);
  if (stream.is_open()) {
    std::getline(stream, cmd);
  }
  return cmd;
}

// DONE: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) {

  // in this method I have fellow the guide line in the course but I do not 
  // think it return the most correct result it just return the VmSize 
  long ram;
  std::string key;
  std::string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (true) {
      std::getline(stream, line);
      std::istringstream linestream(line);
      linestream >> key >> ram;
      if (key == "VmSize:") {
        break;
      }
    }
  }
  return to_string(ram/1024);
}

// DONE: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) {
  std::string uid;
  std::string key;
  std::string line;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatusFilename);
  if (stream.is_open()) {
    while (true) {
      std::getline(stream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      linestream >> key >> uid;
      if (key == "Uid") {
        break;
      }
    }
  }
  return uid;
}

// DONE: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) {
  std::string user;
  std::string temp;
  std::string uid;
  std::string line;
  std::ifstream stream(kPasswordPath);
  if (stream.is_open()) {
    while (true) {
      std::getline(stream, line);
      std::replace(line.begin(), line.end(), ':', ' ');
      std::replace(line.begin(), line.end(), '.', ' ');
      std::istringstream linestream(line);
      linestream >> user >> temp >> uid;
      if (uid == Uid(pid)) {
        break;
      }
    }
  }
  return user;
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) {
  long uptime{0};
  std::string line;
  std::string temp;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> temp >> temp >> temp;
    for (int i = 0; i < 19; i++) linestream >> uptime;
  }
  return uptime;
}

float LinuxParser::Cpu(int pid) {
  long uptime{0}, utime{0}, stime{0}, cutime{0}, cstime{0}, starttime{0};
  float cpu_usage{0.0f};
  long Hertz = sysconf(_SC_CLK_TCK);

  uptime = LinuxParser::UpTime();

  std::string line;
  std::string temp;
  std::string id;
  std::ifstream stream(kProcDirectory + to_string(pid) + kStatFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> id >> temp >> temp;
    for (int i = 3; i < 30; i++) {
      if (i == 13)
        linestream >> utime;
      else if (i == 14)
        linestream >> stime;
      else if (i == 15)
        linestream >> cutime;
      else if (i == 16)
        linestream >> cstime;
      else if (i == 21)
        linestream >> starttime;
      else
        linestream >> temp;
    }

    long total_time = utime + stime;
    total_time = total_time + cutime + cstime;
    float seconds = uptime - (starttime / (float)Hertz);
    cpu_usage = 100 * ((total_time / (float)Hertz) / seconds);
  }
  return cpu_usage;
}


//Jiffies are not used

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { return 0; }

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { return 0; }

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { return 0; }