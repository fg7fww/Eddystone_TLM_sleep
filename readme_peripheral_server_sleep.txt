                Peripheral Device with Sleep Mode Sample Code
                =============================================

NOTE: If you use this sample application for your own purposes, follow
      the licensing agreement specified in Software_Use_Agreement.rtf
      in the home directory of the installed RSL10 Evaluation and
      Development Kit (EDK).

Overview
--------
This sample project generates a battery service and a custom service. It
then starts an undirected connectable advertising with the device's public
address, if an address is available at DEVICE_INFO_BLUETOOTH_ADDR in
non-volatile memory three (NVR3). If this address is not defined
(all 1s or 0s), use a pre-defined, private Bluetooth(R) address
(PRIVATE_BDADDR) located in ble_std.h.

When interacting with a device implementing this sample project, any central
device can scan, connect, and perform service discovery, receive battery value
notifications, or read the battery value. The central device has the ability 
to read and write custom attributes. The RSL10 ADC is used to read the battery
level value. The average for 16 reads is calculated, and if this average value 
changes, a flag is set to send a battery level notification.

The Sleep Mode of the device is supported by the Bluetooth low energy
technology library and the system library. In each loop of the main
application, after routine operations (including battery level readings
and service notifications) are performed, if the system can switch to Sleep
Mode, Bluetooth low energy technology configurations and states are saved
and the system is put into Sleep Mode. The system is then woken up by the
Bluetooth low energy technology baseband timer. On waking up, configurations
and states are restored; therefore, the Bluetooth low energy technology
connection with the central device (established before going to Sleep Mode) 
and normal operations of the application are resumed.

This sample project passes through several states before all services are
enabled:

1.  APPM_INIT (initialization)
    Application initializing and is configured into an idle state. 
2.  APPM_CREATE_DB (create database)
    Application has configured the Bluetooth stack, including GAP, according to
    the required role and features. It is now adding services, handlers, and 
    characteristics for all services that it will provide.
3.  APPM_READY (ready)
    Application has added the desired standard and custom services or profiles 
    into the Bluetooth GATT database and handlers.
4.  APPM_ADVERTISING (advertising)
    The device starts advertising based on the sample project.
5.  APPM_CONNECTED (connected)
    Connection is now established with another compatible device.

This sample project is structured as follows:

The source code exists in a "code" folder, and application-related include
header files are in the "include" folder.

Code
----
    app.c         - main()
    app_init.c    - All initialization functions are called here, but the
                    implementation is in the respective c files
    app_process.c - Message handlers for application
    ble_bass.c    - Support functions and message handlers pertaining to the 
                    Battery Service Server
    app_custom.c  - Support functions and message handlers pertaining to the 
                    Custom Service Server
    ble_std.c     - Support functions and message handlers pertaining to
                    Bluetooth low energy technology
    calibration.c - Fetches or calculates trim values and loads them into the
                    appropriate trim registers
    wakeup_asm.S  - Initialization of stack pointer for wakeup function

Headers
-------
    app.h         - Overall application header file
    ble_bass.h    - Header file for the Battery Service Server
    ble_custom.h  - Header file for the Custom Service Server
    ble_std.h     - Header file for standard Bluetooth low energy support
    calibration.h - Header file for the Calibration file

Linker Script
-------------
    sections.ld       - Linker script with custom DRAM size and directives to
                        place sleep and wakeup routines in DRAM. This file is
                        used for full BLE stack.
    sections_light.ld - Linker script with custom DRAM size, custom stack size
                        and directives to place sleep and wakeup routines in
                        DRAM. This file is used for light BLE stack.

Configuration
-------------
The "Debug" and "Release" build targets use the full BLE stack, which keeps
32 KB of memory in retention during Sleep Mode. It also supports 2 Mbps BLE
operation. The application is configured to an advertisement interval of 
40 ms.

The "Debug_Light" and "Release_Light" build targets use the light BLE stack,
which keeps 16 KB of memory in retention during Sleep Mode, allowing a low
Sleep Mode power consumption. The light BLE stack does not support 2 Mbps BLE
operation. The application is configured to an advertisement interval of 5 s
(using the CFG_ADV_INTERVAL_MS=5000 preprocessor define) for very low power
consumption. Due to the long advertisement interval, it may be more difficult
to connect to the device, when there is a lot of interference in the 2.4 GHz
band.

