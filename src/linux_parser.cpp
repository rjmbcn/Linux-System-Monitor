#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>
#include <unistd.h>

#include "linux_parser.h"

using std::stof;
using std::string;
using std::to_string;
using std::vector;

// SYSTEM BEGIN

// Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  string line;
  string key;
  string value;
  float MemTotal, MemFree;
  std::ifstream filestream("/proc/meminfo");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal") {
          MemTotal =  std::stof(value);
        }
        if (key == "MemFree") {
          MemFree =  std::stof(value);
        }
      }
    }
  }
  
  return (MemTotal - MemFree)/MemTotal; 
}

// Read and return the system uptime
long LinuxParser::UpTime() { 
  // 
  // Note that in multicore systems the iddle time may appear larger than the uptime
  // see this: "The first number is the total number of seconds the system 
  // has been up.  The second number 
  // is how much of that time the machine has spent idle,  in seconds".
  //  https://access.redhat.com/solutions/62383

  string line;
  string uptime;
  string iddletime;
  
  std::ifstream stream("/proc/uptime");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> uptime >> iddletime ;
  }
  return std::stol(uptime);
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

// Read and return the total number of processes
int LinuxParser::TotalProcesses() {
  vector<int> pids = Pids();
   return pids.size(); }

//  Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line;
  string key;
  string value;
  int proc_running =0;
  
  std::ifstream filestream("/proc/stat");
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      // std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          proc_running =  std::stoi(value);
        }
      }
    }
  }
  
  return proc_running; }

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
  string os, version, kernel, test;
  string line;
  std::ifstream stream(kProcDirectory + kVersionFilename);
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> os >> version >> kernel ;
  }
  return kernel;
}
// SYSTEM ENDS

// PROCESS BEGINS

// Read and return the command associated with a process
string LinuxParser::Command(int pid) { 
  
  string line;
  string cmd_line;
  
  std::ifstream stream("/proc/" + std::to_string(pid) + "/cmdline");
  if (stream.is_open()) {
    std::getline(stream, line);
    std::istringstream linestream(line);
    linestream >> cmd_line;
  }

  return cmd_line; }

// Read and return the memory used by a process
string LinuxParser::Ram(int pid) { 
  string line;
  string key;
  string value;
  string ram;
  
  std::ifstream filestream("/proc/" + std::to_string(pid) + "/status");
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "VmSize") {
          ram = value;
        }
      }
    }
  }
  return ram;
}
// Read and return the user ID associated with a process
string LinuxParser::Uid(int pid) { 
  string line;
  string key;
  string value;
  string uid;
  
  std::ifstream filestream("/proc/" + std::to_string(pid) + "/status");
  
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        // std::cout << key << value << std::endl;
        if (key == "Uid") {
           uid = value;
        }
      }
    }
  }
  return uid;
}

// Read and return the user associated with a process
string LinuxParser::User(int pid) { 
  string line;
  string uid = LinuxParser::Uid(pid);

    string searchUID = ":x:" + uid;
    string username;
    std::ifstream filestream(kPasswordPath);
    if (filestream.is_open()) {
      while (std::getline(filestream, line)) {
        auto pos = line.find(searchUID);
        username = line.substr(0, pos );
        return username;
      }
    }
  return "Error: could not open file!";
}
// Read and return the uptime of a process
long LinuxParser::UpTime(int pid) { 
  string line, value;
  long pid_on_time, sys_uptime;
  long pid_uptime =0;
  std::ifstream file("/proc/" + to_string(pid) + "/stat");
  
  if (file.is_open()){
    while( getline(file,line) ){
      std::istringstream linestream(line);
        for (int i =1; i<23; i++){
          linestream >> value;
        }
    }
    file.close();
    pid_on_time = std::stol(value)/sysconf(_SC_CLK_TCK);
    sys_uptime = UpTime();
    pid_uptime = sys_uptime - pid_on_time; 
    return pid_uptime;
  }
  return pid_uptime;
}

// Read the stat file of process pid and return the value at position
std::string LinuxParser::pid_stat(int pid, int val_at){
  string line, value;
  std::ifstream file("/proc/" + to_string(pid) + "/stat");
  if (file.is_open()){
    while( getline(file,line) ){
      std::istringstream linestream(line);
        for (int i =1; i<val_at+1; i++){
          linestream >> value;
        }
    }
    file.close();
    return value;
  }
  return value;
}
                                
std::vector<long> LinuxParser::PIDCpuStats(int pid){
  std::vector<long> stats = {};

  long utime  = std::stol( pid_stat(pid, 14) );
  long stime  = std::stol( pid_stat(pid, 15) );
  long cutime = std::stol( pid_stat(pid, 16) );
  long cstime = std::stol( pid_stat(pid, 17) );
  long starttime = std::stol( pid_stat(pid, 22) );

  long total_time, seconds,total_time_in_seconds;

  total_time = utime + stime;
  total_time += cutime + cstime;
  total_time_in_seconds = total_time/sysconf(_SC_CLK_TCK);
  
  stats.push_back(total_time_in_seconds);

  seconds = UpTime() - starttime/sysconf(_SC_CLK_TCK);
  stats.push_back(seconds);

  return stats;

}

float LinuxParser::PID_CpuUtilization(int pid){
  // see https://stackoverflow.com/questions/16726779/how-do-i-get-the-total-cpu-usage-of-an-application-from-proc-pid-stat/16736599#16736599
  
  // long utime  = std::stol( pid_stat(pid, 14) );
  // long stime  = std::stol( pid_stat(pid, 15) );
  // long cutime = std::stol( pid_stat(pid, 16) );
  // long cstime = std::stol( pid_stat(pid, 17) );
  // long starttime = std::stol( pid_stat(pid, 22) );

  // long total_time, seconds,total_time_in_seconds;

  // total_time = utime + stime;
  // total_time += cutime + cstime;
  // total_time_in_seconds = total_time/sysconf(_SC_CLK_TCK);

  // seconds = UpTime() - starttime/sysconf(_SC_CLK_TCK);

  std::vector<long> stats = PIDCpuStats(pid);
  long total_time_in_seconds = stats[0];
  long seconds = stats[1];

  float cpu_usage; 
  
  cpu_usage = (total_time_in_seconds/(1.00*seconds));

  return cpu_usage;

}

// PROCESSOR CPU BEGINS 

// Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 

  string line, cpu, cpu_time;
  vector<string> cpu_allocations;
  std::ifstream filestream("/proc/stat");

  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> cpu;

    while (linestream >> cpu_time) {
      cpu_allocations.push_back(cpu_time);
    }
  }
  return cpu_allocations;
}
// // TODO: Read and return the number of jiffies for the system
// long LinuxParser::Jiffies() { return 0; }

// // TODO: Read and return the number of active jiffies for a PID
// // REMOVE: [[maybe_unused]] once you define the function
// long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// // TODO: Read and return the number of active jiffies for the system
// long LinuxParser::ActiveJiffies() { return 0; }

// // TODO: Read and return the number of idle jiffies for the system
// long LinuxParser::IdleJiffies() { return 0; }

// PROCESSOR CPU ENDS 
