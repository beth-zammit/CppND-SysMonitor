#include <string>
#include <iostream>

#include "format.h"

using std::string;

// DONE: Complete this helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS

string Format::ElapsedTime(long seconds) {
  int hour = seconds/3600;
  seconds = seconds % 3600;
  int min = seconds/60;
  seconds = seconds % 60;

  string time;
  string second;
  string hours;
  string minute;
  
 if (hour < 10) {
   hours = "0" + std::to_string(hour); }
 else { hours = std::to_string(hour); }  
 if (seconds < 10) {
   second = "0" + std::to_string(seconds); }
 else {second = std::to_string(seconds); }
 if (min < 10) {
   minute = "0" + std::to_string(min); }
 else { minute = std::to_string(min); } 
 time = hours + ":" + minute + ":" + second; 
 return time;
}