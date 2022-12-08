#ifndef FORMAT_H
#define FORMAT_H

#include <string>

namespace Format {
std::string ElapsedTime(long times);  // See src/format.cpp
std::string LeadingZeros(const int value, const unsigned int n_zero);
};                                    // namespace Format

#endif