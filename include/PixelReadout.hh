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
#include "CAENComm.h"

//C++ Includes 
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
#include <chrono>

//Analysis Includes
#include "/home/argonshef/LArAnalysis/srcs/TPC/OnlineAnalysis/OnlineMonitor.hh"
#include "/home/argonshef/LArAnalysis/srcs/TPC/OnlineAnalysis/OnlineEventDisplay.hh"
#include "/home/argonshef/LArAnalysis/srcs/TPC/OnlineAnalysis/OnlineDataBase.hh"

//DAQ Includes
#include "ChannelMap.hh"
#include "keyb.hh"

//ROOT Includes
#include "TApplication.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

typedef int PixelReadoutErrorCode;

namespace DAQ {
  class PixelReadout;
}


class DAQ::PixelReadout {

public:

  PixelReadout();
  std::map<int, std::string> channelMap;
  
  //Holder for the DAQ config
  struct DAQConfig{
  public:
    
    int MAXNB;                    //Number of connected boards 
    int ReadoutSize;              //Size of the Waveform
    int ASIC_Gain;                //Gain of the pre-amps
    int ASIC_Shaping_Time;        //Shaping time of the pre-amps
    int MaxEvents;                //Max number of events in the readout size. If set to -1 this is ignorned  
    int MaxTime;                  //Max time that the DAQ will run for. If set to -1 this is ignorned.
    int SoftwareTrigger;          //Should the software trigger be used. 0 is true, 1 is false.
    int MaxEventsPerFile;         //Max Number of Events in a File 
    int RunOnlineAnalysis;        //Do the Online Analysis
    int Verbose;                  //Give some extra information when runnig the DAQ
    uint32_t TimeOffset;          //Timeoffset from trigger. Give as a %
    int AverageTime;              //Time to average waveform over 
    int ApplyAverageWaveform;     //Bool should you average over the waveform.
    int ApplyDecimationFactor;    //Bool should we apply decimation? 
    uint16_t DecimationFactor;    //New Frequency = Nominal Frequency/2^This number
    int RunEventDiplay;           //Bool should we run the event display.
    int RunEventDisplayOverEveryXEvent; //How many events should we wait to run the evd.
    int DeadTime;                 //Time to wait after every event before triggering
    std::vector<uint32_t> BoardsBaseAddress; //Board Base Addresses
    
    std::map<int, std::vector<uint32_t> > GroupTriggerMasks;      //Map of boards and there groups. Used to turn Groups on and off for triggering.       
    std::map<int, std::vector<uint32_t> > GroupTriggerThresholds; //Map of boards and there groups. Used to set trigger thresholds for groups.
    std::map<int, std::vector<uint32_t> > GroupPolarity;          //Map of boards and there groups. Used to set the polarity.
    std::map<int, std::vector<float> >    GroupDCOffset;          //Map of boards and there groups. Used to set the DCOffset. 
    std::map<int, uint32_t>               GroupAquisitionMasks;   //Map of boards and there groups. Used to turn Groups on and off for aquisition.

    DAQConfig(){}
    ~DAQConfig(){}
  };

  struct Header{
  public: 
    int NumBoards = -99999;
    int ReadoutSize = -99999 ;
    int ASIC_Gain = -99999;
    int ASIC_Shaping_Time = -99999;

    Header(){}
    ~Header(){}
  };

  struct EventHeader{
  public: 
    uint32_t EventNumber = 99999;
    uint32_t Timestamp_s = 99999;
    uint32_t Timestamp_ns = 99999;
    uint32_t EventSize = 99999;
    uint32_t BoardBaseAddress = 99999;
    uint32_t NumChannels = 99999;
    uint32_t ChSize = 99999;
    uint32_t TriggerType = 99999;
    EventHeader(){}
    ~EventHeader(){}
  };

public :

  DAQConfig DAQConfig;
  Header header;
  EventHeader eventheader;

  void PrintConfig();
  void QuitProgram();
  int  StartAcquisition();
  int  OnlineAnalysis();

  PixelReadoutErrorCode err  = 0;

private:

  std::vector<int> handle; //handler will be used for most of CAENDigitizer functions to identify the board       
  CAEN_DGTZ_ErrorCode ret    = CAEN_DGTZ_Success; //Return error message to check things have passed. 
  CAENComm_ErrorCode commerr = CAENComm_Success; //Return error message code for the CAEN commincation 

  char *buffer = NULL;

  int InitialiseBoards();
  int InitialiseTriggers();
  int InitialiseOffsets();
  int InitialiseAcquisition();
  int ReadConfig();
  uint16_t* AverageWaveform(uint16_t* waveform, uint32_t ChSize, uint32_t AveragedChSize);
  uint16_t* CopyWaveform(uint16_t* waveform,uint32_t ChSize);
};

#endif //PixelReadout_hh 
