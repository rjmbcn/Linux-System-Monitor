#include <string>

#include "format.h"

using std::string;

string format_epoch_unit(long int epoch){
    string unit;
    if (epoch < 10){ unit = "0" + std::to_string(epoch);}
    else{ unit = std::to_string(epoch);}
    return unit;
}

// helper function
// INPUT: Long int measuring seconds
// OUTPUT: HH:MM:SS

string Format::ElapsedTime(long seconds) { 
    long int hr; 
    long int min;
    long int sec;
    long int remainder;

    hr = seconds/3600;
    remainder = seconds % 3600;
    min = remainder/60; 
    sec = remainder % 60;
    
    string HH = format_epoch_unit(hr);
    string MM = format_epoch_unit(min);
    string SS = format_epoch_unit(sec);
    
    string elapsed_time = HH + ":" +MM + ":" + SS;
    return elapsed_time;
 }
