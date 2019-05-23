# ALMA-FEMC
## Firmware for the ALMA Front End Monitor and Control (FEMC) module
Written by Andrea Vaccari from approx. 2003 to 2013  https://github.com/avaccari
Maintaned and extended by Morgan McLeod since then https://github.com/morganmcleod

**All this code must be cross-compiled for 16-bit DOS using the Open Watcom IDE**

Makefiles and project files are included for the main FEMC firmware.
Similar may need to be crafted for some of the test sets, which have not been built in 10+ years.

arcom_fe_mc:  contains the FEMC source code<br>
 /3rd party:  a public domain ini file library<br>
 /releases:  the makefile puts its outputs here<br>

arcom_test_sets:  stand-alone test sets for ALMA Front End electronic subsystems<br>
 /FETIM:  the Front End Thermal Interlock Module<br>
 /ifSwitch:  the IF Switch module<br>
 /powerDistribution:  the Cartridge Power Distribution Module (CPDS)<br>
 /Cart: seems to be unfinished or missing some code<br>
 
ini_files:  standard ini files to be loaded on all FEMC modules
