/*! \file   error.c
    \brief  Error handling functions
    \todo   Here or in the can.c a function to extract error information by a CAN message

    <b> File informations: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: error.c,v 1.72 2008/02/16 00:13:08 avaccari Exp $

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

/* Statics */
static unsigned int  * errorHistory;
static unsigned char errorNewest=0;
static unsigned char errorOldest=0;
static unsigned char errorOn=0;
static unsigned long errorTotal=1;

/*! Initializes the error routines trying to allocate enough space in memory for
    the circular buffer containing the latest 255 errors. If there isn't enough
    space in memory to hold the entire array, the error routines are disabled.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int errorInit(void){

    printf("Initializing Error Library...\n");

    // If error initializing the error array, disable error reporting and notify
    errorHistory=(unsigned int *)malloc(ERROR_HISTORY_LENGTH*sizeof(unsigned int));
    if(errorHistory==NULL){
        errorOn = 0;

        #ifdef ERROR_REPORT
            reportErrorConsole(ERR_ERROR,
                               0x01); // Error 0x01 -> Not enough memory to store the error array
        #endif /* ERROR_REPORT */


        return NO_ERROR;
    }

    // Otherwise enable error reporting
    errorOn=1;

    /* Redirect stderr to avoid message on the screen. */
    if(NULL==freopen(NULL,
                     "r",
                     stderr)){
        #ifdef ERROR_REPORT
            storeError(ERR_ERROR,
                     0x02); // Error 0x02 -> Error turning off stderr
        #endif /* ERROR_REPORT */
    }

    printf("done!\n\n");

    return NO_ERROR;
}


/*! This function performs the operation necessary to stop the error handling
    routine.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int errorStop(void){

    printf("Shutting down error handling...\n");

    // Free allocated memory
    free(errorHistory);

    return NO_ERROR;
}

/*! Critical error handler.

    This function stores the information about the occurred error and then
    shuts down the system since the error was unrecoverable. */
