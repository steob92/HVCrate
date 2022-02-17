#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vector>
#include "CAENHVWrapper.h"
#include <iostream>
#include <math.h> 
#pragma once
using namespace std;


class HVCrate{

 public:

  bool fDebug;
  HVCrate();
  ~HVCrate();

  void SetPort(const char *port);
  
  // Initiate the connection to the crate
  bool InitCrate();

  bool SetChannelParameter(unsigned short  ch, const char *parm, float value);
  float GetChannelParameter(unsigned short  ch, const char *parm);
  //bool PowerOnChannel(int ch);
  bool ValidParm(const char *parm);

  void PrintHVStatus();
  // Ramp up to target volatages
  bool RampUp(int nchannels, float *vtarget, int ch = -1);
  // RampUp Specific Channels
  bool RampUp(int nchannels, float *vtarget, unsigned short* ch);

  // Ramp up to target volatages
  bool PowerOff(int nchannels, int ch = -1);
 // Power Off Specific Channels
  bool PowerOff(int nchannels, unsigned short* ch);
  
  void PowerOffAll();

 private:

  // Connection details
  char *fConnString;
  char *fPort;
  int fHandle;

  
  // Setup Details
  int fNCh; // number of channels
  int fLinkType; // Connection type
  ushort * fCh; // Array of channels
  
  // Required CAEN objects
  CAENHVRESULT fRet; // Generic return 
  CAENHV_SYSTEM_TYPE_t fSystem; // Systen type 

  // HV Parameters
  int fNParList;
  vector <char*> fParList;
  

  // Write string for connection
  char *WriteConnectionString();

  // Read port from config file
  void ReadConfigFile(char *filename);

  
};




