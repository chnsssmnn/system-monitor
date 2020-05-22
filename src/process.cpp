#include <unistd.h>
#include <cctype>
#include <sstream>
#include <string>
#include <vector>
#include <iostream>

#include "process.h"
#include "linux_parser.h"

using std::string;
using std::to_string;
using std::vector;

Process::Process(int pid){
    pid_ = pid;
    user_ = LinuxParser::User(pid_);
    command_ = LinuxParser::Command(pid_);
    ram_ = LinuxParser::Ram(pid_);
    uptime_ = LinuxParser::UpTime(pid_);
}

// TODO: Return this process's ID
int Process::Pid() { return pid_; }

// TODO: Return this process's CPU utilization
float Process::CpuUtilization() const{ 
    string line, value;
    // read utime, stime, cutime, cstime from /proc/[pid_]/stat in the indexes 15,16,17,18
    int utime, stime, cutime, cstime;
    float total_time, start_time, seconds, processCpuUtil;

    std::ifstream filestream(LinuxParser::kProcDirectory + std::to_string(pid_) + LinuxParser::kStatFilename);

    if(filestream.is_open()){
        std::getline(filestream, line);
        std::istringstream linestream(line);
        std::istream_iterator<string> beg(linestream), end;
        vector<string> vec(beg, end);
        utime = std::stoi(vec[13]);
        stime = std::stoi(vec[14]);
        cutime = std::stoi(vec[15]);
        cstime = std::stoi(vec[16]);
        start_time = std::stof(vec[21]);

        //std::cout << "utime: " << utime << "\n";
        //std::cout << "stime: " << stime << "\n";
        //std::cout << "cutime: " << cutime << "\n";
        //std::cout << "cstime: " << cstime << "\n";
    }

    total_time = (float)(utime + stime + cutime + cstime);

    // total elapsed time in seconds since the process started
    seconds = LinuxParser::UpTime() - start_time/sysconf(_SC_CLK_TCK);

    processCpuUtil = (total_time/sysconf(_SC_CLK_TCK))/seconds;

    //std::cout << "processCpuUtil: " << processCpuUtil << "\n";

    return processCpuUtil; 
}

// TODO: Return the command that generated this process
string Process::Command() { return command_; }

// TODO: Return this process's memory utilization
string Process::Ram() { return ram_; }

// TODO: Return the user (name) that generated this process
string Process::User() { return user_; }

// TODO: Return the age of this process (in seconds)
long int Process::UpTime() { return uptime_; }

// TODO: Overload the "less than" comparison operator for Process objects
// REMOVE: [[maybe_unused]] once you define the function
bool Process::operator<(Process const& a[[maybe_unused]]) const { 
    return (a.CpuUtilization() < this->CpuUtilization()); 
}