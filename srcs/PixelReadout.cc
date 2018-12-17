//##########################################################################
//## Title:       PixelReadout.cc                                         ##
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

#include "PixelReadout.hh"
#include "keyb.hh"

DAQ::PixelReadout::PixelReadout(){

  err = this->ReadConfig();
  if(!err){err = this->InitialiseBoards();}
  if(!err){err = this->InitialiseTriggers();}
  if(!err){err = this->InitialiseOffsets();}
  if(!err){err = this->InitialiseAcquisition();}
}

int DAQ::PixelReadout::InitialiseBoards(){

  CAEN_DGTZ_BoardInfo_t BoardInfo;
  //Loop over the boards and Open the digitiser
  for(int b=0; b<DAQConfig.MAXNB; b++){
 
    //Open the digitiser through the Nim Board. 
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,DAQConfig.BoardsBaseAddress[b],&handle[b]);
    
    if(ret != CAEN_DGTZ_Success){
      std::cerr << "Can't open digitizer. Error code: " << (int) ret << std::endl;
      this->QuitProgram();
      return 1;
    }
    
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_Reset(handle[b]);}                                               // Reset Digitizer 
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);}                                 // Get Board Info
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetRecordLength(handle[b],DAQConfig.ReadoutSize);}              // Set the length of each waveform (in samples) 
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetMaxNumEventsBLT(handle[b],DAQConfig.MaxEvents);}             // Set the max number of events to transfer in a single readout

    if(ret != CAEN_DGTZ_Success) {
      std::cerr << "Errors during Digitizer Configuration: " << (int) ret << std::endl;
      this->QuitProgram();
      return 1;
    }
    else { 
      std::cout << "Connected to CAEN Digitizer Model " << BoardInfo.ModelName << " recognized as board " << b << std::endl;
      std::cout << "ROC FPGA Release is " << BoardInfo.ROC_FirmwareRel << std::endl;
      std::cout << "FPGA Release is " << BoardInfo.AMC_FirmwareRel << std::endl;
      std::cout << "Initialisation Complete" << std::endl; 
    }
  }
  return 0;
}

void DAQ::PixelReadout::QuitProgram(){

  // Free the buffers and close the digitizers                   
  ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);

  // Close the digitiser 
  for(int b=0; b<DAQConfig.MAXNB; b++){
    ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
  }

  if(ret != CAEN_DGTZ_Success){
    std::cerr << "Error in Closing the digitiser. Error Code: " << (int) ret << std::endl; 
  }

  return;
}

int QuitAcquisition(){

  int c = 0;

 if(!kbhit())
    return 0;

  c = getch();
  if(c == 'q'){
    return 1;
  }

  return 0;
}



