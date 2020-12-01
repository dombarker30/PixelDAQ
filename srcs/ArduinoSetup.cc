#include "ArduinoSetup.hh"

DAQ::ArduinoSetup::ArduinoSetup(){

  err = this->ReadConfig();
  if(!err){err = this->PrintConfig();}
  if(!err){err = this->SendConfig();}

}

//Read the config file and add the file to the arduino config.
int DAQ::ArduinoSetup::ReadConfig(){
  
  std::ifstream defparams("/home/argonshef/CAENDAQ/PixelDAQ/srcs/DAQConfig.txt");
  std::string parname;
  std::string parval;
  std::string line;
  std::string parname_temp;

  std::cout << "##############################" << std::endl;
  std::cout << "####    Setting Arduino   ####" << std::endl;
  
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

	//For the binary parmas 
	if(parname == "LeakageControl")        {ArduinoConfig.LeakageControl           = std::stoi(parval);}
	if(parname == "MonitorSelector")       {ArduinoConfig.MonitorSelector          = std::stoi(parval);}
	if(parname == "MonitorOn")             {ArduinoConfig.MonitorOn                = std::stoi(parval);}
	if(parname == "FilterOn")              {ArduinoConfig.FilterOn                 = std::stoi(parval);}
	if(parname == "OutputBufferOn")        {ArduinoConfig.OutputBufferOn           = std::stoi(parval);}
	if(parname == "OutputCoupling")        {ArduinoConfig.OutputCoupling           = std::stoi(parval);}
	if(parname == "TestCapacitanceEnabled"){ArduinoConfig.TestCapacitanceEnabled   = std::stoi(parval);}

	//For the value params
	if(parname == "ASIC_Shaping_Time"){
	  float ShapingTime = std::stof(parval);
	  if(ShapingTime == 1){
	    ArduinoConfig.ShapingTimeBit0 = 0;
	    ArduinoConfig.ShapingTimeBit1 = 0;
	  }
	  else if(ShapingTime == 0.5){
	    ArduinoConfig.ShapingTimeBit0 = 1;
	    ArduinoConfig.ShapingTimeBit1 = 0;
	  }
	  else if(ShapingTime == 3){
	    ArduinoConfig.ShapingTimeBit0 = 0;
	    ArduinoConfig.ShapingTimeBit1 = 1;
	  }
	  else if(ShapingTime == 2){
	    ArduinoConfig.ShapingTimeBit0 = 1;
	    ArduinoConfig.ShapingTimeBit1 = 1;
	  }
	  else{
	    std::cerr << "Shaping time incorrectly intialised. Please set the value to 1/0.5/3/2 us (no units)" << std::endl;
	    return -1;
	  }
	}

	//For the value params
	if(parname == "ASIC_Gain"){
	  float Gain = std::stof(parval);
	  if(Gain ==(float) 4.7){
	    ArduinoConfig.GainBit0 = 0;
	    ArduinoConfig.GainBit1 = 0;
	  }
	  else if(Gain ==(float) 7.8){
	    ArduinoConfig.GainBit0 = 1;
	    ArduinoConfig.GainBit1 = 0;
	  }
	  else if(Gain ==(float) 14){
	    ArduinoConfig.GainBit0 = 0;
	    ArduinoConfig.GainBit1 = 1;
	  }
	  else if(Gain ==(float) 25){
	    ArduinoConfig.GainBit0 = 1;
	    ArduinoConfig.GainBit1 = 1;
	  }
	  else{
	    std::cerr << "Gain incorrectly intialised. Please set the value to 4.7/7.8/10/25 mV/fC (no units)" << std::endl;
	    return -1;
	  }
	}

	if(parname == "Baseline"){
	  float Baseline =  std::stof(parval);
	  if(Baseline == 200){
	    ArduinoConfig.Baseline = 1;
	  }
	  else if(Baseline == 900){
	    ArduinoConfig.Baseline = 0;
	  }
	  else{std::cerr << "Baseline not initalised correctly. Please set to 200 mV or 900 mV (no units)" << std::endl;
	    return -1;
	  }
	}

	if(parname == "TestPulseDuration"){ArduinoConfig.TestPulseDuration   = std::stoi(parval);}
	if(parname == "TestPulsePeriod"){ArduinoConfig.TestPulsePeriod   = std::stoi(parval);}

	if(parname == "EnableTestPulses"){
	  int EnableTestPulses =  std::stoi(parval); 
	  if(!EnableTestPulses){
	    ArduinoConfig.TestPulseDuration = 0;
	    ArduinoConfig.TestPulsePeriod   = 0;
	  }
	}
    }
  }	  

  std::cout << "####       Complete       ####" << std::endl;
  std::cout << "##############################" << std::endl;   
  std::cout << " " << std::endl;

  return 0;
};


