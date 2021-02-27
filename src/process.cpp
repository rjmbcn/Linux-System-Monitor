#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <cmath>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

// contructor
Process::Process(int pid) {
    pid_ = pid;
    cpu_load_ = LinuxParser::PID_CpuUtilization(pid_);

    std::vector<long> stats = LinuxParser::PIDCpuStats(pid_);
    total_time_in_seconds_ = stats[0];
    seconds_ = stats[1];
}

// Return this process's ID
int Process::Pid() { 
    return pid_; }

// Return this process's CPU utilization
float Process::CpuUtilization() { 
    // cpu_load_ = LinuxParser::PID_CpuUtilization(pid_); 
  std::vector<long> stats = LinuxParser::PIDCpuStats(pid_);
  long total_time_in_seconds = stats[0];
  long seconds = stats[1];
  
  // calculate deltas 
//   long delta_total_time =  total_time_in_seconds - total_time_in_seconds_;
//   long delta_seconds = seconds - seconds_;

//   total_time_in_seconds_ = total_time_in_seconds;
//   seconds_ = seconds;
   
  cpu_load_ = (total_time_in_seconds/(1.00*seconds));

  return  cpu_load_;}

// Return the command that generated this process
string Process::Command() { return LinuxParser::Command(pid_); }

// Return this process's memory utilization
string Process::Ram() { 
    
    int ram_i =std::stoi(LinuxParser::Ram(pid_))/1024;
    string ram_s = to_string( (ram_i) );
    return ram_s; }

// Return the user (name) that generated this process
string Process::User() { return LinuxParser::User(pid_); }

// Return the age of this process (in seconds)
long int Process::UpTime() { return LinuxParser::UpTime(pid_); }

// Overload the "less than" comparison operator for Process objects
// This function is used in sorting processes based on CPU utilization 
bool Process::operator<(Process const& a) const { 
    if(cpu_load_> a.cpu_load_)
        return true;
    else 
    return false; }