int DAQ::PixelReadout::ReadConfig(){

  std::ifstream defparams("/home/argonshef/CAENDAQ/PixelDAQ/srcs/DAQConfig.txt");
  std::string parname;
  std::string parval;
  std::string line;
  std::string parname_temp;

  std::cout << "############################################" << std::endl;
  std::cout << "####    Setting DAQ Config Parameters   ####" << std::endl;

  //Make sure all the parameters are filled
  int par=0;

  if (defparams.is_open()){
    while (std::getline(defparams,line)){
  	std::stringstream ss(line);
	
	parname_temp = parname;
        ss >> parname;

        if(parname[0] == '#'){continue;}
	if(parname == parname_temp){continue;}

        ss  >> parval;


	//#########################
	//### Global Properties ###
	//#########################
 
  	if(parname == "MAXNB")             {DAQConfig.MAXNB             = std::stoi(parval);++par;}
  	if(parname == "ReadoutSize")       {DAQConfig.ReadoutSize       = std::stoi(parval);++par;}
  	if(parname == "ASIC_Gain")         {DAQConfig.ASIC_Gain         = std::stof(parval);++par;}
  	if(parname == "ASIC_Shaping_Time") {DAQConfig.ASIC_Shaping_Time = std::stof(parval);++par;}
  	if(parname == "MaxEvents")         {DAQConfig.MaxEvents         = std::stoi(parval);++par;}
	if(parname == "MaxTime")           {DAQConfig.MaxTime           = std::stof(parval);++par;}
	if(parname == "SoftwareTrigger")   {DAQConfig.SoftwareTrigger   = std::stof(parval);++par;}
	if(parname == "MaxEventsPerFile")  {DAQConfig.MaxEventsPerFile  = std::stof(parval);++par;}
	if(parname == "RunOnlineAnalysis") {DAQConfig.RunOnlineAnalysis = std::stof(parval);++par;}
	if(parname == "Verbose")           {DAQConfig.Verbose           = std::stof(parval);++par;}
	if(parname == "TimeOffset")        {DAQConfig.TimeOffset        = std::stof(parval);++par;}

      	if(parname == "BoardsBaseAddress"){
	  ++par;
      	  int iter=0;
      	  while(parval != "]" && iter<DAQConfig.MAXNB){

      	    if(parval == "["){  ss >> parval; continue;}
      	    if(parval == "#"){iter=DAQConfig.MAXNB;continue;}

	    DAQConfig.BoardsBaseAddress.push_back(std::stoi(parval,nullptr,16)); 
      	    ++iter;
	    ss  >> parval;
      	  }
	}

	//############################
	//### Local DAQ Properties ###
	//############################
	if(parname == "GroupTriggerMasks"){
	  ++par;
	  int board_address = std::stoi(parval,nullptr,16);
	  ss >> parval;
	  while(parval != "]"){
	    if(parval == "["){ss >> parval;continue;}
	    if(parval == "#"){break;}
	    DAQConfig.GroupTriggerMasks[board_address].push_back(std::stoi(parval));
	    ss >> parval;
	  }
	}

	if(parname == "GroupAquisitionMasks"){
	  ++par;
	  int board_address = std::stoi(parval,nullptr,16);
	  ss >> parval; 
	  DAQConfig.GroupAquisitionMasks[board_address] = std::stoi(parval);
	}

	if(parname == "GroupTriggerThresholds"){
	  ++par;
          int board_address = std::stoi(parval,nullptr,16);
	  ss >> parval;
	  while(parval != "]"){
	    if(parval == "["){ss >> parval; continue;}
	    if(parval == "#"){break;}
	    DAQConfig.GroupTriggerThresholds[board_address].push_back(std::stoi(parval));
	    ss >> parval;
	  }
	}
	
	if(parname == "GroupPolarity"){
	  ++par;
          int board_address = std::stoi(parval,nullptr,16);
	  ss >> parval;
	  while(parval != "]"){
	    if(parval == "["){ss >> parval; continue;}
	    if(parval == "#"){break;}
	    DAQConfig.GroupPolarity[board_address].push_back(std::stoi(parval));
	    ss >> parval;
	  }
	}
	
	if(parname == "GroupDCOffset"){
	  ++par;
          int board_address = std::stoi(parval,nullptr,16);
	  ss >> parval;
	  while(parval != "]"){
	    if(parval == "["){ss >> parval; continue;}
	    if(parval == "#"){break;}
	    DAQConfig.GroupDCOffset[board_address].push_back(std::stoi(parval));
	    ss >> parval;
	  }
	}

    }
  }

  //Increase the handle size
  handle.resize(DAQConfig.MAXNB);

  std::cout << "#### Finished Setting Config Parameters ####" << std::endl; 
  std::cout << "############################################" << std::endl;
  std::cout << std::endl;

  this->PrintConfig();

  if(par!=17){
    std::cout << "Config only partially filled. Error" << std::endl;
    return -1;
  }  
  
  return 0;
}

