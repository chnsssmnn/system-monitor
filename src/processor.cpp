#include "processor.h"
#include "linux_parser.h"
#include <iostream>


// TODO: Return the aggregate CPU utilization
float Processor::Utilization() { 
    float util;
    long active = LinuxParser::ActiveJiffies();
    long idle = LinuxParser::IdleJiffies();
    long activeTime = active - active_;
    long idleTime = idle - idle_;
    long duration = activeTime + idleTime;

    util = static_cast<float>(activeTime) / duration;

    active_ = active;
    idle_ = idle;

    //std::cout << "CPU UTIL: " << util <<"\n";
    return util; 
}