//Print the current config which will sent to the arduino
int DAQ::ArduinoSetup::PrintConfig(){

  std::cout << "##" << std::setfill('#') << std::setw(38+3) <<  "##"  << std::endl;
  std::cout << "##" << std::right << std::setw((38+3+18)/2) << " Arduino Parameters " << std::left  << std::setw((38+3-18+2)/2)<< "##" << std::endl;
  std::cout << "##" << std::setfill('#') << std::setw(38+3) <<  "##"  << std::endl;
  std::cout << std::setfill(' ');
  std::cout << std::left << std::setw(36) << "## LeakageControl: " << std::left <<  std::setw(4)  << ArduinoConfig.LeakageControl << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## MonitorSelector: " << std::left <<  std::setw(4)  << ArduinoConfig.MonitorSelector << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## MonitorOn: " << std::left <<  std::setw(4)  << ArduinoConfig.MonitorOn << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## FilterOn: " << std::left <<  std::setw(4)  << ArduinoConfig.FilterOn << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## OutputBufferOn: " << std::left <<  std::setw(4)  << ArduinoConfig.OutputBufferOn << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## OutputCoupling: " << std::left <<  std::setw(4)  << ArduinoConfig.OutputCoupling << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## ShapingTimeBits: " << std::left << ArduinoConfig.ShapingTimeBit0 <<  std::setw(3) << ArduinoConfig.ShapingTimeBit1  << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## GainBits: " << std::left  << ArduinoConfig.GainBit0 <<  std::setw(3) << ArduinoConfig.GainBit1  << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## Baseline: " << std::left <<  std::setw(4)  << ArduinoConfig.Baseline << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## TestCapacitanceEnabled: " << std::left <<  std::setw(4)  << ArduinoConfig.TestCapacitanceEnabled << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## TestPulseDuration: " << std::left <<  std::setw(4)  << ArduinoConfig.TestPulseDuration << " ##" << std::endl;
  std::cout << std::left << std::setw(36) << "## TestPulsePeriod: " << std::left <<  std::setw(4)  << ArduinoConfig.TestPulsePeriod << " ##" << std::endl;
  std::cout << "##" << std::setfill('#') << std::setw(38+3) <<  "##"  << std::endl; 
  std::cout << " " << std::endl;  

  return 0;
};

//Send the condfig to the arduino 
int DAQ::ArduinoSetup::SendConfig(){

  ArduinoCommands.G_SLK  += std::to_string(ArduinoConfig.LeakageControl); 
  ArduinoCommands.G_STB1 += std::to_string(ArduinoConfig.MonitorSelector);
  ArduinoCommands.G_STB  += std::to_string(ArduinoConfig.MonitorOn); 
  ArduinoCommands.G_S16  += std::to_string(ArduinoConfig.FilterOn);
  ArduinoCommands.C_SBF  += std::to_string(ArduinoConfig.OutputBufferOn);
  ArduinoCommands.C_SDC  += std::to_string(ArduinoConfig.OutputCoupling);
  ArduinoCommands.C_ST   += std::to_string(ArduinoConfig.ShapingTimeBit0);
  ArduinoCommands.C_ST   += std::to_string(ArduinoConfig.ShapingTimeBit1);
  ArduinoCommands.C_SG   += std::to_string(ArduinoConfig.GainBit0);
  ArduinoCommands.C_SG   += std::to_string(ArduinoConfig.GainBit1);
  ArduinoCommands.C_SNC  += std::to_string(ArduinoConfig.Baseline);
  ArduinoCommands.C_STS  += std::to_string(ArduinoConfig.TestCapacitanceEnabled);
  ArduinoCommands.TPD    += std::to_string(ArduinoConfig.TestPulseDuration);
  ArduinoCommands.TPP    += std::to_string(ArduinoConfig.TestPulsePeriod);

  ArduinoCommands.G_SLK  += " \r";
  ArduinoCommands.G_STB1 += " \r";
  ArduinoCommands.G_STB  += " \r";
  ArduinoCommands.G_S16  += " \r";
  ArduinoCommands.C_SBF  += " \r";
  ArduinoCommands.C_SDC  += " \r";
  ArduinoCommands.C_ST   += " \r";
  ArduinoCommands.C_SG   += " \r";
  ArduinoCommands.C_SNC  += " \r";
  ArduinoCommands.C_STS  += " \r";
  ArduinoCommands.TPD    += " \r";
  ArduinoCommands.TPP    += " \r";

  int err = SendCommand(ArduinoCommands.G_SLK);
  if(!err){err *= SendCommand(ArduinoCommands.G_STB1);}
  if(!err){err *= SendCommand(ArduinoCommands.G_STB);}
  if(!err){err *= SendCommand(ArduinoCommands.G_S16);}
  if(!err){err *= SendCommand(ArduinoCommands.C_SBF);}
  if(!err){err *= SendCommand(ArduinoCommands.C_SDC);}
  if(!err){err *= SendCommand(ArduinoCommands.C_ST);}
  if(!err){err *= SendCommand(ArduinoCommands.C_SG);}
  if(!err){err *= SendCommand(ArduinoCommands.C_SNC);}
  if(!err){err *= SendCommand(ArduinoCommands.C_STS);}
  if(!err){err *= SendCommand(ArduinoCommands.TPD);}
  if(!err){err *= SendCommand(ArduinoCommands.TPP);}
  

  FILE *file;
  file = fopen("/dev/ttyUSB0","w");  //Opening device file
  if(file != NULL){
    std::cout << "Opened port and sending to ASIC" << std::endl;
    usleep(1000*100);
    fprintf(file,"%s","P \r");
    usleep(1000*100);
    fprintf(file,"%s","S \r");
    usleep(1000*100);
    fprintf(file,"%s","T \r");  
    fclose(file);
  }
  else{
    std::cout << "Unable to open file" << std::endl;           
    return -1; 
  }
  

  std::cout << std::endl;
  return err;
};

//Annoyingly can't do this in one function . Buffer issue prehaps.
int DAQ::ArduinoSetup::SendCommand(std::string& Command){

  //Hoping this refreshes now so all commonds send 
  FILE *file;

  file = fopen("/dev/ttyUSB0","w");  //Opening device file
  if(file != NULL){
    std::cout << "Opened port and sending command: " << Command << std::endl;
    fprintf(file,"%s",Command.c_str());
    usleep(1000*100);
    fclose(file); 
  }
  else{
    std::cout << "Unable to open file" << std::endl;
    return -1;
  }
  return 0;

};