void DAQ::PixelReadout::PrintConfig(){

  int big_name = 32;
  int spacing = 9*DAQConfig.BoardsBaseAddress.size() ;
  if(spacing < 9 + 1 + 16){spacing = 9 + 1 + 32;}
  std::cout << "##" << std::setfill('#') << std::setw(big_name + spacing) << "##"  << std::endl;
  std::cout << "##" << std::right << std::setw(std::floor((big_name+spacing+19-2)/2)) << " Config Parameters " << std::left << std::setw(std::ceil((big_name+spacing-19+3)/2)) << "##" << std::endl;
  std::cout << "##" << std::setfill('#') << std::setw(big_name + spacing) << "##"  << std::endl;
  std::cout << std::setfill(' ');
  std::cout << std::left << std::setw(big_name) << "## MAXNB: " << std::left <<  std::setw(spacing)  << DAQConfig.MAXNB << "##" << std::endl; 
  std::cout << std::left << std::setw(big_name) << "## Readout Size: " << std::left << std::setw(spacing) <<  DAQConfig.ReadoutSize << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## ASIC gain: " << std::left << std::setw(spacing) << DAQConfig.ASIC_Gain << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## ASIC shaping time: " << std::left << std::setw(spacing) << DAQConfig.ASIC_Shaping_Time << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Max Number of Events: " << std::left << std::setw(spacing) << DAQConfig.MaxEvents << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Max Time: " << std::left << std::setw(spacing) << DAQConfig.MaxTime << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Set Software Trigger: " << std::left << std::setw(spacing) << DAQConfig.SoftwareTrigger  << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Max Events per File: " << std::left << std::setw(spacing) << DAQConfig.MaxEventsPerFile  << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Run Online Analysis: " << std::left << std::setw(spacing) << DAQConfig.RunOnlineAnalysis  << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## Verbose: " << std::left << std::setw(spacing) << DAQConfig.Verbose   << "##" << std::endl;
  std::cout << std::left << std::setw(big_name) << "## TimeOffset: " << std::left << std::setw(spacing) << DAQConfig.TimeOffset   << "##" << std::endl;

  std::cout << std::setw(big_name) << "## Board Base Addresses: " << std::flush;
  for(int i=0; i<DAQConfig.BoardsBaseAddress.size();++i){
    std::cout << std::hex << DAQConfig.BoardsBaseAddress[i] << " " << std::flush;
  }
  std::cout << std::dec << std::right << std::setw(spacing - (8+1)*DAQConfig.BoardsBaseAddress.size()+2) << "##" << std::endl;

  for(std::map<int,std::vector<int> >::iterator board_iter=DAQConfig.GroupTriggerMasks.begin(); board_iter!=DAQConfig.GroupTriggerMasks.end(); ++board_iter){
    std::cout << std::left << std::setw(big_name) << "## GroupTriggerMasks: " << std::hex <<  board_iter->first << " " << std::dec << std::flush; 
    int iter=0;
    for(int j=0; j<(board_iter->second).size(); ++j){
      int logvalue;
      if(std::abs(board_iter->second.at(j) == 0)){logvalue = 0;}
      else{logvalue = (std::log10(std::abs(board_iter->second.at(j))));}
      if(board_iter->second.at(j) < 0){logvalue += 1;}
      iter += std::floor (logvalue + 1 + 1);
      std::cout << board_iter->second.at(j) << " " << std::flush;
    }
    std::cout << std::right << std::setw(spacing-iter-7) <<  "##" << std::endl;
  }
  
  for(std::map<int,int >::iterator board_iter=DAQConfig.GroupAquisitionMasks.begin(); board_iter!=DAQConfig.GroupAquisitionMasks.end(); ++board_iter){
    std::cout << std::left << std::setw(big_name) << "## GroupAquisitionMasks: " << std::hex <<  board_iter->first << " " << std::dec << std::left << std::setw(spacing-9) << board_iter->second << "##" << std::endl; 
  }

  for(std::map<int,std::vector<int> >::iterator board_iter=DAQConfig.GroupTriggerThresholds.begin(); board_iter!=DAQConfig.GroupTriggerThresholds.end(); ++board_iter){
    std::cout << std::left << std::setw(big_name) << "## GroupTriggerThresholds: " << std::hex <<  board_iter->first << " " << std::dec << std::flush; 
    int iter=0;
    for(int j=0; j<(board_iter->second).size(); ++j){
      int logvalue;
      if(std::abs(board_iter->second.at(j) == 0)){logvalue = 0;}
      else{logvalue = (std::log10(std::abs(board_iter->second.at(j))));}
      if(board_iter->second.at(j) < 0){logvalue += 1;}
      iter += std::floor (logvalue + 1 + 1);
      std::cout << board_iter->second.at(j) << " " << std::flush;
    }
    std::cout << std::right << std::setw(spacing-iter-7) <<  "##" << std::endl;
  }

  for(std::map<int,std::vector<int> >::iterator board_iter=DAQConfig.GroupPolarity.begin(); board_iter!=DAQConfig.GroupPolarity.end(); ++board_iter){
    std::cout << std::left << std::setw(big_name) << "## GroupPolarity: " << std::hex <<  board_iter->first << " " << std::dec << std::flush; 
    int iter=0;
    for(int j=0; j<(board_iter->second).size(); ++j){
      int logvalue;
      if(std::abs(board_iter->second.at(j) == 0)){logvalue = 0;}
      else{logvalue = (std::log10(std::abs(board_iter->second.at(j))));}
      if(board_iter->second.at(j) < 0){logvalue += 1;}
      iter += std::floor (logvalue + 1 + 1);
      std::cout << board_iter->second.at(j) << " " << std::flush;
    }
    std::cout << std::right << std::setw(spacing-iter-7) <<  "##" << std::endl;
  }
  
  for(std::map<int,std::vector<int> >::iterator board_iter=DAQConfig.GroupDCOffset.begin(); board_iter!=DAQConfig.GroupDCOffset.end(); ++board_iter){
    std::cout << std::left << std::setw(big_name) << "## GroupDCOffset: " << std::hex <<  board_iter->first << " " << std::dec << std::flush; 
    int iter=0;
    for(int j=0; j<(board_iter->second).size(); ++j){
      int logvalue;
      if(std::abs(board_iter->second.at(j) == 0)){logvalue = 0;}
      else{logvalue = (std::log10(std::abs(board_iter->second.at(j))));}
      if(board_iter->second.at(j) < 0){logvalue += 1;}
      iter += std::floor (logvalue + 1 + 1);
      std::cout << board_iter->second.at(j) << " " << std::flush;
    }
    std::cout << std::right << std::setw(spacing-iter-7) <<  "##" << std::endl;
  }


  std::cout << "##" << std::setfill('#') << std::setw(big_name + spacing) << "##"  << std::endl; 
  std::cout << std::setfill(' ');

  return; 
}


