/*! \file   error.c
    \brief  Error handling functions
    \todo   Here or in the can.c a function to extract error information by a CAN message

    <b> File informations: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: error.c,v 1.87 2012/01/17 16:30:58 avaccari Exp $

    This file contains the functions necessary to handle the errors that might
    occour during the operation of the ARCOM Pegasus board.*/

/* Includes */
#include <stdlib.h>     /* malloc */
#include <stdio.h>      /* printf */

#include "error.h"
#include "debug.h"
#include "ppComm.h"
#include "frontend.h"
#include "globalOperations.h"

/* Globals */
/* Externs */
unsigned char errorNewest=0; /*!< This variable stores the current index to the
                                  location for the next newest error. */
unsigned char errorOldest=0; /*!< This variable stores the current index to the
                                  location of oldest unread error. */
unsigned int * errorHistory; /*!< This is a pointer to the array that will
                                  contain the error history if the malloc
                                  succeeds. */

/* Statics */
static unsigned int errorNoErrorHistory=1;
static unsigned char errorOn=0;
static unsigned long errorTotal=0;

#ifdef ERROR_REPORT

    static char *moduleNames[0x41] = {
        "Error",                                // 0x00
        "unassigned",
        "Parallel Port",
        "CAN",
        "Cartridge",
        "Cartridge Temperature Sensor",
        "LO",
        "PLL",
        "YTO",                                  // 0x08
        "Photomixer",
        "AMC",
        "PA",
        "PA Channel",
        "Polarization",
        "Sideband",
        "LNA LED",
        "SIS Heater",                           // 0x10
        "unassigned",
        "SIS",
        "SIS Magnet",
        "LNA",
        "LNA Stage",
        "Polarization Special Messages",
        "Polarization DAC",
        "Serial Interface",                     // 0x18
        "Serial Mux Board",
        "Timer",
        "Bias Serial Interface",
        "LO Serial Interface",
        "Power Distribution",
        "PD",
        "PD Channel",
        "PD Serial Interface",                  // 0x20
        "IF Channel",
        "IF Switch",
        "IF Switch Serial Interface",
        "Cryostat",
        "Turbo Pump",
        "Vacuum Controller",
        "Gate Valve",
        "Solenoid Valve",                       // 0x28
        "Vacuum Sensor",
        "Cryostat Temperature",
        "Cryostat Serial Interface",
        "EDFA Modulation Input",
        "EDFA Photo Detector",
        "EDFA Laser Detector",
        "EDFA",
        "LPR Optical Switch",                   // 0x30
        "LPR",
        "LPR Temperature",
        "LPR Serial Interface",
        "Modulation Input DAC",
        "INI File Access",
        "OWB",
        "FETIM",
        "FETIM Interlock",                      // 0x38
        "FETIM Compressor",
        "FETIM Interlock Sensors",
        "FETIM Interlock State",
        "FETIM Interlock Temperature",
        "FETIM Interlock Flow",
        "FETIM Interlock Glitch",
        "FETIM External Temperature",
        "FETIM He2 Pressure"                    // 0x40
    };

#endif // ERROR_REPORT

/*! Initializes the error routines trying to allocate enough space in memory for
    the circular buffer containing the latest 255 errors. If there isn't enough
    space in memory to hold the entire array, the error routines are disabled.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int errorInit(void) {

    #ifdef DEBUG_STARTUP
        printf("Initializing Error Library...\n");
    #endif /* DEBUG_STARTUP */

    // If error initializing the error array, disable error reporting and notify
    errorHistory=(unsigned int *)malloc(ERROR_HISTORY_LENGTH*sizeof(unsigned int));
    if(errorHistory==NULL){
        errorOn = 0;

        /* If there is no error reporting, we need to store that fact in the
           error history so that the can message can read it. */
        errorHistory=&errorNoErrorHistory;
        ++errorNewest;

        #ifdef DEBUG_STARTUP
            printf("\n\nERROR - Not enough memory to store the error array\n\n");
        #endif /* DEBUG_STARTUP */

        #ifdef ERROR_REPORT
            reportErrorConsole(ERR_ERROR, ERC_NO_MEMORY);
        #endif /* ERROR_REPORT */

        return NO_ERROR;
    }

    // Otherwise enable error reporting
    errorOn=1;

    /* Redirect stderr to avoid message on the screen. */
    if(NULL==freopen(NULL,
                     "r",
                     stderr)){
        #ifdef DEBUG_STARTUP
            printf("\n\nWARNING - Fail to redirect stderr\n\n");
        #endif /* DEBUG_STARTUP */

        #ifdef ERROR_REPORT
            storeError(ERR_ERROR, ERC_REDIRECT_STDERR);
        #endif /* ERROR_REPORT */
    }
    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif /* DEBUG_STARTUP */ 
    return NO_ERROR;
}

/*! This function performs the operation necessary to stop the error handling
    routine.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int errorStop(void){

    #ifdef DEBUG_STARTUP
        printf("Shutting down error handling...\n");
    #endif /* DEBUG_STARTUP */ 

    // Free allocated memory
    free(errorHistory);

    #ifdef DEBUG_STARTUP
        printf("done!\n");
    #endif /* DEBUG_STARTUP */ 
    return NO_ERROR;
}

/*! Critical error handler.

    This function stores the information about the occurred error and then
    shuts down the system since the error was unrecoverable. */
void criticalError(unsigned char moduleNo,
                   unsigned char errorNo){

    /* Call the storeError function */
    storeError(moduleNo, errorNo);
    printf("The previous error is unrecoverable. Reboot required.\n");

    //// MAYBE THE BOARD SHOULD REBOOT IN THIS CASE
    //    shutDown(); // Shutdown the system
    exit(1);
}

