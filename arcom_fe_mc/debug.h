/*! \file   debug.h
    \brief  Debug handling header file

    <b> File informations: </b><br>
    Created: 2004/08/31 18:21:05 by avaccari

    <b> CVS informations: </b><br>

    \$Id: debug.h,v 1.48 2013/07/12 20:16:35 mmcleod Exp $

    This files contains all the informations necessary to debug the running
    software.

    This files right now contains only one define (DEBUG) used to turn on
    software debugging features within the various software modules. */

#ifndef _DEBUG_H
    #define _DEBUG_H

    /* Defines */
    // #define DEVELOPMENT         // Uncomment this when developping the software

    /* If we are developing the software */
    #ifdef DEVELOPMENT
        #define CONSOLE                     // Turn on the console interface
        // #define DEBUG_CONSOLE               // Turn on console debugging

        // #define DEBUG_CAN                   // Turn on CAN interface debug
        // #define DEBUG_CAN_FAST              // Uncomment this line to enable the minimalistic CAN message debug in fast version
        
        // #define DEBUG_SERIAL_READ           // Turn on Serial interface read debug
        // #define DEBUG_SERIAL_WRITE          // Turn on Serial interface write debug
                                               //   Note that this mode works as a one-shot only.
                                               //   Set LATCH_DEBUG_SERIAL_WRITE=1 before each call.
        // #define DEBUG_SERIAL_WAIT           // Turn on Serial interface wait debug 

        // #define DEBUG_POWERDIS              // Turn on Power Distribution debug
        // #define DEBUG_IFSWITCH              // Turn on IF Switch debug
        // #define DEBUG_CRYOSTAT              // Turn on Cryostat debug
        #define NO_STOREERROR_CRYOSTAT      // Master switch for cryostat storeError calls.  For debugging with no cryostat
        // #define DEBUG_LPR                   // Turn on LPR debug
        // #define DEBUG_FETIM                 // Turn on FETIM debug
        // #define DEBUG_INI                   // Turn on INI file access debug
        // #define DEBUG_STARTUP               // Turn on startup debugging
        // #define DEBUG_OWB                   // Turn on one wire bus debugging
        // #define DEBUG_PPCOM                 // Turn on the parallel port communication debugging
        // #define DEBUG_MSG_LOOP              // Turn on debugging the main() message loop
        // #define DEBUG_INIT                  // Turn on initialization debugging
        // #define DEBUG                       // Turn on all the rest and error reporting
        #define ERROR_REPORT                // Uncomment this line to enable the console error report

        // #define DEBUG_CRYOSTAT_ASYNC        // Turn on cryotat async debugging
        #define DEBUG_FETIM_ASYNC           // Turn on the FETIM async debugging
        // #define DEBUG_GO_STANDBY2           // Turn on debugging the STANDBY2 transition
    
    #else /* If we are NOT developing: for release build */
        #define CONSOLE                     // Turn on the console interface
    #endif /* DEVELOPMENT */

    /* In either cases if DEBUG_SERIAL is enabled */
    #ifdef DEBUG_SERIAL_READ
        #define DEBUG_BIAS_SERIAL           // Turn of Bias serial interface debug
        #define DEBUG_POWERDIS_SERIAL       // Turn on Power Distribution serial interface debug
        #define DEBUG_IFSWITCH_SERIAL       // Turn on IF Switch serial interface debug
        #define DEBUG_CRYOSTAT_SERIAL       // Turn on Cryostat serial interface debug
        #define DEBUG_ASYNC_CRYOSTAT_SERIAL // Turn on Cryostat asynchronous serial interface debug
        #define DEBUG_LPR_SERIAL            // Turn on LPR serial interface debug
        #define DEBUG_FETIM_SERIAL          // Turn on FETIM serial interface debug
    #endif /* DEBUG_SERIAL */

#endif /* _DEBUG_H */