int DAQ::PixelReadout::InitialiseTriggers(){

  if(DAQConfig.SoftwareTrigger){
   for(int b=0; b<DAQConfig.MAXNB; b++){
     
     //Set the software trigger to get aquire the values only
     if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetSWTriggerMode(handle[b],CAEN_DGTZ_TRGMODE_ACQ_ONLY);}
     if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetExtTriggerInputMode(handle[b],CAEN_DGTZ_TRGMODE_DISABLED);}
   }
  }
  else{
    for(int b=0; b<DAQConfig.MAXNB; b++){

      int board_address = DAQConfig.BoardsBaseAddress[b];
      
      //Annoyingly we can set the channel triggers individually we have to do it in groups
      for(uint32_t group=0; group<8; ++group){

	//Set the triggers
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetGroupTriggerThreshold(handle[b], group, DAQConfig.GroupTriggerThresholds[board_address][group]);}

	//Enable the groups for triggering. 	
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetGroupSelfTrigger(handle[b],(CAEN_DGTZ_TriggerMode_t) DAQConfig.GroupTriggerMasks[board_address][group],group);}
	
	//Set the Polarity.
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetTriggerPolarity(handle[b],group,(CAEN_DGTZ_TriggerPolarity_t)  DAQConfig.GroupPolarity[board_address][group]);}

	//Set the trigger mode. Pass to the the output trigger as well as take data.
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetExtTriggerInputMode(handle[b],CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT);}
      }
    }
  }



  if(ret != CAEN_DGTZ_Success){
    std::cerr << "Errors during Trigger Configuration. Error code: " << (int) ret << std::endl;
    this->QuitProgram();
    return 1;
  }
  else{
    std::cout << "Trigger Initialsation Complete" << std::endl;
  }

  return 0; 
}

