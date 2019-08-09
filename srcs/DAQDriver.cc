//###########################################################
//## Title:       DAQDriver.cc                             ##
//## Author:      Dominic Barker                           ##
//## Description: Driver used to run the code.             ##
//##              run the excutable in bin/x86_64/Release/ ##
//##              to run the DAQ with settings from        ##
//##              srsc/DAQConfig.txt                       ## 
//###########################################################

#include "PixelReadout.hh"
#include "ArduinoSetup.hh"

int main(int argc, char* argv[]){

  TApplication theApp("App",&argc, argv);

  //  Inatalise the arduino - read config,print config, send config
  if(argc < 2){
    DAQ::ArduinoSetup ArduinoSetup;
    if(ArduinoSetup.err){
      std::cerr << "Error in setting up the arduino" << std::endl;
      return -1;
    }
  }

  //Initialise the DAQ - Open the DAQ, read config, set the triggers.
  DAQ::PixelReadout PixelReadout;

  Sleep(5000);


  //Start the Acquisition 
  if(!PixelReadout.err){PixelReadout.err = PixelReadout.StartAcquisition();}

  //Close The DAQ
  PixelReadout.QuitProgram();
  
  if(!PixelReadout.err){std::cout << "DAQ Finished sucessfully" << std::endl;}

  return 0;
}
  
