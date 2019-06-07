/*! \file       owb.h
    \brief      Header for One Wire Bus functions

    <b> File information: </b><br>
    Created: 2007/09/05 14:33:15 by avaccari

    This file contains the header for the one wire bus access functions. */

#ifndef _OWB_H
    #define _OWB_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define OWB // Define this only if the OWB support is available in the FPGA

    #define MAX_DEVICES_NUMBER  35  // Maximum number of devices to look for

    #define OWB_10_12MHZ    0x86    // Data for clock divisor value to set up for 10-12 MHz clk
    #define OWB_LLM         0x01    // Data for Long Line Mode
    #define OWB_PPM         0x02    // Data for Presence Pulse Masking Mode
    #define OWB_RESET       0x01    // Data for Bus Reset
    #define ACC_SEARCH_MODE 0x02    // Data to enter accelerated search mode

    #define SEARCH_ROM_CODE 0xF0    // One wire bus code to enter search ROM mode

    #define IRQ_PRESENCE_PULSE  0x01    // Presence pulse
    #define IRQ_RX_BUF_FULL     0x10    // Receive buffer full

    #define PRESENCE_PULSE_MASK 0x02    // Presence pulse mask

    #define SEARCH_BYTES_LENGTH 16      // Lenght in bytes of the search

    /* Dallas Maxim Chips Defines */

    /* Family codes */
    #define FAMILY_DS18S20  0x10
    #define FAMILY_DS2433   0x23

    /* ROM commands */
    #define SEARCH_ROM          0xF0
    #define READ_ROM            0x33
    #define MATCH_ROM           0x55
    #define SKIP_ROM            0xCC
    #define ALARM_SEARCH        0xEC
    #define OVERDRIVE_SKIP_ROM  0x3C
    #define OVERDRIVE_MATCH_ROM 0x69

    /* DS18S20 commands */
    #define DS18S20_CONVERT_T           0x44
    #define DS18S20_READ_SCRATCHPAD     0xBE
    #define DS18S20_WRITE_SCRATCHPAD    0x4E
    #define DS18S20_COPY_SCRATCHPAD     0x48
    #define DS18S20_RECALL_E2           0xB8
    #define DS18S20_READ_POWER_SUPPLY   0xB4

    /* DS2433 commands */
    #define DS2433_WRITE_SCRATCHPAD     0x0F
    #define DS2433_READ_SCRATCHPAD      0xAA
    #define DS2433_COPY_SCRATCHPAD      0x55
    #define DS2433_READ_MEMORY          0xF0


    /* Globals */
    /* Externs */
    extern unsigned char esnDevicesFound; //!< Number of devices with ESN found on the bus
    extern unsigned char ESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE]; // Array to store the ESNs of the found devices

    /* Prototypes */
    /* Statics */
    static int owbReset(void); // Send the reset signal on the one wire bus
    static int waitIrq(unsigned char irq); // Check on the state of the selected irq signal
    static int RecoverROM(int* ReceiveData, int* TransmitData, unsigned char* ROMCode); // Algorithm to discover the available devices
    static int writeOwb(int data); // Writes data to the one wire bus
    /* Externs */
    extern int owbInit(void); //!< Performs the initialization of the one wire bus
    extern int owbGetEsn(void); //!< Gather the available ESN from the OWB

#endif /* _OWB_H */
