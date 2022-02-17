#include "HVCrate.h"


// Default constructor
HVCrate::HVCrate()
{

  // Connection
  fConnString = "_9600_8_1_none_0";
  fPort = 0;
  fHandle = -1;

  // System Type N1470
  fSystem = (CAENHV_SYSTEM_TYPE_t)6;
  // Link type USB VCP
  fLinkType = 5;

  // Number of channels
  // Will worry about daisy-chain later
  fNCh = 4;
  fCh = new ushort[fNCh];


  // Debug status
  fDebug = false;
  
  for (int i = 0; i < fNCh; i++)
    {
      fCh[i] = (ushort)i;
    }



  fParList.push_back("Pw");
  fParList.push_back("ISet");
  fParList.push_back("IMon");
  fParList.push_back("VSet");
  fParList.push_back("VMon");
  fParList.push_back("MaxV");
  
  fNParList = fParList.size();
}




void HVCrate::SetPort(const char *port)
{
  /*
    I don't know why this delete doesn't work...
    if (fPort)
    {
    delete[] fPort;
    }
  */
  fPort = new char[strlen(port) +1];
  strcpy(fPort, port);
}

// Read the port number from a config file
// Expected format "port 0"
void HVCrate::ReadConfigFile(char *filename)
{
  
  int ival;
  FILE* iconfigFile=fopen("./config","r");
  char inbuff[256];
  fscanf(iconfigFile, "%s %d\n", &inbuff[0], &ival);
  fPort[6] = ival + '0';
}

// Write the string for connection to HV Crate
char * HVCrate::WriteConnectionString()
{
  char * iConnection = new char[strlen(fPort)+strlen(fConnString)+1];
  
  // Copy the port to the connection string
  strcpy(iConnection, fPort);
  // Append string with the rest of the connection
  strcat(iConnection, fConnString);

  return iConnection;
}


// INitiate the crate
bool HVCrate::InitCrate()
{
  char *iConnection = WriteConnectionString();

  // Initiate system
  fRet = CAENHV_InitSystem(fSystem,
			  fLinkType,
			  iConnection,
			  "",
			  "",
			   &fHandle);


  // Check if connected ok
  if (fRet != CAENHV_OK)
    {

      /*

	To Do
	Write class with easy to read error messages
      
      */
      cout << "CAENHV_InitSystem: "
	   << CAENHV_GetError(fHandle)
	   << " " << fRet << endl;
      
      return false;
    }

  cout << "Connected Successfully" << endl;
  return true;
  
}


// Check if parameter is valid
bool HVCrate::ValidParm(const char *parm)
{
  for (int i = 0 ; i <fNParList ; i ++)
    {
      if (strcmp (parm, fParList[i]))
	{
	  return true;
	}
    }
  return false;
}

// Get the channel parameters
float HVCrate::GetChannelParameter(unsigned short  ch, const char *parm)
{
  float iparm = -1;
  if ( ValidParm(parm) )
    {
      fRet = CAENHV_GetChParam(fHandle,
			       0,
			       parm,
			       1,
			       &fCh[ch],
			       &iparm);
      if (fRet != CAENHV_OK)
	{
	  
	  cout << "CAENHV_GetChParam: "
	       << CAENHV_GetError(fHandle)
	       << "(num. " << fRet << ")\n" ;
	  return -1;
	}
    }
  else
    {
      cout << "Invalid Parameter" << endl;
    }
 
  return iparm;
}

bool HVCrate::SetChannelParameter(unsigned short  ch, const char *parm, float value)
{

  float pass[1] = {value};
  if ( ValidParm(parm) )
    {
      fRet = CAENHV_SetChParam(fHandle,
			       0,
			       parm,
			       1,
			       &fCh[ch],
			       &pass);

      if (fRet != CAENHV_OK)
	{
	  
	  cout << "CAENHV_SetChParam: "
	       << CAENHV_GetError(fHandle)
	       << "(num. " << fRet << ")\n" ;
	  return false;
	}
      return true;
    }
  else
    {
      cout << "Invalid Parameter" << endl;
      return false;
    }


}




