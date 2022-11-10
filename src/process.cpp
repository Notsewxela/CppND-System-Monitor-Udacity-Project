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
    Process::pid_ = pid;
    Process::command_ = LinuxParser::Command(pid);

}

// DONE: Return this process's ID
int Process::Pid() const { return pid_; }

// DONE: Return this process's CPU utilization
float Process::CpuUtilization() const { 
    try {
        return float(LinuxParser::ActiveJiffies(pid_)) / float(LinuxParser::UpTime() - LinuxParser::UpTime(pid_));
    } catch (...) {
        return 0.0;
    }
}

// DONE: Return the command that generated this process

string Process::Command() const { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() const { return string(); }

// TODO: Return the user (name) that generated this process
string Process::User() const { return string(); }

// DONE: Return the age of this process (in seconds)
long int Process::UpTime() const { return LinuxParser::UpTime(pid_); }

// DONE: Overload the "less than" comparison operator for Process objects
bool Process::operator<(Process const& a) const { 
    return CpuUtilization() < a.CpuUtilization();
}