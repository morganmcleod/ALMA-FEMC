/*! \file       owb.h
    \brief      Header for One Wire Bus functions

    <b> File informations: </b><br>
    Created: 2007/09/05 14:33:15 by avaccari

    <b> CVS informations: </b><br>
    \$Id: owb.h,v 1.3 2008/02/16 00:13:08 avaccari Exp $

    This file contains the header for the one wire bus access functions. */

#ifndef _OWB_H
    #define _OWB_H

    /* Defines */
    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

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

    #define ESNS_DATA_FILE  "esns.ini"  // File containing the ESNs data

    #define ESNS_NO_SECTION     "ESNS"      // Section containing the stored ESNs number
    #define ESNS_NO_KEY         "DEVICES"   // Key containing the stored ESNs number
    #define ESNS_NO_EXPECTED    1           // Expected keys in section containing the stored ESNs number

    #define ESNS_DEVICE_SECTION     "ESNS"      // Section Containing the stored ESNs device SN
    #define ESNS_DEVICE_BASE        "ESN"       // Base for ESNs device key name
    #define ESNS_DEVICE_KEY(Dv)     buildString(ESNS_DEVICE_BASE,Dv,NULL) // Key containing the stored ESNs device SN
    #define ESNS_DEVICE_EXPECTED    1           // Expected keys in section containing the stored ESNs device SN

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





#endif /* _OWB_H */