void criticalError(unsigned char moduleNo,
                   unsigned char errorNo){

    /* Call the storeError function */
    storeError(moduleNo,
               errorNo);
    printf("The previous error is unrecoverable. The program will now exit.\nPlease reset the system board before attempting to run the program again.\n");
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
void storeError(unsigned char moduleNo,
                unsigned char errorNo){

    /* Check if the error reporting is turned on */
    if(errorOn){
        ++errorNewest; // Increase the start index

        /* If error buffer overflow, increase the errorOldest to contain the most recent 255 errors */
        if(errorNewest==errorOldest){
            errorOldest++;
        }

        errorHistory[errorNewest]=(((unsigned int)moduleNo)<<8)+((unsigned int)errorNo); // Stores the new unread error
        #ifdef ERROR_REPORT
            reportErrorConsole(moduleNo,
                               errorNo); // Print error on COM1 for debugging purposes
        #endif /* ERROR_REPORT */

/************** This has to go in the readError routine ****************/
        errorOldest++; // Error reported. Point to next oldest.
     }

     errorTotal++; // Increase the global error counter
}

#ifdef ERROR_REPORT
    /* Print error informations on the ARCOM Pegasus console. */
    void reportErrorConsole(unsigned char moduleNo,
                            unsigned char errorNo){

        unsigned char module[45];
        unsigned char error[150];

        /* Print error information on screen */
        switch(moduleNo){
            /* Error library */
            case ERR_ERROR: // Error library
                sprintf(module,
                        "%s",
                        "Error library");
                switch(errorNo){
                    case 0x01: // Not enough memory for the error array
                        sprintf(error,
                                "%s",
                                "Warning: enough memory for error reporting. Error reporting disabled");
                        break;
                    case 0x02: // Error redirecting stderr
                        sprintf(error,
                                "%s",
                                "Warning: error redirecting stderr.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* CPU (AMD ELAN SC520) */
/*            case ERR_CPU: // CPU (AMD ELAN SC520)
                sprintf(module,
                        "CPU (AMD ELAN SC520)");
                switch(errorNo){
                    case 0x01: // MMCR aliasing disables
                        sprintf(error,
                                "Error: The aliasing of MMCR is not enabled. The SSI configuration registers cannot be addressed.");
                        criticalError(MMCR_INITIALIZATION_ERROR); // This error will terminate the program
                        break;
                    case 0x02: // MMCR aliasing out of range
                        sprintf(error,
                                "Error: The aliasing of MMCR is out of the Real Mode range (>1MB)");
                        criticalError(MMCR_INITIALIZATION_ERROR); // This error will terminate the program
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;
*/
            /* Parallel Port */
            case ERR_PP: // Parallel Port
                sprintf(module,
                        "Parallel Port");
                switch(errorNo){
                    case 0x01: // IRQ not enable
                        sprintf(error,
                                "%s%d",
                                "Warning: The IRQ for the parallel port was disable.\nIt was enable and assigned the value:",
                                PP_DEFAULT_IRQ_NO);
                        break;
                    case 0x02: // IRQ out ot range
                        sprintf(error,
                                "Error: The IRQ number for the parallel port is out of range");
                        break;
                    case 0x03: // AMBSI not ready
                        sprintf(error,
                                "Warning: Waiting for AMBSI to get ready for parallel communication");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* CAN */
            case ERR_CAN: // CAN
                sprintf(module,
                        "CAN");
                switch(errorNo){
                    case 0x01: // Required Class outside allowed range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required RCA class (",
                                currentClass,
                                ") is outside the allowed range");
                        break;
                    case 0x02: // Required module outside allowed range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Front End module (",
                                currentModule,
                                ") is outside the allowed range");
                        break;
                    case 0x03: // Monitor RCA out of range
                        sprintf(error,
                                "%s0x%lX%s",
                                "Warning: The RCA of the received monitor message (",
                                CAN_ADDRESS,
                                ") is out of the defined range");
                        break;
                    case 0x04: // Control RCA out of range
                        sprintf(error,
                                "%s0x%lX%s",
                                "Warning: The RCA of the received control message (",
                                CAN_ADDRESS,
                                ") is out of the defined range");
                        break;
                    case 0x05: // Special Monitor RCA out of range
                        sprintf(error,
                                "%s0x%lX%s",
                                "Warning: The RCA of the received special monitor message (",
                                CAN_ADDRESS,
                                ") is out of the defined range");
                        break;
                    case 0x06: // Special Control RCA out of range
                        sprintf(error,
                                "%s0x%lX%s",
                                "Warning: The RCA of the received special control message (",
                                CAN_ADDRESS,
                                ") is out of the defined range");
                        break;
                    case 0x07: // Hardware not accessible during initialization
                        sprintf(error,
                                "%s",
                                "Warning: The Hardware is not available during initialization");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Cartridge */
            case ERR_CARTRIDGE: // Cartridge
                sprintf(module,
                        "Cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Cartridge not installed
                        sprintf(error,
                                "Error: The addressed cartridge is not intalled in the dewar");
                        break;
                    case 0x02: // Cartridge submodule out of Range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed subsystem (",
                                currentCartridgeSubsystem,
                                ") is out of the defined range");
                        break;
                    case 0x03: // LO and cartridge temperature submodule out of Range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed LO and cartrdige temperature module (",
                                currentLoAndTempModule,
                                ") is out of the defined range");
                        break;
                    case 0x04: // cartridge temperature submodule out of Range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Cartrdige temperature module (",
                                currentCartridgeTempSubsystemModule,
                                ") is out of the defined range");
                        break;
                    case 0x05: // BIAS submodule out of Range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed BIAS module (",
                                currentBiasModule,
                                ") is out of the defined range");
                        break;
                    case 0x06: // Cartridge not POWERED
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed cartridge (",
                                currentModule,
                                ") is not powered");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Cartridge Temperature Sensor */
            case ERR_CARTRIDGE_TEMP: // Cartridge Temperature Sensor
                sprintf(module,
                        "Cartridge (%d) Temperature Sensor",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Temperature sensor not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed cartrdige temperature sensor (",
                                currentCartridgeTempModule,
                                ") is not installed");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored cartridge temperature in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored cartridge temperature (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored cartrdige temperature in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored cartrdige temperature (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LO */
            case ERR_LO: // LO
                sprintf(module,
                        "LO in cartridge (%d)",
                        currentModule);
                switch(errorNo){
/*                  case 0x01: // LO module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The LO in thie cartridge is not installed");
                        break; */
                    case 0x02: // LO submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed LO submodule (",
                                currentLoModule,
                                ") is out of range");
                        break;
                    case 0x03: // Warning: The addressed hardware is not properly defined yet
                        sprintf(error,
                                "Warning: The addressed hardware is not properly defined yet. Firmware needs updating.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* PLL */
            case ERR_PLL: // PLL
                sprintf(module,
                        "PLL in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // PLL submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed PLL submodule (",
                                currentPllModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored PLL lock detect voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PLL lock detect voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored PLL lock detect voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PLL lock detect voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored PLL correction voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PLL correction voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored PLL correction voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PLL correction voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x08: // Monitored PLL assembly temperature in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PLL assembly temperature (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x09: // Monitored PLL assembly temperature in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PLL assembly temperature (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0A: // Monitored YIG heater current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored YIG heater current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0B: // Monitored YIG heater current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored YIG heater current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0C: // Monitored PLL reference total power in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PLL reference total power (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0D: // Monitored PLL reference total power in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PLL reference total power (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0E: // Monitored PLL if total power in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PLL if total power (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0F: // Monitored PLL if total power in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PLL if total power (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x10: // Monitored unlock detect latch bit in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored unlock detect latch bit (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x11: // Monitored unlock detect latch bit in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored unlock detect latch bit (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* YTO */
            case ERR_YTO: // YTO
                sprintf(module,
                        "YTO in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // YTO coarse tune out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded YTO coarse tune set point (",
                                convert.
                                 uint[1],
                                ") is out of the allowed range");
                        break;
                    case 0x02: // Monitored YTO coarse tune in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored YTO coarse tune (",
                                CAN_UINT,
                                ") is in the error range");
                        break;
                    case 0x03: // Monitored YTO coarse tune in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored YTO coarse tune (",
                                CAN_UINT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Photomixer */
            case ERR_PHOTOMIXER: // Photomixer
                sprintf(module,
                        "Photomixer in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Photomixer submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Photomixer submodule (",
                                currentPhotomixerModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored photomixer voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored photomixer voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored photomixer voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored photomixer voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored photomixer current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored photomixer current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored photomixer current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored photomixer current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* AMC */
            case ERR_AMC: // AMC
                sprintf(module,
                        "AMC in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // AMC submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed AMC submodule (",
                                currentAmcModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored AMC gate A voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC gate A voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored AMC gate A voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC gate A voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored AMC drain A voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain A voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored AMC drain A voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain A voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x08: // Monitored AMC drain A current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain A current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x09: // Monitored AMC drain A current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain A current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0A: // Monitored AMC gate B voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC gate B voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0B: // Monitored AMC gate B voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC gate B voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0C: // Set AMC drain B voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded AMC drain B voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x0D: // Monitored AMC drain B voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain B voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0E: // Monitored AMC drain B voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain B voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0F: // Monitored AMC drain B current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain B current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x10: // Monitored AMC drain B current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain B current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x11: // Set AMC multiplier D voltage out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The commanded AMC multiplier D voltage set point (",
                                CAN_BYTE,
                                ") is out of the allowed range");
                        break;
                    case 0x12: // Monitored AMC multiplier D current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC multiplier D current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x13: // Monitored AMC multiplier D current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC multiplier D current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x14: // Set AMC gate E voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded AMC gate E voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x15: // Monitored AMC gate E voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC gate E voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x16: // Monitored AMC gate E voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC gate E voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x17: // Set AMC drain E voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded AMC drain E voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x18: // Monitored AMC drain E voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain E voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x19: // Monitored AMC drain E voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain E voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x1A: // Monitored AMC drain E current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC drain E current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x1B: // Monitored AMC drain E current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC drain E current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x1C: // Monitored AMC 3V supply voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC 3V supply voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x1D: // Monitored AMC 3V supply voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC 3V supply voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x1E: // Monitored AMC 5V supply voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored AMC 5V supply voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x1F: // Monitored AMC 5V supply voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored AMC 5V supply voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* PA */
            case ERR_PA: // PA
                sprintf(module,
                        "PA in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // PA submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed PA submodule (",
                                currentPaModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored PA 3V power supply voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PA 3V power supply voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored PA 5V power supply voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PA 5V power supply voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* PA Channel */
            case ERR_PA_CHANNEL: // PA Channel
                sprintf(module,
                        "PA Channel in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // PA Channel submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed PA channel submodule (",
                                currentPaChannelModule,
                                ") is out of range");
                        break;
                    case 0x02: // Set PA channel gate voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded PA gate voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x03: // Monitored PA channel gate voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PA channel gate voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored PA channel gate voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PA channel gate voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x05: // Set PA channel drain voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded PA drain voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x06: // Monitored PA channel drain voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PA channel drain voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored PA channel drain voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PA channel drain voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x08: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x09: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x0A: // Monitored PA channel drain current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored PA channel drain current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0B: // Monitored PA channel drain current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored PA channel drain current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x0C: // Warning: The addressed hardware is not properly defined yet
                        sprintf(error,
                                "Warning: The addressed hardware is not properly defined yet. Firmware needs updating.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Polarization */
            case ERR_POLARIZATION: // Polarization
                sprintf(module,
                        "Polarization in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Polarization module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required polarization (",
                                currentBiasModule,
                                ") is not installed");
                        break;
                    case 0x02: // Polarization submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed polarization submodule (",
                                currentPolarizationModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Sideband */
            case ERR_SIDEBAND: // Sideband
                sprintf(module,
                        "Sideband in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Sideband module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required sideband (",
                                currentPolarizationModule,
                                ") is not installed");
                        break;
                    case 0x02: // Sideband submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed sideband submodule (",
                                currentSidebandModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LNA led */
            case ERR_LNA_LED: // LNA led
                sprintf(module,
                        "LNA led in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // LNA led module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required LNA led (",
                                currentPolarizationModule,
                                ") is not installed");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* SIS Heater */
            case ERR_SIS_HEATER: // SIS Heater
                sprintf(module,
                        "SIS Heater in cartrdige (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // SIS Heater module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required SIS Heater (",
                                currentPolarizationModule,
                                ") is not installed");
                        break;
                    case 0x02: // SIS Heater submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed SIS heater submodule (",
                                currentSisHeaterModule,
                                ") is out of range");
                        break;
                    case 0x03: // Monitored SIS heater current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored SIS heater current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored SIS heater current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored SIS heater current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x05: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x06: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Schottky Mixer */
            case ERR_SCHOTTKY_MIXER: // Schottky Mixer
                sprintf(module,
                        "Schottky Mixer in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Schottky Mixer module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required Schottky Mixer (",
                                currentPolarizationModule,
                                ") is not installed");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* SIS */
            case ERR_SIS: // SIS
                sprintf(module,
                        "SIS in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // SIS module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required SIS (",
                                currentSidebandModule,
                                ") is not installed");
                        break;
                    case 0x02: // SIS submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed SIS submodule (",
                                currentSisModule,
                                ") is out of range");
                        break;
                    case 0x03: // Set SIS voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded SIS voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x04: // Monitored SIS voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored SIS voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored SIS voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored SIS voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored SIS current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored SIS current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored SIS current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored SIS current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x08: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x09: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* SIS Magnet */
            case ERR_SIS_MAGNET: // SIS magnet
                sprintf(module,
                        "SIS magnet in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // SIS magnet module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required SIS magnet (",
                                currentSidebandModule,
                                ") is not installed");
                        break;
                    case 0x02: // SIS magnet submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed SIS magnet submodule (",
                                currentSisMagnetModule,
                                ") is out of range");
                        break;
                    case 0x03: // Monitored SIS Magnet voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored SIS Magnet voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored SIS Magnet voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored SIS Magnet voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x05: // Monitored SIS Magnet current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored SIS Magnet current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x06: // Monitored SIS Magnet current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored SIS Magnet current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x07: // Set SIS Magnet current out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded SIS Magnet current set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x08: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x09: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LNA */
            case ERR_LNA: // LNA
                sprintf(module,
                        "LNA in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // LNA module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required LNA (",
                                currentSidebandModule,
                                ")  is not installed");
                        break;
                    case 0x02: // LNA submodule out of range
                        sprintf(error,
                                "%s%d%s%",
                                "Error: The addressed LNA submodule (",
                                currentLnaModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LNA Stage */
            case ERR_LNA_STAGE: // LNA stage
                sprintf(module,
                        "LNA stage in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // LNA stage module not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The required LNA stage (",
                                currentLnaModule,
                                ") is not installed");
                        break;
                    case 0x02: // LNA stage submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed LNA stage submodule (",
                                currentLnaStageModule,
                                ") is out of range");
                        break;
                    case 0x03: // Monitored LNA stage gate voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored LNA stage gate voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored LNA stage gate voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored LNA stage gate voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x05: // Monitored LNA stage drain voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored LNA stage drain voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x06: // Monitored LNA stage drain voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored LNA stage drain voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x07: // Monitored LNA stage drain current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored LNA stage drain current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x08: // Monitored LNA stage drain current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored LNA stage drain current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x09: // Set LNA stage drain voltage out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded LNA stage drain voltage set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x0A: // Set LNA stage drain current out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded LNA stage drain current set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x10: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x11: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Polarization Special Messages */
            case ERR_POL_SPECIAL_MSGS: // Polarization Special Messages
                sprintf(module,
                        "Polarization Special Messages in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Polarization special messages submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed polarization special messages submodule (",
                                currentPolSpecialMsgsModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Polarization DAC */
            case ERR_POL_DAC: // Polarization DAC
                sprintf(module,
                        "Polarization DAC in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Polarization DAC submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed polarization DAC submodule (",
                                currentPolSpecialMsgsModule,
                                ") is out of range");
                        break;
                    case 0x02: // Polarization DAC submodule doesn't accept clear strobes
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed polarization DAC submodule (",
                                currentPolSpecialMsgsModule,
                                ") doesn't accept clear strobes");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Serial interface */
            case ERR_SERIAL_INTERFACE: // Serial Interface
                sprintf(module,
                        "Serial Interface");
                switch(errorNo){
                    case 0x01: // Selected command out of range
                        sprintf(error,
                                "Error: The selected command word is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Serial Mux Board */
            case ERR_SERIAL_MUX: // Serial Mux Board
                sprintf(module,
                        "Serial Multiplexing Board");
                switch(errorNo){
                    case 0x01: // Data lenght out of range
                        sprintf(error,
                                "Error: The selected data length is out of range");
                        break;
                    case 0x02: // Timeout while waiting for the mux board to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the board to become ready");
                        break;
                    case 0x03: // FPGA is not ready
                        sprintf(error,
                                "Error: The FPGA is not ready for communication");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Timer Module */
            case ERR_TIMER: // Timer Module
                sprintf(module,
                        "Timer Module");
                switch(errorNo){
                    case 0x01: // Async timer already running
                        sprintf(error,
                                "Error: The asynchronous timer is already running");
                        break;
                    case 0x02: // Required async timer out of range
                        sprintf(error,
                                "Error: The required asynchronous timer is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Bias Serial Interface Module */
            case ERR_BIAS_SERIAL: // Bias Serial Interface Module
                sprintf(module,
                        "Bias Serial Interface in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the BIAS ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the BIAS ADC to become ready");
                        break;
                    case 0x02: // Timeout while waiting for the BIAS DAC1 to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the BIAS DAC1 to become ready");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LO Serial Interface Module */
            case ERR_LO_SERIAL: // LO Serial Interface Module
                sprintf(module,
                        "LO Serial Interface in cartridge (%d)",
                        currentModule);
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the LO ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the LO ADC to become ready");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Power Distribution */
            case ERR_POWER_DISTRIBUTION: // Power distribution
                sprintf(module,
                        "Power Distribution");
                switch(errorNo){
                    case 0x01: // Power distribution submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed power distribution submodule (",
                                currentPowerDistributionModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Power Distribution Module */
            case ERR_PD_MODULE: // Power distribution module
                sprintf(module,
                        "Power Distribution Module");
                switch(errorNo){
                    case 0x01: // Power distribution module submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed power distribution module submodule (",
                                currentPdModuleModule,
                                ") is out of range");
                        break;
                    case 0x02: // Corresponding cartridge not installed
                        sprintf(error,
                                "%s%d%s",
                                "Error: The cartridge corresponding to the addressed power distribution module (",
                                currentPowerDistributionModule,
                                ") is not intalled in the dewar");
                        break;
                    case 0x03: // Max number of cartrdiges already on
                        sprintf(error,
                                "%s%d%s",
                                "Error: The maximum allowed number of powered cartrdiges (",
                                frontend.
                                 powerDistribution.
                                  poweredModules[MAX_SET_VALUE],
                                ") is already turned on");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Power Distribution Channel */
            case ERR_PD_CHANNEL: // Power distribution channel
                sprintf(module,
                        "Power Distribution Channel");
                switch(errorNo){
                    case 0x01: // Power distribution channel submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed power distribution channel submodule (",
                                currentPdChannelModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored power distribution channel voltage in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored power distribution channel voltage (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored power distribution channel voltage in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored power distribution channel voltage (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored power distribution channel current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored power distribution channel current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored power distribution channel current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored power distribution current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Power distribution Serial Interface Module */
            case ERR_PD_SERIAL: // PD Serial Interface Module
                sprintf(module,
                        "Power distribution Serial Interface");
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the PD ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the PD ADC to become ready");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* IF Channel */
            case ERR_IF_CHANNEL: // IF Channel
                sprintf(module,
                        "IF Channel");
                switch(errorNo){
                    case 0x01: // IF Channel submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed IF channel submodule (",
                                currentIfChannelModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored IF channel assembly temperature in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored IF channel assembly temperaure (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored IF channel assembly temperature in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored IF channel assembly temperaturet (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Set IF channel attenuation out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The commanded IF channel attenuation set point (",
                                CAN_BYTE,
                                ") is out of the allowed range");
                        break;
                    case 0x07: // Temperature servo OFF
                        sprintf(error,
                                "Error: The temperature servo is OFF: Monitoring of temperature disabled.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* IF Switch */
            case ERR_IF_SWITCH: // IF Switch
                sprintf(module,
                        "IF Switch");
                switch(errorNo){
                    case 0x01: // IF Switch submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed IF Switch submodule (",
                                currentIfSwitchModule,
                                ") is out of range");
                        break;
                    case 0x02: // Set IF switch select band out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The commanded IF switch band select (",
                                CAN_BYTE,
                                ") is out of the allowed range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* IF Switch Serial Interface Module */
            case ERR_IF_SERIAL: // IF Switch Serial Interface Module
                sprintf(module,
                        "IF Switch Serial Interface");
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the IF Switch ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the IF Switch ADC to become ready");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_CRYOSTAT: // Cryostat
                sprintf(module,
                        "Cryostat");
                switch(errorNo){
                    case 0x01: // Cryostat submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Cryostat submodule (",
                                currentCryostatModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored 230V supply current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored 230V supply current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored 230V supply current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored 230V supply current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Warning: Backing pump OFF
                        sprintf(error,
                                "Warning: The backing pump is OFF. Supply current monitoring disabled.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_TURBO_PUMP: // Turbo Pump
                sprintf(module,
                        "Turbo Pump");
                switch(errorNo){
                    case 0x01: // Turbo pump submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Turbo Pump submodule (",
                                currentTurboPumpModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored turbo pump state in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored turbo pump state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored turbo pump state in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored turbo pump state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored turbo pump speed in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored turbo pump speed (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored turbo pump speed in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored turbo pump speed (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x08: // Warning: Backing pump OFF
                        sprintf(error,
                                "Warning: The backing pump is OFF. Turbo pump disabled.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_VACUUM_CONTROLLER: // Vacuum Controller
                sprintf(module,
                        "Vacuum Controller");
                switch(errorNo){
                    case 0x01: // Vacuum Controller submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Vacuum Controller submodule (",
                                currentVacuumControllerModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored vacuum controller state in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored vacuum controller state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored vacuum controller state in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored vacuum controller state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_GATE_VALVE: // Gate Valve
                sprintf(module,
                        "Gate Valve");
                switch(errorNo){
                    case 0x01: // Monitored gate valve state in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored gate valve state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x02: // Monitored gate valve state in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored gate valve state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x03: // Warning: Backing pump OFF
                        sprintf(error,
                                "Warning: The backing pump is OFF. Gate valve disabled.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_SOLENOID_VALVE: // Solenoid Valve
                sprintf(module,
                        "Solenoid Valve");
                switch(errorNo){
                    case 0x01: // Monitored solenoid valve state in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored solenoid valve state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x02: // Monitored solenoid valve state in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored solenoid valve state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x03: // Warning: Backing pump OFF
                        sprintf(error,
                                "Warning: The backing pump is OFF. Solenoid valve disabled.");
                        break;
                    case 0x04: // Solenoid valve controlled by interlock
                        sprintf(error,
                                "Warning: The solenoid valve is controlled by the interlock system and closed.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_VACUUM_SENSOR: // Vacuum Sensor
                sprintf(module,
                        "Vacuum Sensor");
                switch(errorNo){
                    case 0x01: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x02: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x03: // Monitored vacuum sensor pressure in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored vacuum sensor pressure (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored vacuum sensor pressure in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored vacuum sensor pressure (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            case ERR_CRYOSTAT_TEMP: // Cryostat temperature
                sprintf(module,
                        "Cryostat Temperature");
                switch(errorNo){
                    case 0x01: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x02: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x03: // Monitored cryostat temperature in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored cryostat temperature (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored cryostat temperature pressure
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored cryostat temperature (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Cryostat Serial Interface Module */
            case ERR_CRYO_SERIAL: // Cryostat Serial Interface Module
                sprintf(module,
                        "Cryostat Serial Interface");
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the Cryostat ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the Cryostat ADC to become ready");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* EDFA Modulation Input */
            case ERR_MODULATION_INPUT: // EDFA Modulation Input
                sprintf(module,
                        "EDFA modulation Input");
                switch(errorNo){
                    case 0x01: // Set EDFA modulation input value out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded EDFA modulation input value (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x02: // Modulation input submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed modulation input submodule (",
                                currentModulationInputModule,
                                ") is out of range");
                        break;

                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* EDFA Photo Detector */
            case ERR_PHOTO_DETECTOR: // EDFA Photo detector
                sprintf(module,
                        "EDFA photo detector");
                switch(errorNo){
                    case 0x01: // EDFA photo detector submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed EDFA photo detector submodule (",
                                currentPhotoDetectorModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored EDFA photodetector current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored EDFA photodetector current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored EDFA photodetector current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored EDFA photodetector current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Monitored EDFA photodetector power in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored EDFA photodetector power (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x07: // Monitored EDFA photodetector power in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored EDFA photodetector power (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* EDFA laser */
            case ERR_LASER: // EDFA Laser
                sprintf(module,
                        "EDFA Laser");
                switch(errorNo){
                    case 0x01: // EDFA laser submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed EDFA laser submodule (",
                                currentLaserModule,
                                ") is out of range");
                        break;
                    case 0x02: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x03: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x04: // Monitored laser pump temperature in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored pump temperature (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x05: // Monitored laser pump temperature in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored pump temperature (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x06: // Set laser drive current out of range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The commanded laser drive current set point (",
                                CAN_FLOAT,
                                ") is out of the allowed range");
                        break;
                    case 0x07: // Monitored laser drive current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored laser drive current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x08: // Monitored laser drive current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored laser drive current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    case 0x09: // Monitored laser photo detector current in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored laser photo detector current (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x0A: // Monitored laser photo detector current in warning range
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored laser photo detector current (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* EDFA */
            case ERR_EDFA: // EDFA
                sprintf(module,
                        "EDFA");
                switch(errorNo){
                    case 0x01: // EDFA submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed EDFA submodule (",
                                currentEdfaModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Optical Switch */
            case ERR_OPTICAL_SWITCH: // Optical switch
                sprintf(module,
                        "Optical Switch");
                switch(errorNo){
                    case 0x01: // Optical Switch submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed Optical Switch submodule (",
                                currentOpticalSwitchModule,
                                ") is out of range");
                        break;
                    case 0x02: // Set Optical Switch port out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The commanded Optical Switch port (",
                                CAN_BYTE,
                                ") is out of the allowed range");
                        break;
                    case 0x03: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x04: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x05: // Monitored optical switch state in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored optical switch state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x06: // Monitored optical switch state in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored optical switch state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x07: // Monitored optical switch busy state in error range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The monitored optical switch busy state (",
                                CAN_BYTE,
                                ") is in the error range");
                        break;
                    case 0x08: // Monitored optical switch busy state in warning range
                        sprintf(error,
                                "%s%d%s",
                                "Warning: The monitored optical switch busy state (",
                                CAN_BYTE,
                                ") is in the warning range");
                        break;
                    case 0x09: // Optical switch timed out while waiting for busy state during initialization
                        sprintf(error,
                                "Timed out while waiting for ready signal during initialization");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LPR */
            case ERR_LPR: // LPR
                sprintf(module,
                        "LPR");
                switch(errorNo){
                    case 0x01: // LPR submodule out of range
                        sprintf(error,
                                "%s%d%s",
                                "Error: The addressed LPR submodule (",
                                currentLprModule,
                                ") is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LPR temperature */
            case ERR_LPR_TEMP: // LPR temperature
                sprintf(module,
                        "LPR Temperature");
                switch(errorNo){
                    case 0x01: // Control message out of range
                        sprintf(error,
                                "Error: The control message is out of range");
                        break;
                    case 0x02: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    case 0x03: // Monitored LPR temperature in error range
                        sprintf(error,
                                "%s%f%s",
                                "Error: The monitored LPR temperature (",
                                CAN_FLOAT,
                                ") is in the error range");
                        break;
                    case 0x04: // Monitored LPR temperature pressure
                        sprintf(error,
                                "%s%f%s",
                                "Warning: The monitored LPR temperature (",
                                CAN_FLOAT,
                                ") is in the warning range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* LPR Serial Interface Module */
            case ERR_LPR_SERIAL: // LPR Serial Interface Module
                sprintf(module,
                        "LPR Serial Interface");
                switch(errorNo){
                    case 0x01: // Timeout while waiting for the LPR ADC to become ready
                        sprintf(error,
                                "Error: Timeout while waiting for the LPR ADC to become ready");
                        break;
                    case 0x02: // Optical switch busy
                        sprintf(error,
                                "Error: Optical switch busy");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Modulation Input DAC */
            case ERR_MI_DAC: // Modulation Input DAC
                sprintf(module,
                        "Modulation Input DAC");
                switch(errorNo){
                    case 0x01: // Monitor message out of range
                        sprintf(error,
                                "Error: The monitor message is out of range");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* INI file access */
            case ERR_INI: // INI file access
                sprintf(module,
                        "INI file access");
                switch(errorNo){
                    case 0x01: // Data not found
                        sprintf(error,
                                "Error: The required data was not found in the configuration file.");
                        break;
                    case 0x02: // Error opening file
                        sprintf(error,
                                "Error: The was an error while opening the required configuration file.");
                        break;
                    case 0x03: // Number of items returned is wrong
                        sprintf(error,
                                "Error: The number of returned items doesn't match the required ones.");
                        break;
                    case 0x04: // Error handling file
                        sprintf(error,
                                "Error: The was an error while handling the required configuration file.");
                        break;
                    case 0x05: // Error closing file
                        sprintf(error,
                                "Error: The was an error while closing the required configuration file.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* One wire bus */
            case ERR_OWB: // One wire bus
                sprintf(module,
                        "One Wire Bus");
                switch(errorNo){
                    case 0x01: // Time out on IRQ
                        sprintf(error,
                                "Error: Time out waiting for IRQ");
                        break;
                    case 0x02: // Presence pulse not detected
                        sprintf(error,
                                "Error: Presence pulse not detected");
                        break;
                    case 0x03: // Maximum number of devices reached
                        sprintf(error,
                                "Error: Maximum number of devices reached. Check the bus for problems!");
                        break;
                    case 0x04: // Bus reset timed out
                        sprintf(error,
                                "Error: Time out while resetting the bus.");
                        break;
                    default: // Undefined error
                        sprintf(error,
                                "%s%d%s",
                                "The specified error (",
                                errorNo,
                                ") is not defined for this module");
                        break;
                }
                break;

            /* Default */
            default: // Undefined module
                sprintf(module,
                        "%s%d%s",
                        "The specified module (",
                        moduleNo,
                        ") is not defined");
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
               errorHistory[errorNewest],
               (unsigned char)(errorHistory[errorNewest]>>8),
               (unsigned char)errorHistory[errorNewest],
               module,
               error);
    }
#endif /* ERROR_REPORT */


