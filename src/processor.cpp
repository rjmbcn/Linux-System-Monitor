#include<vector>
#include<string>

#include "linux_parser.h"
#include "processor.h"

using namespace std;

Processor::Processor(){

  vector<string> utilizations = LinuxParser::CpuUtilization();
  long user = stol(utilizations[0]);
  long nice = stol(utilizations[1]);
  long system = stol(utilizations[2]);
  long idle = stol(utilizations[3]);
  long iowait = stol(utilizations[4]);
  long irq = stol(utilizations[5]);
  long softirq = stol(utilizations[6]);
  long steal = stol(utilizations[7]);

  long NonIdle = user + nice + system + irq + softirq + steal;
   
  Idle_ = idle + iowait;
  Total_ = Idle_ + NonIdle;


}

// Return the aggregate CPU utilization
float Processor::Utilization() { 
  vector<string> utilizations = LinuxParser::CpuUtilization();
  long user = stol(utilizations[0]);
  long nice = stol(utilizations[1]);
  long system = stol(utilizations[2]);
  long idle = stol(utilizations[3]);
  long iowait = stol(utilizations[4]);
  long irq = stol(utilizations[5]);
  long softirq = stol(utilizations[6]);
  long steal = stol(utilizations[7]);

  long Idle = idle + iowait;
  long NonIdle = user + nice + system + irq + softirq + steal;
  
  long PrevTotal = Total_;
  long PrevIdle =  Idle_;

  long Total = Idle + NonIdle;
  
  Total_ = Total;
  Idle_ = Idle;
  // calculate deltas

  float total_delta = Total - PrevTotal;
  float idle_delta  = Idle - PrevIdle;

  return (total_delta - idle_delta) / total_delta;
 }