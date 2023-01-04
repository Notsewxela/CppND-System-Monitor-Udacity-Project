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

    // To get the utilisation: https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
    uptime_ = LinuxParser::UpTime(pid); // starttime / hz
    active_jiffies_ = LinuxParser::ActiveJiffies(pid_); // total_time / hz
    system_uptime_ = LinuxParser::UpTime(); // uptime

    try {
        cpu_ = float(active_jiffies_) / float(system_uptime_ - uptime_);
    } catch (...) { // Divide by 0
        cpu_ = 0.0;;
    }
    command_ = LinuxParser::Command(pid);
    ram_ = LinuxParser::Ram(pid_);
    user_ = LinuxParser::User(pid_);
}

// Return this process's ID
int Process::Pid() const { return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() const { return cpu_; }

// Return the command that generated this process
string Process::Command() const { return command_; }

// Return this process's memory utilization
string Process::Ram() const { return ram_; }

// Return the user (name) that generated this process
string Process::User() const { return user_; }

// Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime() - LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization();

    // Alternative method if one wanted
    // return std::stoi(Ram()) < std::stoi(a.Ram());
}