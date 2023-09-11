#include <dirent.h>
#include <unistd.h>
#include <sstream>
#include <string>
#include <vector>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// DONE: An example of how to read data from the filesystem
string LinuxParser::OperatingSystem() {
  string line;
  string key;
  string value;
  std::ifstream filestream(kOSPath);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ' ', '_');
      std::replace(line.begin(), line.end(), '=', ' ');
      std::replace(line.begin(), line.end(), '"', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "PRETTY_NAME") {
          std::replace(value.begin(), value.end(), '_', ' ');
          return value;
        }
      }
    }
  }
  return value;
}

// DONE: An example of how to read data from the filesystem
string LinuxParser::Kernel() {
  string os, kernel, version;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel;
  }
  return kernel;
}

// BONUS: Update this to use std::filesystem
vector<int> LinuxParser::Pids() {
  vector<int> pids;
  DIR* directory = opendir(kProcDirectory.c_str());
  struct dirent* file;
  while ((file = readdir(directory)) != nullptr) {
    // Is this a directory?
    if (file->d_type == DT_DIR) {
      // Is every character of the name a digit?
      string filename(file->d_name);
      if (std::all_of(filename.begin(), filename.end(), isdigit)) {
        int pid = stoi(filename);
        pids.push_back(pid);
      }
    }
  }
  closedir(directory);
  return pids;
}

// DONE: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line, key;
  int value, t_value, a_value;
  float u_value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") { t_value = value; }
        if (key == "MemAvailable:") { a_value = value; }
        u_value = float(t_value - a_value) /float( t_value); 
      }   
    }
  }
     
  return u_value; }

// DONE: Read and return the system uptime
long LinuxParser::UpTime() { 
  long UpTime;
  string line, uptime;
  std::ifstream stream(kProcDirectory + kUptimeFilename) ;
  if (stream.is_open()) {
    std::getline (stream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    UpTime = std::stol(uptime);
  }
  return UpTime; }

// DONE: Read and return the number of jiffies for the system
// sum of active and idle jiffies
long LinuxParser::Jiffies() { 
  long jiffy;
  jiffy = ActiveJiffies() + IdleJiffies(); 
return jiffy; }
 

// DONE: Read and return the number of active jiffies for a PID
long LinuxParser::ActiveJiffies(int pid) { 
  string line, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatFilename);
  long active = 0;
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    int count =0;
    while(linestream >> value){
      if((count >=13) && (count  <17)){
   		active += std::stol(value) ;
     	}
      count++;
    }
  }
  return active / sysconf(_SC_CLK_TCK);
}

// DONE: Read and return the number of active jiffies for the system
// utilize non idle variable from CPU utilization function
// extract active values from cpuutilization, convert from string to long, return their sum 
long LinuxParser::ActiveJiffies() { 
  long value;
  vector<string> cpu_list = CpuUtilization();
  string user = cpu_list[1];
  string niced = cpu_list[2];
  string system = cpu_list[3];
  string irq = cpu_list[6];
  string softirq = cpu_list[7];
  string steal = cpu_list[8];
  value = stol(user) + stol(niced) + stol(system) + stol(irq) + stol(softirq) + stol(steal);
  return value;}

// DONE: Read and return the number of idle jiffies for the system
// extract idle values from cpuutilization, convert from string to long, return their sum 
long LinuxParser::IdleJiffies() { 
  long value;
  vector<string> cpu_list = CpuUtilization();
  string idle = cpu_list[4];
  string iowait = cpu_list[5];
  value = stol(idle) + stol(iowait);
  return value;}

// DONE: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  string value, line;
  vector<string> cpu_list;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    while (linestream >> value) {
      cpu_list.push_back(value);
    }
  // cpu user nice system idle iowait irq softirq steal guest guestnice
  // idle = idle + iowait
  // non idle = user + nice + system + irq + softirq + steal
  // total = idle + non idle
  //cpu percentage = (total -idle)/total
  } 
 return cpu_list; }

// DONE: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "processes") {
        return value;
      }
    }
  }  
  return value; }

// DONE: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key;
  int value;
  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while(getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key >> value;
      if (key == "procs_running") {
        return value;
      }
    }
  }  
  return value; }
  
// DONE: Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  string line;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    return line; 
  }
  return line; }

// DONE: Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line, key, value;
  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if (filestream.is_open()) {
    while(std::getline (filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize:") {
          return value; 
        }
      }  
    }
  }  
  return value; }

// DONE: Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
    string line, key, value;
    std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
    if (filestream.is_open()) {
      while(std::getline (filestream, line)) {
        std::istringstream linestream(line);
        while (linestream >> key >> value) {
          if (key == "Uid:") {
            return value; 
          }  
       }
    }
  }    
  return value; }
      
// DONE: Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line, key, x, value;
  string user;
  string uid = Uid(pid);
  std::ifstream filestream(kPasswordPath);
  if (filestream.is_open()) {
    while (std::getline (filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> x >> value) {
        if (value == uid) {
          user = key;
        }
      }
    }
  }  
  return user; }

// DONE: Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
    long UpTime = 1;
    string line, uptime;
    std::ifstream stream(kProcDirectory + std::to_string(pid) + kStatFilename) ;
    if (stream.is_open()) {
      std::getline (stream, line);
      std::istringstream linestream(line);
      for (int i = 0; i < 22; i++) {
	    linestream >> uptime;
      }
       UpTime = std::stol(uptime) / sysconf(_SC_CLK_TCK);
    }
  return  LinuxParser::UpTime() - UpTime; 
}
