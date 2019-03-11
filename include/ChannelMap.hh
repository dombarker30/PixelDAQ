//####################################################\\
//## Title:        ChannelMap.hh                    ##\\
//## Author:       Andrew Scarff                    ##\\
//## Description:  Functions to inisialise the      ##\\
//##               channel map and to return the    ##\\
//##               pixel/ROI channel from a given   ##\\
//##               DAQ channel.                     ##\\
//## Last Updated: 4 Mar 2019                       ##\\
//####################################################\\
 
#ifndef ChannelMap_hh
#define ChannelMap_hh

//C++ Includes
#include <iostream>
#include <map>
#include <string>
#include <iterator>

namespace DAQ{
  namespace ChannelMap{
    std::map<int,std::string> InitChannelMap();
    std::string GetChannelID(int daqChannel, std::map<int,std::string> channelMap);
    std::pair<int,int> GetXYCoords(std::string channelID);
  }
}

#endif //ChannelMap_hh
