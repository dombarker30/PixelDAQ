#include "PixelReadout.hh"


DAQ::PixelReadout::PixelReadout(){
  this->InitialiseBoards();
  this->QuitProgram();
}

void DAQ::PixelReadout::InitialiseBoards(){

  CAEN_DGTZ_BoardInfo_t BoardInfo;

  for(int b=0; b<MAXNB; b++){
 
    ret = CAEN_DGTZ_OpenDigitizer(CAEN_DGTZ_USB,0,0,Boards_BaseAddress[b],&handle[b]);
    
    if(ret != CAEN_DGTZ_Success) {
     printf("Can't open digitizer\n");
     this->QuitProgram();
     return;
    }
    
    ret = CAEN_DGTZ_GetInfo(handle[b], &BoardInfo);
    printf("\nConnected to CAEN Digitizer Model %s, recognized as board %d\n", BoardInfo.ModelName, b);
    printf("\tROC FPGA Release is %s\n", BoardInfo.ROC_FirmwareRel);
    printf("\tAMC FPGA Release is %s\n", BoardInfo.AMC_FirmwareRel);
  }
}

void DAQ::PixelReadout::QuitProgram(){

  int c;
  // Free the buffers and close the digitizers                                                                                                                                     
  ret = CAEN_DGTZ_FreeReadoutBuffer(&buffer);
  for(int b=0; b<MAXNB; b++)
    ret = CAEN_DGTZ_CloseDigitizer(handle[b]);
  
  printf("Press 'Enter' key to exit\n");
  c = getchar();
  return;
}
