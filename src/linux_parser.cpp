#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>
#include <iostream>

#include "linux_parser.h"
#include <iomanip>

using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line, key, value;
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
    linestream >> os >> version >> kernel;
  }
  return kernel;
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
  
  // These are our string stream extraction variables
  string key, units;
  long value;

  long mem_total, mem_free;

  // Allows breaking out of the loop early
  bool mem_total_found{false}, mem_free_found{false};
  
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);

  if (filestream.is_open()) {
    string line;
    while (std::getline(filestream, line) and !(mem_free_found and mem_total_found)) {
      std::istringstream linestream{line};
      linestream >> key >> value >> units;
      if (key == "MemTotal:") {
        mem_total = value;
        mem_total_found = true;
      } else if (key == "MemFree:") {
        mem_free = value;
        mem_free_found = true;
      }
    }
  }

  if (mem_total_found and mem_free_found) {
    // Cast to a float as late as possible to ensure accuracy is not lost as much as possible.
    return (float) (mem_total - mem_free) / (mem_total*1.0);
  /* Called in case it didn't find any info, either because the file does not exist or something missing in file
     Alternative here is just to throw an error, but that has benefits and drawbacks compared with what I have done.
     What I have here is a program that won't crash but will produce erroneous results instead
     If it was st to crash, it fails whole program gets stuck and crashes, but easier to identify erroneous results
     Unsure which is better in this context.... */
  } else {
    return 0;
  }
}

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  long uptime{0};
  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    std::istringstream linestream{line};
    linestream >> uptime;
  }
  return uptime;
}

// DONE: Read and return the number of jiffies for the system since the last boot
long LinuxParser::Jiffies() {
  // Wonder if there is easy way around two calls to the CpuUtilization function?
  return ActiveJiffies() + IdleJiffies();
}

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) {
  long utime, stime, cutime, cstime;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    string line;
    // Set the delimeter to a space
    // Need to use the '' characters so that C++ interprets this as a character and not a string with the extra character on the end
    // https://stackoverflow.com/questions/39066998/what-are-the-meaning-of-values-at-proc-pid-stat
    // https://knowledge.udacity.com/questions/129844

    std::stringstream linestream{line};
    long useless_token;
    for (int i = 0; i < 13; i++) {
      linestream >> useless_token;

    }
    linestream >> utime >> stime >> cutime >> cstime;
    return (utime + stime + cutime + cstime) / sysconf(_SC_CLK_TCK);
  }
  // Return 0 if it all goes wrong :(
  return 0;
}

// DONE: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  vector<string> jiffies = CpuUtilization();
  using std::stol;
  return stol(jiffies[CPUStates::kUser_]) + stol(jiffies[CPUStates::kGuest_]) +
  stol(jiffies[CPUStates::kGuestNice_]) + stol(jiffies[CPUStates::kIRQ_]) + 
  stol(jiffies[CPUStates::kNice_]) + stol(jiffies[CPUStates::kSoftIRQ_]) +
  stol(jiffies[CPUStates::kSteal_]) + stol(jiffies[CPUStates::kSystem_]);
}

// DONE: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  vector<string> jiffies = CpuUtilization();
  using std::stol;
  return stol(jiffies[CPUStates::kIdle_]) + stol(jiffies[CPUStates::kIOwait_]);

}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() {
  vector<string> jiffies;
  bool found{false};
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    string line, cpu, jiffy;
    // Doing the loop allows for accomodation to changes of the structure of /proc/stat
    
    while (std::getline(filestream, line) and !found) {
      std::istringstream linestream{line};
      linestream >> cpu;
      if (cpu == "cpu") {
        while (linestream >> jiffy) {
          jiffies.emplace_back(jiffy);
        }
        found = true;
      }
    }
  }
  if (found) {
    return jiffies;
  } else { // Something went wrong :(
    return std::vector<string> {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
  }
}

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  return Processes("processes");
}

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() {
  return Processes("procs_running");
}

// A function I added to deal with repeated code in the two above process functions
int LinuxParser::Processes(string key) {
  int processes;
  bool found{false};
  std::ifstream filestream(kProcDirectory + kStatFilename);

  if (filestream.is_open()) {
    string line, label;
    while (std::getline(filestream, line) and !found) {
      std::istringstream linestream{line};
      linestream >> label;
      if (label == key) {
        linestream >> processes;
        found = true;
      }
    }
  }
  if (found) {
    return processes;
  } else {
    return 0;
  }
}

// Done: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  
  // Don't need to istring stream here as we want the entire line's contents
  if (filestream.is_open()) {
    string line;
    std::getline(filestream, line);
    return line;
  }
  // Something went wrong :(
  return "Problem accessing " + kProcDirectory + std::to_string(pid) + kCmdlineFilename;
}

// Done: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    long ram;
    string line, label;
    while (std::getline(filestream, line)) {
      std::istringstream linestream{line};
      linestream >> label;
      if (label == "VmSize:") {
        linestream >> ram;
        // Convert from kB to MB
        return std::to_string(ram / 1000);

      }
    }
  }
  return "Problem accessing " + kProcDirectory + std::to_string(pid) + kStatusFilename;
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid[[maybe_unused]]) { return string(); }

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid[[maybe_unused]]) { return string(); }

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  if (filestream.is_open()) {
    long uptime;
    string line, useless_token;
    std::istringstream linestream{line};
    // Get the token in position 21 (starttime)
    for (int i = 0; i < 21; i++) {
      linestream >> useless_token;
    }
    linestream >> uptime;
    return uptime / sysconf(_SC_CLK_TCK);
  }
  return 0;
}
