/*! \file   debug.h
    \brief  Debug handling header file

    <b> File informations: </b><br>
    Created: 2004/08/31 18:21:05 by avaccari

    <b> CVS informations: </b><br>

    \$Id: debug.h,v 1.39 2009/03/23 20:43:04 avaccari Exp $

    This files contains all the informations necessary to debug the running
    software.

    This files right now contains only one define (DEBUG) used to turn on
    software debugging features within the various software modules. */

#ifndef _DEBUG_H
    #define _DEBUG_H

    /* Defines */
    //#define DEVELOPMENT         // Uncomment this when developping the software


    #ifdef DEVELOPMENT
        #define CONSOLE                 // Turn on the console interface
        #define DEBUG_CAN               // Turn on CAN interface debug
        #define DEBUG_SERIAL            // Turn on Serial interface debug
        #define DEBUG_POWERDIS          // Turn on Power Distribution debug
        #define DEBUG_POWERDIS_SERIAL   // Turn on Power Distribution serial interface debug
        #define DEBUG_IFSWITCH          // Turn on IF Switch debug
        #define DEBUG_IFSWITCH_SERIAL   // Turn on IF Switch serial interface debug
        #define DEBUG_CRYOSTAT          // Turn on Cryostat debug
        #define DEBUG_CRYOSTAT_SERIAL   // Turn on Cryostat serial interface debug
        #define DEBUG_LPR               // Turn on LPR debug
        #define DEBUG_LPR_SERIAL        // Turn on LPR serial interface debug
        #define DEBUG_INI               // Turn on INI file access debug
        #define DEBUG_STARTUP           // Turn on startup debugging
        #define DEBUG_OWB               // Turn on one wire bus debugging
        #define DEBUG_PPCOM             // Turn on the parallel port communication debugging
        #define DEBUG_INIT              // Turn on initialization debugging
        #define DEBUG_CONSOLE           // Turn on console debugging
        #define DEBUG                   // Turn on all the rest and error reporting
        #define ERROR_REPORT            // Uncomment this line to enable the console error report
    #endif /* DEVELOPMENT */

    #ifndef DEVELOPMENT
        // Add here the desired level of debugging when in fast mode
        #define CONSOLE                 // Turn on the console interface
        // #define DEBUG_CAN               // Turn on CAN interface debug
        // #define DEBUG_SERIAL            // Turn on Serial interface debug
        // #define DEBUG_POWERDIS          // Turn on Power Distribution debug
        // #define DEBUG_POWERDIS_SERIAL   // Turn on Power Distribution serial interface debug
        // #define DEBUG_IFSWITCH          // Turn on IF Switch debug
        // #define DEBUG_IFSWITCH_SERIAL   // Turn on IF Switch serial interface debug
        // #define DEBUG_CRYOSTAT          // Turn on Cryostat debug
        // #define DEBUG_CRYOSTAT_SERIAL   // Turn on Cryostat serial interface debug
        // #define DEBUG_LPR               // Turn on LPR debug
        // #define DEBUG_LPR_SERIAL        // Turn on LPR serial interface debug
        // #define DEBUG_INI               // Turn on INI file access debug
        // #define DEBUG_STARTUP           // Turn on startup debugging
        // #define DEBUG_OWB               // Turn on one wire bus debugging
        // #define DEBUG_PPCOM             // Turn on the parallel port communication debugging
        // #define DEBUG_INIT              // Turn on initialization debugging
        // #define DEBUG_CONSOLE           // Turn on console debugging
        #define ERROR_REPORT            // Uncomment this line to enable the console error report
        // #define DEBUG_CAN_FAST          // Uncomment this line to enable the minimalistic CAN message debug in fast version
    #endif /* DEVELOPMENT */

#endif /* _DEBUG_H */