int DAQ::PixelReadout::InitialiseOffsets(){

  for(int b=0; b<DAQConfig.MAXNB; b++){
    
    int board_address = DAQConfig.BoardsBaseAddress[b];

    //Set the timing offset this is a percentage from 0% = trigger is at the end.
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetPostTriggerSize(handle[b], DAQConfig.TimeOffset);}
 
    for(uint32_t group=0; group<8; ++group){
      //Set DC offset for  V1740 is controleld by a 16bits DAC and by default off set is set of -Vpp/2 so range is from -Vpp/2 to +Vpp/2. for V1740 Vpp=2V DCOffset set per bit 0 to 65535. 0 is +1V. 0V = 2048.  
      //0x10C0 + 0x100· n -> Correction values for channel offset 0.3 
      //0x10C4 + 0x100· n -> Correction values for channel offset 4..7
      if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetGroupDCOffset(handle[b],group, DAQConfig.GroupDCOffset[board_address][group]);}
    }
  }
  
  if(ret != CAEN_DGTZ_Success){
    std::cerr << "Errors during Offset Configuration. Error code: " << (int) ret << std::endl;
    this->QuitProgram();
    return 1;
  }
  else{
    std::cout << "Offset Initialsation Complete" << std::endl;
  }

  return 0; 
}

int DAQ::PixelReadout::InitialiseAcquisition(){

  for(int b=0; b<DAQConfig.MAXNB; b++){

    //Tell which group should be involved in the aquistion. All = 255 
    int board_address = DAQConfig.BoardsBaseAddress[b];
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetGroupEnableMask(handle[b],DAQConfig.GroupAquisitionMasks[board_address]);}

    //Set the aquisition start. If multiple boards this woudld require a change.
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SetAcquisitionMode(handle[b],CAEN_DGTZ_SW_CONTROLLED);}
  }

  if(ret != CAEN_DGTZ_Success){
    std::cerr << "Errors during Aquisition Configuration. Error code: " << (int) ret << std::endl;
    this->QuitProgram();
    return 1;
  }
  else{
    std::cout << "Aquisition Initialsation Complete" << std::endl;
  }
  return 0;
}
  

