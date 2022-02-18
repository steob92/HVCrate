# HVCrate

Python wrapper for remote operation of Caen N1470 HV crates. Code only tested on N1470 models, could be generalized to other models.

Assumes CAENHVWrapper (only tested with v6.0) has been installed on the system see [Caen's website](https://www.caen.it/products/caen-hv-wrapper-library/) for download and installation details.


Code used in [Studies of VERITAS photomultipliers after eight years of use](https://ui.adsabs.harvard.edu/abs/2022NIMPA102766235H/abstract). Simple example of powering off two crates:
```python
from pyHVCrate import pyHVCrate

# Create instance
Crate0 = pyHVCrate()
Crate1 = pyHVCrate()

# Connect to crates
Crate0.SetPort("ttyUSB1")
Crate1.SetPort("ttyUSB0")

# Initialize connection
Crate0.InitCrate()
Crate1.InitCrate()

# Output the status of the crate
print ("Crate 0")
Crate0.PrintHVStatus()
print ("Crate 1")
Crate1.PrintHVStatus()

# Power off crates
Crate0.PowerOffAll()
Crate1.PowerOffAll()
```
