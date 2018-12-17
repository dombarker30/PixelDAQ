Welcome to the University of Sheffield Pixel DAQ directory. 

# Introduction
---------------------------------------------------------------------------------------------

Currently there is a PixelReadout class. This holds functions to run the CAEN 1710 DAQ. To start the DAQ you have to create a PixelReadoutClass. This will initialise the DAQ and set the the trigger thresholds. Afterwards aquisition will run along with some online analysis.

The DAQDriver module runs the DAQ. After using the make file to create the exectutables you can find the exectutable to run the code in /home/argonshef/CAENDAQ/PixelDAQ/bin/x86_64/Release/DAQDriver.bin. This will take specfic prameters out the DAQConfig.txt in the srcs directory.


# Groups 
-----------------------------------------------------------------------------------------------

The 64 channels in the DAQ are grouped into 8 groups of 8. These groups have to have the same triggers or a collective trigger. Also the groups can be turned on or off using: CAEN_DGTZ_SetGroupEnableMask(handle,mask). The mask is a number that indicates which groups should be turned on or off i.e mask=1 only the first group is used, mask=2 group=2 is used, mmask=3, groups 1 and 2 used and mask=7 groups one two three used. Hence mask=2^n enables just group n+1. Hence mask 2^n -1 enables all groups up to and including n. You can set the mask number in the config param.

For the Trigger Mask the following integers imply the following: 

CAEN_DGTZ_TRGMODE_DISABLED        = 0
CAEN_DGTZ_TRGMODE_EXTOUT_ONLY     = 1 
CAEN_DGTZ_TRGMODE_ACQ_ONLY        = 2
CAEN_DGTZ_TRGMODE_ACQ_AND_EXTOUT  = 3

EXTOUT means the trigger exits the board through the trig out to other boards. 


# Update Information 
------------------------------------------------------------------------------------------------


# Example config file.
------------------------------------------------------------------------------------------------

\################################################

\## This DAQ config is read at the DAQ startup ##

\## Lines starting with a # are ignroned       ##

\################################################

\#####  Global DAQ Parameters ####

MAXNB                           1               #Number of Boards
Readout_size                    1000            #Size of the Waveforms saved
ASIC_gain                       14              #mV/fc
ASIC_shaping_time               2               #mus
Max_events                      2
Boards_BaseAddress              [ 0x32100000 ]
Software_Trigger                1               #


# Contact Information 
------------------------------------------------------------------------------------------------
For more infomation email dominic.barker@hseffield.ac.uk

