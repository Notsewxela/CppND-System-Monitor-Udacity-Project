#include <string>
#include <algorithm> // For std:min
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
    // The conversion from size_t to int throws a warning in the compiler about overflowing.
    // Given the rediculous amount of time a program would have to be running to cause this, I am not concerned and can ignore the warning.
    // +2147483647 hours is approx 245000 years.
    // seconds and minutes will never be longer than two digits.
    unsigned int old_length = int{(std::to_string(value)).length()};
    return std::string(n_zero - std::min(n_zero, old_length), '0') + std::to_string(value);
}