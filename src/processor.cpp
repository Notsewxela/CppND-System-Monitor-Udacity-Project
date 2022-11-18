#include "processor.h"

// DONE: Return the aggregate CPU utilization
// https://stackoverflow.com/questions/23367857/accurate-calculation-of-cpu-usage-given-in-percentage-in-linux
float Processor::Utilization() { 
    prev_idle_ = idle_jiffies_;
    prev_active_ = active_jiffies_;
    prev_total_ = prev_idle_ + prev_active_;

    idle_jiffies_ = LinuxParser::IdleJiffies();
    active_jiffies_ = LinuxParser::ActiveJiffies();
    total_jiffies_ = idle_jiffies_ + active_jiffies_;

    idle_difference = idle_jiffies_ - prev_idle_;
    total_difference_ = total_jiffies_ - prev_total_;

    return float(total_difference_ - idle_difference) / float(total_difference_);
}