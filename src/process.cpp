#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <stdexcept>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(const int pid) {
    pid_ = pid;
    uptime_ = LinuxParser::UpTime(pid);
    try {
        active_jiffies_ = LinuxParser::ActiveJiffies(pid_);
        system_uptime_ = LinuxParser::ActiveJiffies();
        cpu_ = float(active_jiffies_) / float(system_uptime_ - uptime_);
    } catch (...) { // Divide by 0 or something
        cpu_ = 0.0;;
    }
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid_);
    user_ = LinuxParser::User(pid_);
    
}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; }

// DONE: Return the command that generated this process
string Process::Command() const { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() const { return ram_; }

// DONE: Return the user (name) that generated this process
string Process::User() const { return user_; }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization();
}