void HVCrate::PrintHVStatus()
{
  float *fvalues = new float[fNParList];
  for (int i = 0; i < fNParList ;  i++)
    {
      cout << fParList[i] << "\t" ;
    }
  cout << endl;

  bool iPower = false;

  for (ushort ch = 0; ch < fNCh; ch++)
    {

      for (int i = 0; i < fNParList ; i++)
	{

	  // IMon gives a read error?
	  // Need to think why...
	  // Maybe we need power to measure the Current?
	  if ( strcmp(fParList[i], "IMon" ) == 0 )
	    {
	      //cout << "IMon," <<  fParList[i] << endl;
	      //if (!iPower)
	      //{
	      cout << "N/A\t";
	      continue;
	      //}
	    }


	  fRet = CAENHV_GetChParam(fHandle,
				   0,
				   fParList[i],
				   1,
				   &fCh[ch],
				   &fvalues[i]);

	  
	  if (fRet != CAENHV_OK)
	    {
	      
	      cout << "CAENHV_GetChParam: "
		   << CAENHV_GetError(fHandle)
		   << "(num. " << fRet << ")\n" ;
	    }
	  
	  // Check the power status
	  if (strcmp (fParList[i] , "Pw")  == 0)
	    {
	      iPower = (bool)fvalues[i];
	      cout << (bool)fvalues[i] << "\t";
	      continue;
	      //cout << "Checking Power : " << fvalues[i] <<  " " << iPower << endl;

	    }
	  
	  cout << fvalues[i] << "\t";
	  //cout << fParList[i] << " : " << fvalues[i] << endl;
	}
      cout << endl;
    }
}




// Ramp up volages
bool HVCrate::RampUp(int nchannels, float *vtarget, int ch )
{
  // Current VMon
  float *VMon = new float[nchannels];
  bool bSet = false;

  // Set the target values
  if (nchannels == 1)
    {
      SetChannelParameter(ch, "Pw", 1);
      SetChannelParameter(ch, "VSet", vtarget[0]);

      while (bSet == false)
	{
	  VMon[0] = GetChannelParameter(ch, "VMon");

	  if (fabs(VMon[0] - vtarget[0]) < 1)
	    {
	      bSet = true;
	    }
	  else
	    {
	      cout << "Channel: " << ch
		   << ", VSet: " << vtarget[0]
		   << ", VMon: " << VMon[0] << endl;
	      
	      sleep(0.1);
	    }
	}
    }

  else
    {
      for (int i = 0; i < nchannels; i++)
	{
	  SetChannelParameter(i, "Pw", 1);
	  SetChannelParameter(i, "VSet", vtarget[i] );
	}

      while (bSet == false)
	{
	  for (int i = 0; i < nchannels; i++)
	    {
	      VMon[i] = GetChannelParameter(i, "VMon");
	      
	      if (fabs(VMon[i] - vtarget[i]) < 1)
		{
		  bSet = true;
		}
	      else
		{
		  bSet = false;
		  cout << "Channel: " << i
		       << ", VSet: " << vtarget[i]
		       << ", VMon: " << VMon[i] << endl;

		  sleep(0.1);
		  continue;
		}
	    }
	}      
    }
  return bSet;
}

// Ramp up volages
bool HVCrate::RampUp(int nchannels, float *vtarget, unsigned short* ch )
{
  // Current VMon
  float *VMon = new float[nchannels];
  bool bSet = false;

  // Set the target values
  if (nchannels == 1)
    {
      SetChannelParameter(ch[0], "Pw", 1);
      SetChannelParameter(ch[0], "VSet", vtarget[0]);

      while (bSet == false)
	{
	  VMon[0] = GetChannelParameter(ch[0], "VMon");

	  if (fabs(VMon[0] - vtarget[0]) < 1)
	    {
	      bSet = true;
	    }
	  else
	    {
	      cout << "Channel: " << ch[0]
		   << ", VSet: " << vtarget[0]
		   << ", VMon: " << VMon[0] << endl;
	      
	      sleep(0.1);
	    }
	}
    }

  else
    {
      for (int i = 0; i < nchannels; i++)
	{
	  SetChannelParameter(ch[i], "Pw", 1);
	  SetChannelParameter(ch[i], "VSet", vtarget[i] );
	}

      while (bSet == false)
	{
	  for (int i = 0; i < nchannels; i++)
	    {
	      VMon[i] = GetChannelParameter(ch[i], "VMon");
	      
	      if (fabs(VMon[i] - vtarget[i]) < 1)
		{
		  bSet = true;
		}
	      else
		{
		  bSet = false;
		  if (fDebug)
		    {
		      cout << "Channel: " << ch[i]
			   << ", VSet: " << vtarget[i]
			   << ", VMon: " << VMon[i] << endl;
		    }
		  sleep(0.1);
		  continue;
		}
	    }
	}      
    }
  return bSet;
}



