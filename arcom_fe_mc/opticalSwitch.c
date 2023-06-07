/*! \file       opticalSwitch.c
    \brief      Optical switch

    <b> File information: </b><br>
    Created: 2007/06/02 15:50:13 by avaccari

    This file contains all the function necessary to handle the optical switch
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "can.h"
#include "frontend.h"
#include "lprSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentOpticalSwitchModule=0;
/* Statics */
static HANDLER  opticalSwitchModulesHandler[OPTICAL_SWITCH_MODULES_NUMBER]={portHandler,
                                                                            shutterHandler,
                                                                            forceShutterHandler,
                                                                            stateHandler,
                                                                            busyHandler};
static LPR *currentLPR;

/* Optical switch handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the optical switch. */
void opticalSwitchHandler(void){
    #ifdef DEBUG_LPR
        printf("  Optical Switch\n");
    #endif /* DEBUG_LPR */

    /* Since the EDFA is always outfitted with the optical switch, no hardware
       check is performed. */

    /* Check if the specified submodule is in range */
    currentOpticalSwitchModule=(CAN_ADDRESS&OPTICAL_SWITCH_MODULES_RCA_MASK)>>OPTICAL_SWITCH_MODULES_MASK_SHIFT;
    if(currentOpticalSwitchModule>=OPTICAL_SWITCH_MODULES_NUMBER){
        storeError(ERR_OPTICAL_SWITCH, ERC_MODULE_RANGE); //Optical switch submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* select which LPR data structure to use */
    currentLPR = (currentModule == LPR2_MODULE) ? &frontend.lpr2 : &frontend.lpr;
    
    /* Call the correct handler */
    (opticalSwitchModulesHandler[currentOpticalSwitchModule])();
}

/* Port handler */
static void portHandler(void){
    #ifdef DEBUG_LPR
        printf("   Port select\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastPort)

        /* Since the payload is just a byte, there is no need to convert the
           received data from the CAN message to any particular format, the
           data is already available in CAN_BYTE. */
        if(checkRange(BAND1, CAN_BYTE, BAND10)) {
            storeError(ERR_OPTICAL_SWITCH, ERC_COMMAND_VAL); //Selected port set value out of range

            /* Store error in the last control message variable */
            currentLPR->opticalSwitch.lastPort.status=CON_ERROR_RNG;
            return;
        }

        /* Set the LPR port. If an error occurs then store the state and
           return. */
        if(setOpticalSwitchPort()==ERROR){
            /* Store the error state in the last control message variable. */
            currentLPR->opticalSwitch.lastPort.status=ERROR;
            return;
        }

        /* If everything went fine modify the current state of the shutter: it's
           not enable anymore since we selected a new channel. This
           automatically removes the shutter so, the effective state should be
           updated. */
        currentLPR->opticalSwitch.shutter=SHUTTER_DISABLE;
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastPort)
        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is updated
       when the state of the optical switch port/shutter is changed by a
       control command. */
    CAN_BYTE=currentLPR->opticalSwitch.port;
    CAN_SIZE=CAN_BYTE_SIZE;
}

/* Shutter handler */
static void shutterHandler(void){

    #ifdef DEBUG_LPR
        printf("   Shutter\n");
    #endif /* DEBUG_LPR */

    /* If it's a control message (size!=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastShutter)

        /* The shutter is enable everytime a message is received independently
           of the payload. */
        if(setOpticalSwitchShutter(STANDARD)==ERROR){
            /* Store the ERROR state in the last control message variable */
            currentLPR->opticalSwitch.lastShutter.status=ERROR;
            return;
        }

        /* If everyting went fine, update the optical switch port to reflect
           the fact that the shutter is enabled. */
        currentLPR->opticalSwitch.port=PORT_SHUTTERED;
        return;
    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastShutter)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the port/shutter is changed by a control command. */
    CAN_BYTE=currentLPR->opticalSwitch.shutter;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

/* Force Shutter handler */
static void forceShutterHandler(void){

    #ifdef DEBUG_LPR
        printf("   Force Shutter\n");
    #endif /* DEBUG_LPR */

    /* If it's a control message (size!=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastForceShutter)

        /* The shutter is enable everytime a message is received independently
           of the payload. */
        if(setOpticalSwitchShutter(FORCED)==ERROR){
            /* Store the ERROR state in the last control message variable */
            currentLPR->opticalSwitch.lastForceShutter.status=ERROR;
            return;
        }

        /* If everyting went fine, update the optical switch port to reflect
           the fact that the shutter is enabled. */
        currentLPR->opticalSwitch.port=PORT_SHUTTERED;
        return;
    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(currentLPR->opticalSwitch.lastForceShutter)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Return error, no monitor messages are allowed on this RCA. */
    storeError(ERR_OPTICAL_SWITCH, ERC_RCA_RANGE); //Monitor message out of range
    CAN_STATUS=MON_CAN_RNG;
}

/* State handler */
static void stateHandler(void){
    #ifdef DEBUG_LPR
        printf("   Switch state\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_OPTICAL_SWITCH, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_OPTICAL_SWITCH, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Get the LPR states */
    if(getLprStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=currentLPR->opticalSwitch.state;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = currentLPR->opticalSwitch.state;
    }
    /* Load the CAN message payload with the returned value and set the state */
    CAN_BYTE=currentLPR->opticalSwitch.state;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

/* Busy handler */
static void busyHandler(void){

    #ifdef DEBUG_LPR
        printf("   Optical switch busy state\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_OPTICAL_SWITCH, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error snce there are no control
       messages allowed on this RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_OPTICAL_SWITCH, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the LPR states */
    if(getLprStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE =currentLPR->opticalSwitch.busy;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=currentLPR->opticalSwitch.busy;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=currentLPR->opticalSwitch.busy;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
