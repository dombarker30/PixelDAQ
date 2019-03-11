//##########################################################################
//## Title:       PixelReadout.hh                                         ##
//## Author:      Dominic Barker                                          ##
//## Date:        08/03/19                                                ##
//## Description: Class for the setting up the ardiuno. This includes     ##
//##              functions for including the reading of the config file, ##
//##              and sending the commands to the arduino. A bit number   ##
//##              is sent to the ASIC i.e 0001110101101 for each channel. ##
//##              The code loops over the channel and sets the same thing.##
//##              This code sends a code i.e. G SLK 1 which assigns the   ##
//##              bit asssociated to the SLK to 1 using binary | command  ##
//##              i.e. you take the SLK bit 10000000000 | 0001110101101   ##
//##              = 1001110101101 (i.e. the bit is replaced using a list  ##
//##              using a couple of bitwise operators                     ##     
//##########################################################################          
       
#ifndef ArduinoSetup_hh
#define ArduinoSetup_hh

//C++ Includes 
#include <string>
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>                        
#include <iomanip>
#include <unistd.h>

typedef int ArduinoSetupErrorCode;

namespace DAQ {
  class ArduinoSetup;
}


class DAQ::ArduinoSetup {

public:
  ArduinoSetup();

private:
  int PrintConfig();
  int ReadConfig();
  int SendConfig();
  int SendCommand(std::string& Command);

  //Commands sent to arudino 
  struct ArduinoCommands{
  public:

    std::string G_SLK  = "G SLK ";   //Leakage Control
    std::string G_STB1 = "G STB1 ";  //Mointor selector, 1=bandgap reference, 0=temperature  
    std::string G_STB  = "G STB ";   //1=Monitor ON on Ch0 
    std::string G_S16  = "G S16 ";   //1= high filter on ch16 enabled
    std::string C_SBF  = "C SBF ";   //Output buffer 0=down and bypassed, 1=active
    std::string C_SDC  = "C SDC ";   //Output coupling, 0=DC, 1=AC
    std::string C_ST   = "C ST ";   //Shaping time bit 1 00=1us 10=0.5us, 01 = 3us, 11=2u s
    std::string C_SG   = "C SG ";   //Gain bit 1 00=4.7mC/fC 10=7.8mV/fC, 01=14mV/fC,11=25mV/fc
    std::string C_SNC  = "C SNC ";   //Baseline 0=900mv 1=200mv 
    std::string C_STS  = "C STS ";   //Test capacitance enable 
    std::string TPD    = "G TPD ";   //Test Pulse duration
    std::string TPP    = "G TPP ";   //Test Pulse period
  };

  //Bit values to send to the arduino. Set to standard run params
  struct ArduinoConfig{
  public:

    int LeakageControl  = 0;
    int MonitorSelector = 0;
    int MonitorOn       = 0;
    int FilterOn        = 0;
    int OutputBufferOn  = 0;
    int OutputCoupling  = 0; 
    int ShapingTimeBit1 = 1; 
    int ShapingTimeBit0 = 1;
    int GainBit1        = 1;
    int GainBit0        = 0;
    int Baseline        = 1;
    int TestCapacitanceEnabled = 0;
    int TestPulseDuration      = 0;
    int TestPulsePeriod        = 0;
  };
  
  ArduinoConfig ArduinoConfig;
  ArduinoCommands ArduinoCommands;

public:
  ArduinoSetupErrorCode err;
};


#endif //PixelReadout_hh 