/*! Stores error informations in the error buffer.

    The stored informations are:
        - Module number (Identifies the module causing the error)
        - Error number (Identifies the particular error)

    This function also calls a function to print the error on the ARCOM Pegasus
    console depending on the existance of the \ref ERROR_REPORT define.

    \param moduleNo     This is the module where the error occured
    \param errorNo      This is the error number for the specified module */
void storeError(unsigned char moduleNo, unsigned char errorNo) {

    /* Increases the total error counter even if the error routine is not enabled */
    errorTotal++;

    /* Check if the error reporting is turned on */
    if(errorOn) {
        /* Stores the error at the current start index */
        errorHistory[errorNewest]=(((unsigned int)moduleNo)<<8)+((unsigned int)errorNo);

        ++errorNewest; // Increase the start index

        /* If error buffer overflow, increase the errorOldest to contain a full
           buffer of the most recent errors */
        if(errorNewest==errorOldest){
            errorOldest++;
        }

        #ifdef ERROR_REPORT
            reportErrorConsole(moduleNo, errorNo);
        #endif /* ERROR_REPORT */
    }
}

#ifdef ERROR_REPORT

    /* Print error informations on the ARCOM Pegasus console. */
    void reportErrorConsole(unsigned char moduleNo, unsigned char errorNo){

        unsigned char *module;
        unsigned char error[150];

        module = moduleNames[moduleNo];

        /* Print error information on screen */
        switch(errorNo) {

            case ERC_NO_MEMORY:         // Not enough memory for the error array
                sprintf(error,
                        "%s",
                        "Warning: not enough memory for error reporting. Error reporting disabled");
                break;

            case ERC_REDIRECT_STDERR:   // Error redirecting stderr
                sprintf(error,
                        "%s",
                        "Warning: error redirecting stderr.");
                break;

            case ERC_IRQ_DISABLED:      // IRQ not enabled
                sprintf(error,
                        "%s%d",
                        "Warning: The IRQ for the parallel port was disabled.\nIt was enable and assigned the value:",
                        PP_DEFAULT_IRQ_NO);
                break;

            case ERC_IRQ_RANGE:         // IRQ out ot range
                sprintf(error,
                        "Error: The IRQ number for the parallel port is out of range");
                break;

            case ERC_AMBSI_WAIT:        // AMBSI not ready
                sprintf(error,
                        "Warning: Waiting for AMBSI to get ready for parallel communication");
                break;

            case ERC_AMBSI_EXPIRED:     // AMBSI ready timer expired
                sprintf(error,
                        "Error: Timer expired while waiting for AMBSI to get ready. CAN disabled! Only console operations available");
                break;

            case ERC_MAINT_MODE:        // Front End in maintenance mode: standard RCAs blocked
                sprintf(error,
                        "The Front End is in Maintenance mode: only the special RCAs are available");
                break;

            case ERC_HARDWARE_TIMEOUT:  // Timed out waiting for hardware to become ready
                sprintf(error,
                        "Timeout waiting for hardware to become ready");
                break;

            case ERC_HARDWARE_ERROR:    // Hardware is in an error state
                sprintf(error,
                        "Module is in an error state");
                break;

            case ERC_HARDWARE_WAIT:     // Waiting for previous command to finish
                sprintf(error,
                        "Waiting for previous command to finish");
                break;

            case ERC_FLASH_ERROR:       //Error reading/writing flash disk
                sprintf(error,
                        "Error accessing flash disk");
                break;

            case ERC_HARDWARE_BLOCKED:  //!< Command blocked by safety check
                sprintf(error,
                        "Command blocked by safety check");
                break;

            case ERC_DEBUG_ME:          //Software entered an invalid or impossible state   
                sprintf(error,
                        "Software entered an invalid or impossible state");
                break;

            case ERC_MODULE_RANGE:      // Sub-module out of range
                sprintf(error,
                        "%s%d%s",
                        "Error: The addressed sub-module (",
                        currentModule,
                        ") is outside the allowed range");
                break;

            case ERC_MODULE_ABSENT:     // Sub-module not installed
                sprintf(error,
                        "%s%d%s",
                        "Error: The addressed sub-module (",
                        currentModule,
                        ") is not installed");
                break;

            case ERC_MODULE_POWER:      // Sub-module not powered
                sprintf(error,
                        "%s%d%s",
                        "Error: The addressed sub-module (",
                        currentModule,
                        ") is powered off");
                break;

            case ERC_RCA_CLASS:         // RCA class out of range
                sprintf(error,
                        "%s0x%lX%s",
                        "Warning: The monitor or command RCA (",
                        CAN_ADDRESS,
                        ") class is out of the defined range");
                break;

            case ERC_RCA_RANGE:         // RCA out of range
                sprintf(error,
                        "%s0x%lX%s",
                        "Warning: The monitor or command RCA (",
                        CAN_ADDRESS,
                        ") is out of the defined range");
                break;

            case ERC_COMMAND_VAL:       // Command value out of range
                sprintf(error,
                        "Error: The command vaue is out of range");
                break;

            case ERC_0E:
            case ERC_0F:
            default:                    // Undefined error
                sprintf(error,
                        "%s%d%s",
                        "The specified error (",
                        errorNo,
                        ") is not defined for this module");
                break;
        }

        printf("\nError %lu (%d/255): 0x%02X (module: %d, error: %d)\n Message from module %s:\n %s\n\n",
               errorTotal,
               errorNewest,
               errorHistory[errorNewest-1],
               (unsigned char)(errorHistory[errorNewest-1]>>8),
               (unsigned char)errorHistory[errorNewest-1],
               module,
               error);
    }
#endif /* ERROR_REPORT */


