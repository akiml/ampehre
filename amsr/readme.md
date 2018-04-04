AMSR Linux Kernel Module

The AMSR module extends the original MSR module with the following changes: 

* CAP_SYS_RAWIO capability check is removed
* reading/writing whitelists with allowed registers is added
* dynamic device number is registered
* devnode paths are changed to /dev/cpu/?/amsr
* devnode mode bits are set to 666

Currently there are modules for two kernel versions:

* Kernel version 4.15.4
* CentOS 6.8 Kernel version 2.6.32-642

Modify register whitelists:

* Add allowed registers to the read/write whitelists at the top of the amsr.c file.
* Add register definition if missing.

Licensing information can be found in the source files and in LICENSE_GPL.
