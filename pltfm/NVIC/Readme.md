# PRIORITY CONFIGURATION

Aiming of this section is to configure the priority of all interrupts registered by users 

## How does the priority of interrupt work

The thorough overview about interrupt priorities setup is well described in [Cortex-M3 reference manual]

(Check more in Priority levels: 5.3.1)
[Find the link For more infomation] 

## About this section

1. APIs setting up priorities are provided by Cortex_M3 provider.  This section creates a wrapper to call those APIs
2. This section sets up a static priority level table, allow user to configure the priorities of all registered interrupt when the device is powered up 
3. Besides, user shall be able to setup the priority of each interrupt dynamically via provided wrappers
