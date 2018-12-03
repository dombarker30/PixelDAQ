//###########################################################
//## Title:       DAQDriver.cc                             ##
//## Author:      Dominic Barker                           ##
//## Description: Driver used to run the code.             ##
//##              run the excutable in bin/x86_64/Release/ ##
//##              to run the DAQ with settings from        ##
//##              srsc/DAQConfig.txt                       ## 
//###########################################################

#include "PixelReadout.hh"
#include "keyb.hh"

int main(int argc, char* argv[]){

  //Initialise the DAQ - Open the DAQ, read config, set the triggers.
  DAQ::PixelReadout PixelReadout;

  //Start the Acquisition 
  if(!PixelReadout.err){PixelReadout.err = PixelReadout.StartAcquisition();}

  //Close The DAQ
  if(!PixelReadout.err){PixelReadout.QuitProgram();}
  
  if(!PixelReadout.err){std::cout << "DAQ Finished sucessfully" << std::endl;}

  return 0;
}
  
