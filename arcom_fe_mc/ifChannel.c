/*! \file   ifChannel.c
    \brief  IF Channel functions

    <b> File informations: </b><br>
    Created: 2006/12/01 13:24:46 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifChannel.c,v 1.9 2010/11/02 14:36:29 avaccari Exp $

    This files contains all the functions necessary to handle IF Channel
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "ifSerialInterface.h"
#include "ifSwitch.h"
#include "debug.h"
#include "database.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
unsigned char   currentIfChannelModule=0;
/* A couple of globals to perform the mapping between IF channels and the
   [polarization][sideband] coordinates. */
unsigned char   currentIfChannelPolarization[IF_CHANNELS_NUMBER]={P0,   // IF Channel 0
                                                                  P0,   // IF Channel 1
                                                                  P1,   // IF Channel 2
                                                                  P1};  // IF Channel 3
unsigned char   currentIfChannelSideband[IF_CHANNELS_NUMBER]={S0,   // IF Channel 0
                                                              S1,   // IF Channel 1
                                                              S0,   // IF Channel 2
                                                              S1};  // IF Channel 3

/* Statics */
static HANDLER ifChannelModulesHandler[IF_CHANNEL_MODULES_NUMBER]={ifTempServoHandler,
                                                                   attenuationHandler,
                                                                   assemblyTempHandler};


/* If channel handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the IF channel. */
void ifChannelHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf("  IF Channel\n");
    #endif /* DEBUG_SWITCH */

    /* Since the IF switch is always outfitted with all the channels, no
       hardware check is performed. */

    /* Check if the submodule is in range */
    currentIfChannelModule=(CAN_ADDRESS&IF_CHANNEL_MODULES_RCA_MASK);
    if(currentIfChannelModule>=IF_CHANNEL_MODULES_NUMBER){
        storeError(ERR_IF_CHANNEL,
                   0x01); // Error 0x01 -> IF channel submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the corrent handler */
    (ifChannelModulesHandler[currentIfChannelModule])();
}

/* IF attenuation handler */
/* This function will deal with monitor and control requests to the IF switch
   channel attenuation. */
void attenuationHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf("   Attenuation\n");
    #endif /* DEBUG_SWITCH */

    /* If control (size!=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   ifSwitch.
                                    ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                             [currentIfChannelSideband[currentIfSwitchModule]].
                                     lastAttenuation)

        /* Since the payload is just a byte, there is no need to conver the
           received data from the can message to any particular format, the
           data is already available in CAN_BYTE. */
        if(checkRange(frontend.
                       ifSwitch.
                        ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                 [currentIfChannelSideband[currentIfSwitchModule]].
                         attenuation[MIN_SET_VALUE],
                      CAN_BYTE,
                      frontend.
                       ifSwitch.
                        ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                 [currentIfChannelSideband[currentIfSwitchModule]].
                         attenuation[MAX_SET_VALUE])){
            storeError(ERR_IF_CHANNEL,
                       0x06); // Error 0x06 -> Attenuation set value out of range

            /* Store error in the last control message variable */
            frontend.
             ifSwitch.
              ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                       [currentIfChannelSideband[currentIfSwitchModule]].
               lastAttenuation.
                status=CON_ERROR_RNG;

            return;
        }

        /* Set the IF channel attenuation. If an error occurs then store the
           state and then return. */
        if(setIfChannelAttenuation()==ERROR){
            /* Store the Error state in the last control message variable */
            frontend.
             ifSwitch.
              ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                       [currentIfChannelSideband[currentIfSwitchModule]].
               lastAttenuation.
                status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     ifSwitch.
                                      ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                               [currentIfChannelSideband[currentIfSwitchModule]].
                                       lastAttenuation)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the IF channel attenuation is changed by a
       control command. */
    CAN_BYTE=frontend.
              ifSwitch.
               ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                        [currentIfChannelSideband[currentIfSwitchModule]].
                attenuation[CURRENT_VALUE];

    CAN_SIZE=CAN_BYTE_SIZE;
}

/* IF assembly temperature handler */
/* This function deals with all the monitor requests directed to the IF switch
   channel assembly temperature. There are no control messages allowed for the
   IF switch channel assembly temperature. */
void assemblyTempHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf("   Assembly Temperature\n");
    #endif /* DEBUG_SWITCH */

    /* Check if the temperature servo is enable. The electronics to read the
       temperature is biased only when the temperature servo is enable. If it
       isn't then return the HARDW_BLKD_ERR and return. This is true only for
       revision 0 of the hardware, for the following revision, the temperature
       is always available. */
    if((frontend.
         ifSwitch.
          ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                   [currentIfChannelSideband[currentIfSwitchModule]].
           ifTempServo.
            enable[CURRENT_VALUE]==IF_TEMP_SERVO_DISABLE)&(frontend.
                                                            ifSwitch.
                                                             hardwRevision==IF_SWITCH_HRDW_REV0)){
        storeError(ERR_IF_CHANNEL,
                   0x07); // Error 0x07 -> the temperature servo is not enabled
        CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
        return;
    }

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_IF_CHANNEL,
                   0x02); // Error 0x02 -> Control message out of range.
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_IF_CHANNEL,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the assembly temperature */
    if((CAN_STATUS=getIfChannelTemp())!=NO_ERROR){
        /* If error during monitoring, the error state was stored in the
           outgoing CAN message state during the previous statement. This
           different format is used because getCryostatTemp might return
           two different error state depending on error conditions. */
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   ifSwitch.
                    ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                             [currentIfChannelSideband[currentIfSwitchModule]].
                     assemblyTemp[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process, gather the stored data */
        CONV_FLOAT=frontend.
                   ifSwitch.
                    ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                             [currentIfChannelSideband[currentIfSwitchModule]].
                     assemblyTemp[CURRENT_VALUE];
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