int DAQ::PixelReadout::StartAcquisition(){

  CAEN_DGTZ_EventInfo_t    eventInfo; //Event header info
  CAEN_DGTZ_UINT16_EVENT_t *Evt=NULL; //Event data buffer
  char                     *evtptr = NULL;
  uint32_t                 size,bsize;
  uint32_t                 numEvents;
  int                      TotalnumEvents = 0;

  int quit         = 0; 
  int start_time   = std::time(0);
  int time_passed  = 0;
  
  int MaxEvents = DAQConfig.MaxEvents;
  int MaxTime   = DAQConfig.MaxTime;

  if(MaxEvents < 1){MaxEvents = std::numeric_limits<int>::max();}
  if(MaxTime   < 1){MaxTime   = std::numeric_limits<int>::max();}

  if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_MallocReadoutBuffer(handle[0],&buffer,&size);}

  for(int b=0; b<DAQConfig.MAXNB; b++){
    if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SWStartAcquisition(handle[b]);}
    std::cout << "Aquisition Started" << std::endl;
  }

  while(TotalnumEvents < MaxEvents && time_passed < MaxTime && !quit && ret==0){ 

    //Initalise the Output file
    //Get the time now.
    time_t now = time(0);
  
    //Define the structure that holds the time.
    struct tm tstruct;
    char filename[30];
    tstruct = *localtime(&now);
    
    //Make time into a string.
    strftime(filename,sizeof(filename),"PixelTPC_%Y%m%d_%H%M%S.dat", &tstruct);
    
    //Create the data file.
    std::ofstream outputfile(filename,std::ios::out|std::ios::binary);
    
    if (!outputfile.is_open())
      {
	std::cerr << "error: open file for output failed!" << std::endl;
	this->QuitProgram();
	ret = (CAEN_DGTZ_ErrorCode) -99999999;
      }

    //Write the File header 
    header.NumBoards         = DAQConfig.MAXNB;
    header.ReadoutSize       = DAQConfig.ReadoutSize;
    header.ASIC_Gain         = DAQConfig.ASIC_Gain;
    header.ASIC_Shaping_Time = DAQConfig.ASIC_Shaping_Time;

    outputfile.write((char*)&header,sizeof(Header));
    
    if(ret != 0){continue;}

    //Loop over the output file until we reach the max number of events.
    int EventsInFile=0; 
    while(EventsInFile < DAQConfig.MaxEventsPerFile){  

      //Check the time and event number and finished if needed
      if(TotalnumEvents > MaxEvents || time_passed > MaxTime || quit){
	break;
      }
        
      for(int b=0; b<DAQConfig.MAXNB; b++) {
            
	//Send software trigger
	if(DAQConfig.SoftwareTrigger){
	  Sleep(100);
	  if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_SendSWtrigger(handle[b]);} 
	}

	//Read the buffer from the digitizer
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_ReadData(handle[b],CAEN_DGTZ_SLAVE_TERMINATED_READOUT_MBLT,buffer,&bsize);}
	
	//The buffer read from the digitizer is used in the other functions to get the event data. The following function returns the number of events in the buffer 
	if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_GetNumEvents(handle[b],buffer,bsize,&numEvents);}
	int buffersize = bsize;
      
	//Only count one event for all the boards
	if(b==0){
	  EventsInFile += numEvents;
	}

	for (int ev=0;ev<numEvents;ev++) {
	  
	  // Get the Infos and pointer to the event
	  if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_GetEventInfo(handle[b],buffer,bsize,ev,&eventInfo,&evtptr);}
	  
	  int Eventsize = eventInfo.EventSize;
	  
	  //Decode the event to get the data
	  if(ret == CAEN_DGTZ_Success){ret = CAEN_DGTZ_DecodeEvent(handle[b],evtptr,(void**) &Evt);}

	  //Fill the header information
	  eventheader.Timestamp         = eventInfo.TriggerTimeTag;
	  eventheader.EventNumber       = eventInfo.EventCounter; 
	  eventheader.BoardBaseAddress  = eventInfo.BoardId;
	  eventheader.NumChannels       = sizeof(Evt->DataChannel)/sizeof(Evt->DataChannel[0]); 
	  eventheader.ChSize            = Evt->ChSize[0];

	  if(DAQConfig.Verbose){
	    std::cout << "Event Found!" << std::endl;
	    std::cout << " Event Number: " << eventheader.EventNumber << std::endl;
	    std::cout << " Event Time  : " << eventheader.EventNumber << std::endl;
	    std::cout << " Board ID    : " << eventheader.BoardBaseAddress << std::endl;
	  }
	
	  //Write the data to the file
	  outputfile.write((char*)&eventheader,sizeof(EventHeader));

	  for(uint16_t ch=0; ch<sizeof(Evt->DataChannel)/sizeof(Evt->DataChannel[0]); ++ch){
	    outputfile.write((char*)Evt->DataChannel[ch],sizeof(uint16_t)*eventheader.ChSize);
	  }

	  if(DAQConfig.RunOnlineAnalysis){
	    std::cout << " Event Number: " << eventheader.EventNumber << std::endl;
	    for(uint16_t ch=0; ch<sizeof(Evt->DataChannel)/sizeof(Evt->DataChannel[0]); ++ch){
	      
	      for(int adc_it=0; adc_it<(Evt->ChSize[ch]); ++adc_it){
		std::cout << Evt->DataChannel[ch][adc_it] << " ";
	      }
	      std::cout << " " << std::endl;
	    }
	  }
	}
	//Free the Event 
	if(ret == CAEN_DGTZ_Success){CAEN_DGTZ_FreeEvent(handle[b],(void**) &Evt);}
      }//End of loop on boards                
      
      //Check if it time to end the acquisition 
      quit = QuitAcquisition();
      time_passed  = std::time(0) - start_time;
    }//Loop for filling file

    std::cout << " File: " << filename << " written" << std::endl;
    outputfile.close();
    TotalnumEvents += EventsInFile;

  }//While Loop

  //End the aquisition
  for(int b=0; b<DAQConfig.MAXNB; b++){
    ret = CAEN_DGTZ_SWStopAcquisition(handle[b]);
    std::cout << "Aquisition finished" << std::endl;
  }

  if(ret != CAEN_DGTZ_Success) {
    std::cerr << " Errors Data Aquisition. Error code: " << (int) ret << std::endl;
    this->QuitProgram();
    return 1;
  }
  else {
    std::cout << "Data Aquisition Complete with " << TotalnumEvents << " Events taken." << std::endl;
  }
 
  return 0;
}


