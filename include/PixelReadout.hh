#ifndef PixelReadout_hh
#define PixelReadout_hh

#include <stdio.h>
#include "CAENDigitizer.h"

#define CAEN_USE_DIGITIZERS
#define IGNORE_DPP_DEPRECATED

#define MAXNB 1 /* Number of connected boards */

namespace DAQ {
  class PixelReadout;
}


class DAQ::PixelReadout {

public:

  PixelReadout();

private:
  //handler will be used for most of CAENDigitizer functions to identify the board */                                 
  int     handle[MAXNB];
  uint32_t Boards_BaseAddress[MAXNB] {0x32100000};
  CAEN_DGTZ_ErrorCode ret;
  char *buffer = NULL;

  void InitialiseBoards();
  void QuitProgram();

};

#endif /* PixelReadout_hh */
