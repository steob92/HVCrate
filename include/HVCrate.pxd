from libcpp.vector cimport vector
from libcpp cimport bool
from libcpp.string cimport string


from HVCrate cimport HVCrate

cdef extern from "HVCrate.cpp":
    pass

    # Declare the class with cdef
    cdef extern from "HVCrate.h":
        cdef cppclass HVCrate:
        
            HVCrate () except +

            void SetPort(const char *port)
            bool InitCrate()


            bool SetChannelParameter(unsigned short  ch, char *parm, float value)
            
            float GetChannelParameter(unsigned short  ch, char *parm)

            #bool PowerOnChannel(int ch)
            bool ValidParm(char *parm)

            void PrintHVStatus()
            # Ramp up to target volatages
            # Only use the general access version
            # bool RampUp(int nchannels, float *vtarget, int ch = -1)
            bool RampUp(int nchannels, float *vtarget, unsigned short* ch)
            
            # Ramp up to target volatages
            #bool PowerOff(int nchannels, int ch = -1);
            # Power Off Specific Channels
            bool PowerOff(int nchannels, unsigned short* ch)
            
            void PowerOffAll()
