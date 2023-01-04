#include <string>
#include <algorithm>    // For std:min
#include <cmath>        // For calculating length of integer
#include "format.h"

// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS
std::string Format::ElapsedTime(long seconds) {

    long hours = seconds / 3600; // Integer division
    seconds %= 3600;
    int minutes = seconds / 60;
    seconds %= 60;
    int n_zero{2};
    std::string seconds_string = LeadingZeros(seconds, n_zero);
    std::string minutes_string = LeadingZeros(minutes, n_zero);
    std::string hours_string = LeadingZeros(hours, n_zero);
    return std::string(hours_string + ":" + minutes_string + ":" + seconds_string);
}

// This function pads the an integer with zeros and returns this as a string.
// Returns a string at least n_zero in length but potentially longer
std::string Format::LeadingZeros(const int value, const unsigned int n_zero)
{
    // Get length of base 10 representation of value
    unsigned int old_length = value == 0 ? 1 : trunc(log10(value)) + 1;
    // Pad (if necessary) with leading zeros
    return std::string(n_zero - std::min(n_zero, old_length), '0') + std::to_string(value);
}