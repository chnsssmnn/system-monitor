#include <dirent.h>
#include <unistd.h>
#include <string>
#include <vector>
#include <iostream>

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
  string os, version, kernel;
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

// TODO: Read and return the system memory utilization
float LinuxParser::MemoryUtilization() { 
  float memTotal, memFree, buffers;

  string line;
  string key;
  float value;
  std::ifstream filestream(kProcDirectory + kMeminfoFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "MemTotal:") {
          memTotal = value;
          //std::cout << "MemTotal: " << memTotal << "\n";
        }
        else if (key == "MemFree:"){
          memFree = value;
          //std::cout << "MemFree: "<<  memFree << "\n";
        }
        else if (key == "Buffers:"){
          buffers = value;
          //std::cout << "Buffers: "<<  buffers << "\n";
        }
      }
    }
  }
  //std::cout << "MemUtil: " << (memTotal-memFree)/(memTotal-buffers) << "\n";
  return (memTotal-memFree)/(memTotal-buffers); 
}

// TODO: Read and return the system uptime
long LinuxParser::UpTime() { 
  string line;
  long uptime;

  std::ifstream filestream(kProcDirectory + kUptimeFilename);
  if (filestream.is_open()) {
    std::getline(filestream, line);
    std::istringstream linestream(line);
    linestream >> uptime;
    //std::cout << "System uptime: "<<  uptime << "\n";
  }
  return uptime; 
}

// TODO: Read and return the number of jiffies for the system
long LinuxParser::Jiffies() { 
  // jiffies = avtive + idle
  //std::cout << "JIFFIES: " << ActiveJiffies() + IdleJiffies() << "\n";
  return ActiveJiffies() + IdleJiffies(); 

}

// TODO: Read and return the number of active jiffies for a PID
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::ActiveJiffies(int pid[[maybe_unused]]) { return 0; }

// TODO: Read and return the number of active jiffies for the system
long LinuxParser::ActiveJiffies() { 
  long activeJiffies;
  vector<CPUStates> activeStates = {
    kUser_,
    kNice_,
    kSystem_,
    kIRQ_,
    kSoftIRQ_,
    kSteal_
  };
  
  vector<string> cpuUtil = CpuUtilization();

  for(auto state : activeStates){
    activeJiffies += stol(cpuUtil[state]);
  }

  //std::cout << "active total: " << activeJiffies << "\n";
  return activeJiffies; 
}

// TODO: Read and return the number of idle jiffies for the system
long LinuxParser::IdleJiffies() { 
  long idleJiffies;
  vector<CPUStates> idleStates = {kIdle_, kIOwait_};
  vector<string> cpuUtil = CpuUtilization();

  for(auto state : idleStates){
    idleJiffies += stol(cpuUtil[state]);
  }

  //std::cout << "idle total: " << idleJiffies << "\n";
  return idleJiffies; 
}

// TODO: Read and return CPU utilization
vector<string> LinuxParser::CpuUtilization() { 
  vector<string> cpuUtil;
  string line, key, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if(filestream.is_open()){
   while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      linestream >> key;
      if ( key == "cpu"){
        while(linestream >> value){
          //std::cout << "CPU UTIL: " << value << "\n";
          cpuUtil.push_back(value);
        }
      }
   }
  }
  return cpuUtil; 
}

// TODO: Read and return the total number of processes
int LinuxParser::TotalProcesses() { 
  string line, key;
  int procs, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "processes") {
          procs = value;
          //std::cout << "Processes: "<<  procs << "\n";
        }
      }
    }
  }
  return procs; 
}

// TODO: Read and return the number of running processes
int LinuxParser::RunningProcesses() { 
  string line, key;
  int procsRunning, value;

  std::ifstream filestream(kProcDirectory + kStatFilename);
  if (filestream.is_open()) {
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value) {
        if (key == "procs_running") {
          procsRunning = value;
          //std::cout << "Procs_Running: "<<  procsRunning << "\n";
        }
      }
    }
  }
  return procsRunning; 
}

// TODO: Read and return the command associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Command(int pid) { 
  string line;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kCmdlineFilename);
  if(filestream.is_open())
    std::getline(filestream, line);
  //std::cout << "Command: " << line << "\n";
  return line; 
}

// TODO: Read and return the memory used by a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Ram(int pid) { 
  string line, key, ram;
  int value;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "VmSize:")
          ram = to_string(value/1000);
      }
    }
  }
  //std::cout << "Ram: " << ram << "\n";
  return ram; 
}

// TODO: Read and return the user ID associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::Uid(int pid) { 
  string line, key, value, uid;

  std::ifstream filestream(kProcDirectory + std::to_string(pid) + kStatusFilename);
  if(filestream.is_open()){
    while (std::getline(filestream, line)) {
      std::istringstream linestream(line);
      while (linestream >> key >> value){
        if (key == "Uid:")
          uid = value;
      }
    }
  }
  //std::cout << "Uid: " << uid << "\n";
  return uid; 
}

// TODO: Read and return the user associated with a process
// REMOVE: [[maybe_unused]] once you define the function
string LinuxParser::User(int pid) { 
  string user = "";
  string uid = Uid(pid);
  string line;
  string first, second, third;

  std::ifstream filestream(kPasswordPath);

  if(filestream.is_open()){
    while(std::getline(filestream, line)){
      std::replace(line.begin(), line.end(), ':', ' ');
      std::istringstream linestream(line);
      while(linestream >> first >> second >> third){
        if(third == uid){
          user = first;
          break;
        }
      }
    }
  }
  
  return user; 
}

// TODO: Read and return the uptime of a process
// REMOVE: [[maybe_unused]] once you define the function
long LinuxParser::UpTime(int pid) { 
  string line, value;
  long uptime;

  std::ifstream filestream(kProcDirectory + to_string(pid) + kStatFilename);
  if(filestream.is_open()){
    std::getline(filestream, line);
    std::istringstream linestream(line);

    // time that the process started after system boot is the 22nd value
    for (int i = 0; i < 22; ++i){
      linestream >> value;
    }
    //std::cout << value << "\n";
  }
  uptime = UpTime() - (std::stol(value)/sysconf(_SC_CLK_TCK)); 
  //std::cout << "UPTIME:" << uptime << "\n";

  //the value is expressed in clock ticks (divide by sysconf(_SC_CLK_TCK))
  return uptime; 
}