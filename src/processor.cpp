#include "processor.h"
#include "linux_parser.h"

// DONE: Return the aggregate CPU utilization
float Processor::Utilization() { 
  float total = LinuxParser::Jiffies();
  float active = LinuxParser::ActiveJiffies();
  float result;
  if (total == 0) { result = 0.0; }
  else { result = 1.0 * (active/total); }
  return result;
 }
