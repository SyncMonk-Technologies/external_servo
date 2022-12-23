# External Servo

## Preface
This projects pulls out the native servo from LinuxPTP and uses them as external servo based on slave event monitor channel available in Linux PTP. This basically aims to the clock which acts as frequency jitter attenuator but lacks of TOD and needs to control TOD on other device. 

The user can explicilty configure and control different devices for TOD and frequency based on the hardware design and applicability. This works by using the TLVs as defined in IEEE 1588 TLV_SLAVE_RX_SYNC_TIMING_DATA and user defined TLV SLAVE_DELAY_TIMING_DATA_NP in Linux PTP.

# Acknowledgement
 This project uses the servo , filter and TS processor code from LinuxPTP. Special thanks to Richard Cochran, Miroslav and other LinuxPTP contibutors for the wonderful work done by them.
 
# License
The software is copyrighted by the authors and is licensed under the GNU General Public License. See the file, COPYING, for details of the license terms.

# Installation
  1. Requirement
      - libyaml
  2. make

# Usage
```
  ./ext_servo -f config.yml
```  

