/*! \file   ifTempServo.c
    \brief  IF switch temperature servo functions

    <b> File informations: </b><br>
    Created: 2006/11/30 14:49:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifTempServo.c,v 1.4 2007/05/21 15:49:26 avaccari Exp $

    This files contains all the functions necessary to handle IF switch
    temperature servo events. */

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
unsigned char   currentIfTempServoModule=0;
/* Statics */
static HANDLER ifTempServoModulesHandler[IF_TEMP_SERVO_MODULES_NUMBER]={enableHandler};

/* IF switch temperature servo handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the IF switch temperature servo system. */
void ifTempServoHandler(void){
    /* The value of currentIfTempServoModule is not changed since there is only
       one submodule in the IF temperature servo module.
       Ths structure is preserved only for consistency.
       If the timing should be an issue, it can be removed and the functionality
       can be placed directly in the ifTempServoHandler function. */
    #ifdef DEBUG_IFSWITCH
        printf("  Temperature Servo\n");
    #endif /* DEBUG_IFSWITCH */

    /* Since the IF switch is always outfitted with all the modules, no hardware
       check is performed. */

    /* Since the is only one submodule in the temperature servo, the check to
       see if the desired submodule is in range, is not needed and we can
       directly call the correct handler. */
    (ifTempServoModulesHandler[currentIfTempServoModule])();
}

/* IF switch temperature servo enable handler */
/* This function deals with the monitor and control requests to the IF switch
   temperature servo system enable. */
static void enableHandler(void){

    #ifdef DEBUG_IFSWITCH
        printf("   Servo enable\n");
    #endif /* DEBUG_SWITCH */

    /* If control (size!=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   ifSwitch.
                                    ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                             [currentIfChannelSideband[currentIfSwitchModule]].
                                     ifTempServo.
                                      lastEnable)

        /* Check that the CAN_BYTE is a legal value for enable/disable */
        if(CAN_BYTE!=IF_TEMP_SERVO_ENABLE && CAN_BYTE!=IF_TEMP_SERVO_DISABLE){
            storeError(ERR_IF_CHANNEL,
                       0x08); // Error 0x08 -> Bad command for servo enable/disable
            /* Store the ERROR state in the last control message variable */
            frontend.
             ifSwitch.
              ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                       [currentIfChannelSideband[currentIfSwitchModule]].
               ifTempServo.
                lastEnable.
                 status=ERROR;
            return;
        }
        
        /* Change the status of the temperature servo according to the content
           of the CAN message. */
        if(setIfTempServoEnable(CAN_BYTE?IF_TEMP_SERVO_ENABLE:
                                         IF_TEMP_SERVO_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             ifSwitch.
              ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                       [currentIfChannelSideband[currentIfSwitchModule]].
               ifTempServo.
                lastEnable.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     ifSwitch.
                                      ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                                               [currentIfChannelSideband[currentIfSwitchModule]].
                                       ifTempServo.
                                        lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memoryu status is updated
       when the state of the servo is changed by a control command. */
    CAN_BYTE=frontend.
              ifSwitch.
               ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                        [currentIfChannelSideband[currentIfSwitchModule]].
                ifTempServo.
                 enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