For lower power consumption all build targets use VCC 1.10 V and VDDRF 1.05 V
for nearly 0 dBm output (typically -0.25 dBm). While these voltages are not
stored in the calibration records during manufacturing (NVR4), they are still
supported by the Sys_Power_VCCConfig and Sys_Power_VDDRFConfig system library
functions.

In case an application is build upon the light BLE stack example and more
memory is required, there are some tricks to gain more memory without
increasing the amount of memory kept in retention:
    - The stack can be moved to DRAM1, as it does not need to be retained in
      Sleep Mode. Change sections_light.ld according to the indications 
      inside.
      At the same time, DRAM1_POWER_ENABLE needs to be added to the assignment
      of the sleep_mode_init_env.mem_power_cfg_wakeup variable in the file
      app_process.c. In the .map file, make sure that the __Heap_Begin__
      location is at 0x20001FA8 or below, as the heap needs to be at least 64
      bytes as it is used by the BLE stack and srand/rand functions, and
      0x20001FE8-0x20001FFF is still used for the wakeup from RAM information.
      This change frees up memory with a very slight increase of the power 
      consumption, as an additional DRAM instance is powered during active 
      mode.
    - The sys_powermodes_sleep line can be removed from the sections_light.ld
      file. This will place the Sys_PowerModes_Sleep function in flash.
      At the same time, FLASH_POWER_ENABLE needs to be added to the assignment
      of the sleep_mode_env->mem_power_cfg variable in the file app_process.c.
      This change frees up memory with a very slight increase of the power
      consumption, as the flash remains power-up during the sleep sequence.
When both these tricks have been implemented, the amount of available 
retention RAM with the "Release_Light" build target is around 892 bytes.

Hardware Requirements
---------------------
This application can be executed on any Evaluation and Development Board
with no external connections required.

Importing Project
-----------------
To import the sample code into your IDE workspace, refer to the
Getting Started Guide for your IDE for more information.

Verification
------------
To verify if this application is functioning correctly, use RSL10 or another
third-party central device application to establish a connection. In addition 
to establishing a connection, this application can be used to read/write 
characteristics and receive notifications.

To show how an application can send notifications, the application sends a
custom service notification every 20 sleep-wakeup cycles. Changing the Bluetooth
low energy connection interval or slave latency changes data notification
intervals accordingly. The LED will blink to show sleep-wakeup cycles.

To show that the RF transmission power is set to the desired level (which 
could be 0, 3, or 6 dBm), use a spectrum analyzer to probe the antenna with an
SMA connector. For instance, for a 0 dBm configuration, the spectrum analyzer
should yield a maximum RF power of around 0 dBm.

To show the current consumption of the application, configure the board for an
unregulated external supply, as referred to in the Evaluation and Development 
Board Manual. Connect the board to a DC Power Analyzer and acquire the 
measurement parameters listed below:
    Is - Average Sleep Mode current
    Ts - Sleep Mode time duration
    Ia - Average active current
    Ta - Active time duration
    I  - Overall average current

The overall average current is calculated as follows:
    I = ( (Ts * Is) + (Ia * Ta) ) / (Ts + Ta)

Compare the calculated overall average current against the value in the
datasheet.

Notes
-----
Sometimes the firmware in RSL10 cannot be successfully re-flashed, due to the
application going into Sleep Mode or resetting continuously (either by design 
or due to programming error). To circumvent this scenario, a software recovery
mode using DIO12 can be implemented with the following steps:

1.  Connect DIO12 to ground.
2.  Press the RESET button (this restarts the application, which will
    pause at the start of its initialization routine).
3.  Re-flash RSL10. After successful re-flashing, disconnect DIO12 from
    ground, and press the RESET button so that the application will work
    properly.

==============================================================================
Copyright (c) 2017 Semiconductor Components Industries, LLC
(d/b/a ON Semiconductor).
