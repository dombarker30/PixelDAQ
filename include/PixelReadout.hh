//##########################################################################
//## Title:       PixelReadout.hh                                         ##
//## Author:      Dominic Barker                                          ##
//## Description: Class for the running the Pixel Readout. This includes  ##
//##              functions for including the reading of the config file, ##
//##              starting the board, setting the trigger levels,         ##
//##              starting aqiusition and online monitoring. In addition  ##
//##              the config for the ASICs is set read into the arduino   ##
//##              code. This code provides the structure to run the DAQ.  ##
//##              However DAQDriver.cc runs the DAQ                       ##
//## Updates:     28th Nov 2018 - Readout stream complete                 ##
//##########################################################################                    

#ifndef PixelReadout_hh
#define PixelReadout_hh

//CAEN Includes
#include "CAENDigitizer.h"

//C++ Inclues 
#include <stdio.h>
#include <vector>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
#include <iomanip>  
#include <cmath>  
#include <map>
#include <ctime>
#include <limits>

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

typedef int PixelReadoutErrorCode;

namespace DAQ {
  class PixelReadout;
}


class DAQ::PixelReadout {

public:

  PixelReadout();
  
  //Holder for the DAQ config
  struct DAQConfig{
  public:
    
    int MAXNB;              //Number of connected boards 
    int ReadoutSize;        //Size of the Waveform
    int ASIC_Gain;          //Gain of the pre-amps
    int ASIC_Shaping_Time;  //Shaping time of the pre-amps
    int MaxEvents;          //Max number of events in the readout size. If set to -1 this is ignorned  
    int MaxTime;            //Max time that the DAQ will run for. If set to -1 this is ignorned.
    int SoftwareTrigger;    //Should the software trigger be used. 0 is true, 1 is false.
    int MaxEventsPerFile;   //Max Number of Events in a File 
    int RunOnlineAnalysis;  //Do the Online Analysis

    std::vector<uint32_t> BoardsBaseAddress; //Board Base Addresses
    
    std::map<int, std::vector<int> > GroupTriggerMasks;      //Map of boards and there groups. Used to turn Groups on and off for triggering.       
    std::map<int, std::vector<int> > GroupTriggerThresholds; //Map of boards and there groups. Used to set trigger thresholds for groups.
    std::map<int, int>               GroupAquisitionMasks;   //Map of boards and there groups. Used to turn Groups on and off for aquisition.


    DAQConfig() {}
  };

  struct Header{
  public: 
    int NumBoards;
    int ReadoutSize;
    int ASIC_Gain;
    int ASIC_Shaping_Time;
    
    Header(){}
  };

  struct EventHeader{
  public: 
    uint32_t EventNumber;
    uint32_t Timestamp;
    uint32_t EventSize;

    EventHeader() {}
  };

public :

  DAQConfig DAQConfig;
  Header header;
  EventHeader eventheader;

  void PrintConfig();
  void QuitProgram();
  int  StartAcquisition();
  int  OnlineAnalysis();

  PixelReadoutErrorCode err = 0;

private:

  std::vector<int> handle; //handler will be used for most of CAENDigitizer functions to identify the board       
  CAEN_DGTZ_ErrorCode ret = CAEN_DGTZ_Success; //Return error message to check things have passed. 
  char *buffer = NULL;

  int  InitialiseBoards();
  int  InitialiseTriggers();
  int  ReadConfig();

};

#endif //PixelReadout_hh 
