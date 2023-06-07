/*! \file   edfa.c
    \brief  EDFA

    <b> File information: </b><br>
    Created: 2007/06/02 12:34:50 by avaccari

    This file contains all the functions necessary to handle EDFA events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "lprSerialInterface.h"

/* Globals */
/* Extern */
unsigned char   currentEdfaModule=0;
/* Statics */
static HANDLER  edfaModulesHandler[EDFA_MODULES_NUMBER]={laserHandler,
                                                         photoDetectorHandler,
                                                         modulationInputHandler,
                                                         driverStateHandler};

static LPR *currentLPR;

/* EDFA handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the EDFA. */
void edfaHandler(void){

    #ifdef DEBUG_LPR
        printf("  EDFA\n");
    #endif /* DEBUG_LPR */

    /* Since the LPR is always outfitted with the EDFA, no hardware check is
       performed. */

    /* Check if the specified submodule is in range. */
    currentEdfaModule=(CAN_ADDRESS&EDFA_MODULES_RCA_MASK)>>EDFA_MODULES_MASK_SHIFT;
    if(currentEdfaModule>=EDFA_MODULES_NUMBER){
        storeError(ERR_EDFA, ERC_MODULE_RANGE); //EDFA submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* select which LPR data structure to use */
    currentLPR = (currentModule == LPR2_MODULE) ? &frontend.lpr2 : &frontend.lpr;

    /* Call the correct handler */
    (edfaModulesHandler[currentEdfaModule])();
}

/* EDFA Driver State Handler */
/* This functions deals with the monitor requests to the EDFA driver state. */
static void driverStateHandler(void){

    #ifdef DEBUG_LPR
        printf("   Driver State\n");
    #endif /* DEBUG_LPR */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_EDFA, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_EDFA, ERC_RCA_RANGE); //Monitor message out or range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Get LPR driver state */
    if(getLprStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=currentLPR->edfa.driverTempAlarm;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=currentLPR->edfa.driverTempAlarm;
    }

    /* If the alarm is triggered, set the modulation input to 0.0 to prevent
       ON/OFF oscillations. */
    if(CAN_BYTE) {
        CONV_FLOAT=0.0;
        if(setModulationInputValue()==ERROR){ // If error, notify the monitor message
            CAN_STATUS=ERROR;
        } else { // If no error, update the modulation input value for future monitor messages
            currentLPR->edfa.modulationInput.value=CONV_FLOAT;
            CAN_STATUS=MON_ERROR_ACT; // Notify control software that action was taken
        }
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=currentLPR->edfa.driverTempAlarm;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


