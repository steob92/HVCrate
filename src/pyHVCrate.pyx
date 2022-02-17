# distutils: language = c++
# distutils: sources = HVCrate.h, HVCrate.cpp
# distutils: include_dirs = ./, ../include


from HVCrate cimport HVCrate

# python side of things
import numpy as np
#from cython ushort as ushrt
from libcpp.vector cimport vector
from libcpp.string cimport string
#from libcpp.ushort cimport ushort

from cpython cimport array
import array


# Class we will use from python
cdef class pyHVCrate:

    
    cdef HVCrate *_hvcrate

    # C++ initialization
    def __cinit__(self):
        self._hvcrate = new HVCrate()

        
    def __dealloc__(self):
        del self._hvcrate

    
    # Set the port
    def SetPort(self, port):
        
        cdef string cport = port
        self._hvcrate.SetPort(cport.c_str())

    # Initiate the connection to the crate
    def InitCrate(self):
        binit = bool(self._hvcrate.InitCrate())

    
    def SetChannelParameter(self, ch, parm, value):
        cdef int c_ch = 0
        cdef string c_parm = parm
        cdef float c_value = 0
        c_ch = ch
        c_value = value
        
        self._hvcrate.SetChannelParameter( c_ch, c_parm.c_str(), c_value)


    def GetChannelParameter(self, ch, parm):
        cdef int c_ch = ch
        cdef string c_parm = parm
        
        return float(self._hvcrate.GetChannelParameter(c_ch,
                                                       c_parm.c_str()))

    #def PowerOnChannel(self, ch):
    #    return bool(self._hvcrate.PowerOnChannel(ch))

    def RampUp(self, channels, vtargets):

        cdef int nchannels = len(channels)
        # Create arb large array length
        cdef unsigned short c_chans[10]
        cdef float c_vtargets[10]
        
        
        
        for i in range(nchannels):
            c_chans[i] = channels[i]
            c_vtargets[i] = vtargets[i]
              
        bramp =  bool(self._hvcrate.RampUp(nchannels,
                                           c_vtargets,
                                           c_chans))


        #del c_chans
        #del c_vtargets
        return bramp

    
    def PowerOff(self, channels):
        cdef int nchannels = len(channels)
        cdef unsigned short c_chans[10]
        for i in range(channels):
            c_chans[i] = channels[i]
            
        self._hvcrate.PowerOff(nchannels, c_chans)


    def PowerOffAll(self):
        self._hvcrate.PowerOffAll()

    def PrintHVStatus(self):
        self._hvcrate.PrintHVStatus()
        
