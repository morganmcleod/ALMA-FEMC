/*! \file   ifSwitch.c
    \brief  IF Switch functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifSwitch.c,v 1.10 2010/11/02 14:36:29 avaccari Exp $

    This files contains all the functions necessary to handle IF Switch
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "ifSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentIfSwitchModule=0;
/* An extern to perform the mapping between bands and IF switch way. */
unsigned char   currentIfSwitchWay[IF_SWITCH_WAYS]={WAY0,   // Band 1
                                                    WAY1,   // Band 2
                                                    WAY2,   // Band 3
                                                    WAY3,   // Band 4
                                                    WAY4,   // Band 5
                                                    WAY5,   // Band 6
                                                    WAY6,   // Band 7
                                                    WAY7,   // Band 8
                                                    WAY8,   // Band 9
                                                    WAY9};  // Band 10
/* Statics */
static HANDLER  ifSwitchModulesHandler[IF_SWITCH_MODULES_NUMBER]={ifChannelHandler,
                                                                  ifChannelHandler,
                                                                  ifChannelHandler,
                                                                  ifChannelHandler,
                                                                  bandSelectHandler};

/* IF Switch handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the IF Switch. */
void ifSwitchHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf(" IF Switch\n");
    #endif /* DEBUG_IFSWITCH */

    /* Since the IF Switch is always installed in the frontend., no hardware
       check is performed. */

    /* Check if the submodule is in range */
    currentIfSwitchModule=(CAN_ADDRESS&IF_SWITCH_MODULES_RCA_MASK)>>IF_SWITCH_MODULES_MASK_SHIFT;
    if(currentIfSwitchModule>=IF_SWITCH_MODULES_NUMBER){
        storeError(ERR_IF_SWITCH,
                   0x01); // Error 0x01 -> IF Switch submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct handler */
    (ifSwitchModulesHandler[currentIfSwitchModule])();
}


/* Band select handler */
void bandSelectHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf("  Band Select\n");
    #endif /* DEBUG_IFSWITCH */

    /* If control (size!=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 ifSwitch.
                  lastBandSelect,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         ifSwitch.
          lastBandSelect.
           status=NO_ERROR;

        /* Since the payload is just a byte, there is no need to conver the
           received data from the can message to any particular format, the
           data is already available in CAN_BYTE. */
        if(checkRange(frontend.
                       ifSwitch.
                        bandSelect[MIN_SET_VALUE],
                      CAN_BYTE,
                      frontend.
                       ifSwitch.
                       bandSelect[MAX_SET_VALUE])){
            storeError(ERR_IF_SWITCH,
                       0x02); // Error 0x02 -> Selected band set value out of range

            /* Store error in the last control message variable */
            frontend.
             ifSwitch.
              lastBandSelect.
               status=CON_ERROR_RNG;

            return;
        }

        /* Set the IF switch band. If an error occurs then store the state and
           return. */
        if(setIfSwitchBandSelect()==ERROR){
            /* Store the error state in the last control message variable */
            frontend.
             ifSwitch.
              lastBandSelect.
               status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        /* Retrun last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 ifSwitch.
                  lastBandSelect,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the IF switch select band is changed by a
       control command. */
    CAN_BYTE=frontend.
              ifSwitch.
               bandSelect[CURRENT_VALUE];

    CAN_SIZE=CAN_BYTE_SIZE;
}

/* IF Switch initialization */
/*! This function performs all the necessary initialization for the IF switch
    system. These are executed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int ifSwitchStartup(void){

    /* Set the currentModule variable to reflect the fact that the IF switch is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=IF_SWITCH_MODULE;

    printf(" Initializing IF Switch Module...\n");
    printf("  - Reading IF switch M&C module hardware revision level...\n");

    /* Call the getIfSwitchHadrwRevision() function to read the hardware
       revision level. If error, return error and abort initialization. */
    if(getIfSwitchHardwRevision()==ERROR){
        return ERROR;
    }

    printf("     Revision level: %d\n",
           frontend.
            ifSwitch.
             hardwRevision);

    printf("    done!\n"); // Hardware Revision Level

    /* Set the limits for control messages */
    printf("  - Setting limits for control messages\n");
    printf("    - Band select\n");
    frontend.
     ifSwitch.
      bandSelect[MIN_SET_VALUE]=BAND1;
    frontend.
     ifSwitch.
      bandSelect[MAX_SET_VALUE]=BAND10;
    printf("      done!\n"); // Band select


    printf("    - Channel attenuation\n");
    printf("      - Channel 1\n"); // The channel identification here has no meaning it's only to print out a message
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION0][SIDEBAND0].
       attenuation[MIN_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MIN;
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION0][SIDEBAND0].
       attenuation[MAX_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MAX;
    printf("        done!\n"); // Channel 1

    printf("      - Channel 2\n"); // The channel identification here has no meaning it's only to print out a message
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION0][SIDEBAND1].
       attenuation[MIN_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MIN;
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION0][SIDEBAND1].
       attenuation[MAX_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MAX;
    printf("        done!\n"); // Channel 2

    printf("      - Channel 3\n"); // The channel identification here has no meaning it's only to print out a message
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION1][SIDEBAND0].
       attenuation[MIN_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MIN;
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION1][SIDEBAND0].
       attenuation[MAX_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MAX;
    printf("        done!\n"); // Channel 3

    printf("      - Channel 4\n"); // The channel identification here has no meaning it's only to print out a message
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION1][SIDEBAND1].
       attenuation[MIN_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MIN;
    frontend.
     ifSwitch.
      ifChannel[POLARIZATION1][SIDEBAND1].
       attenuation[MAX_SET_VALUE]=IF_CHANNEL_SET_ATTENUATION_MAX;
    printf("        done!\n"); // Channel 4

    printf("      done!\n"); // Channel Attenuation

    printf("    done!\n"); // Control message limits

    printf(" done!\n\n"); // Initialization

    return NO_ERROR;
}