// Power off channels
bool HVCrate::PowerOff(int nchannels, int ch )
{
  // Current VMon
  float *VMon = new float[nchannels];
  float *vtarget = new float[nchannels];
  bool bSet = false;

  // Set the target values
  if (nchannels == 1)
    {
      vtarget[0] = 0;
      SetChannelParameter(ch, "Pw", 1);
      SetChannelParameter(ch, "VSet", vtarget[0]);

      while (bSet == false)
	{
	  VMon[0] = GetChannelParameter(ch, "VMon");

	  if (fabs(VMon[0] - vtarget[0]) < 10.)
	    {
	      bSet = true;
	    }
	  else
	    {
	      cout << "Channel: " << ch
		   << ", VSet: " << vtarget[0]
		   << ", VMon: " << VMon[0] << endl;
	      
	      sleep(0.1);
	    }
	}
      // Power off channel
      SetChannelParameter(ch, "Pw", 0);
      return bSet;
    }

  else
    {
      for (int i = 0; i < nchannels; i++)
	{
	  vtarget[i] = 0;
	  SetChannelParameter(i, "Pw", 1);
	  SetChannelParameter(i, "VSet", vtarget[i] );
	}
      while (bSet == false)
	{
	  for (int i = 0; i < nchannels; i++)
	    {
	      VMon[i] = GetChannelParameter(i, "VMon");

	      // Slower ramp down
	      // Volage tends to hang at ~1.1V
	      // Send pwr off at <1.5V
	      if (fabs(VMon[i] - vtarget[i]) < 10.)
		{
		  bSet = true;
		}
	      else
		{
		  bSet = false;
		  if (fDebug)
		    {
		      
		      cout << "Channel: " << i
			   << ", VSet: " << vtarget[i]
			   << ", VMon: " << VMon[i] << endl;
		    }
		  // 5 second sleep
		  // Takes longer to power down
		  sleep(5);
		  continue;
		}
	    }
	}

      // Power off channels
      cout << "Turning off Channels" << endl;
      for (int i = 0; i < nchannels; i++)
	{
	  SetChannelParameter(i, "Pw", 0);
	  sleep (1);
	  bool bOff = (bool)GetChannelParameter(i, "Pw");
	  cout << "Ch: " << i << " " << bOff << endl;
	}
      return bSet;

    }  
}

// Power off Specific Channels
bool HVCrate::PowerOff(int nchannels, unsigned short* ch )
{
  // Current VMon
  float *VMon = new float[nchannels];
  float *vtarget = new float[nchannels];
  bool bSet = false;

  // Set the target values
  if (nchannels == 1)
    {
      vtarget[0] = 0;
      SetChannelParameter(ch[0], "Pw", 1);
      SetChannelParameter(ch[0], "VSet", vtarget[0]);

      while (bSet == false)
	{
	  VMon[0] = GetChannelParameter(ch[0], "VMon");

	  if (fabs(VMon[0] - vtarget[0]) < 1)
	    {
	      bSet = true;
	    }
	  else
	    {
	      if (fDebug)
		{
		  cout << "Channel: " << ch[0]
		       << ", VSet: " << vtarget[0]
		       << ", VMon: " << VMon[0] << endl;
		}
	      sleep(1);
	    }
	}
      // Power off channel
      SetChannelParameter(ch[0], "Pw", 0);
      return bSet;
    }

  else
    {
      for (int i = 0; i < nchannels; i++)
	{
	  vtarget[i] = 0;
	  SetChannelParameter(ch[i], "Pw", 1);
	  SetChannelParameter(ch[i], "VSet", vtarget[i] );
	}
      while (bSet == false)
	{
	  for (int i = 0; i < nchannels; i++)
	    {
	      VMon[i] = GetChannelParameter(ch[i], "VMon");

	      // Slower ramp down
	      // Volage tends to hang at ~1.1V
	      // Send pwr off at <1.5V
	      if (fabs(VMon[i] - vtarget[i]) < 1.5)
		{
		  bSet = true;
		}
	      else
		{
		  bSet = false;
		  cout << "Channel: " << ch[i]
		       << ", VSet: " << vtarget[i]
		       << ", VMon: " << VMon[i] << endl;

		  // 5 second sleep
		  // Takes longer to power down
		  sleep(5);
		  continue;
		}
	    }
	}

      // Power off channels
      cout << "Turning off Channels" << endl;
      for (int i = 0; i < nchannels; i++)
	{
	  SetChannelParameter(ch[i], "Pw", 0);
	  sleep (2);
	  bool bOff = (bool)GetChannelParameter(ch[i], "Pw");
	  cout << "Ch: " << ch[i] << " " << bOff << endl;
	}
      return bSet;

    }  
}



void HVCrate::PowerOffAll()
{
  PowerOff(fNCh);
}

// Destructor
HVCrate::~HVCrate()
{

  // Clear vector
  //cout << "Clearing Vector" << endl;

  /*
  for (int i = 0; i < fParList.size(); i++)
    {
      delete fParList[i];
    }
  */
  fParList.clear();
  //cout << "Done" << endl;

  //cout << "Deleting fConnString" << endl;
  //delete fConnString;
  //cout << "Deleting fPort" << endl;
  delete[] fPort;
  //cout << "Deleting fCh" << endl;
  delete[] fCh;
  //cout << "Done" << endl;

  // Shut the connection
  CAENHV_DeinitSystem(fHandle);

}
