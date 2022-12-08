#ifndef PROCESSOR_H
#define PROCESSOR_H

#include "linux_parser.h"

class Processor {
 public:
  float Utilization();  // See src/processor.cpp

  // Declare any necessary private members
 private:
    long active_jiffies_;
    long idle_jiffies_;
    long total_jiffies_;

    long prev_idle_;
    long prev_active_;
    long prev_total_;

    long total_difference_;
    long idle_difference;
};

#endif