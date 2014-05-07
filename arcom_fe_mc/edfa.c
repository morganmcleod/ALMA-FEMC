/*! \file   edfa.c
    \brief  EDFA

    <b> File informations: </b><br>
    Created: 2007/06/02 12:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: edfa.c,v 1.4 2009/10/21 22:24:22 avaccari Exp $

    This file contains all the functions necessary to handle EDFA events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "database.h"
#include "lprSerialInterface.h"

/* Globals */
/* Extern */
unsigned char   currentEdfaModule=0;
/* Statics */
static HANDLER  edfaModulesHandler[EDFA_MODULES_NUMBER]={laserHandler,
                                                         photoDetectorHandler,
                                                         modulationInputHandler,
                                                         driverStateHandler};

/* EDFA handler */
/*! This function will be called by the CAN message handling subrutine when the
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
        storeError(ERR_EDFA,
                   0x01); // Error 0x01 -> EDFA submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (edfaModulesHandler[currentEdfaModule])();
}

/* EDFA Driver State Handler */
/*  This functions deald with the monitor requests to the EDFA driver state. */
static void driverStateHandler(void){

    #ifdef DEBUG_LPR
        printf("   Driver State\n");
    #endif /* DEBUG_LPR */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_EDFA,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_EDFA,
                   0x03); // Error 0x03 -> Monitor message out or range
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
        CAN_BYTE=frontend.
                  lpr.
                   edfa.
                    driverTempAlarm[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=frontend.
                  lpr.
                   edfa.
                    driverTempAlarm[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           lpr.
                            edfa.
                             driverTempAlarm[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           lpr.
                           edfa.
                            driverTempAlarm[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               lpr.
                                edfa.
                                 driverTempAlarm[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               lpr.
                                edfa.
                                 driverTempAlarm[HI_ERROR_RANGE])){
                    storeError(ERR_EDFA,
                               0x04); // Error 0x04 -> Error: EDFA driver state in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_EDFA,
                               0x05); // Error 0x05 -> Warining: EDFA driver state in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* If the alarm is triggered, set the modulation input to 0.0 to prevent
       ON/OFF oscillations. */
    if(CAN_BYTE){
        CONV_FLOAT=0.0;
        if(setModulationInputValue()==ERROR){ // If error, notify the monitor message
            CAN_STATUS=ERROR;
        } else { // If no error, update the modulation input value for future monitor messages
            frontend.
             lpr.
              edfa.
               modulationInput.
                value[CURRENT_VALUE]=CONV_FLOAT;

            CAN_STATUS=MON_ERROR_ACT; // Notify control software that action was taken
        }
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              lpr.
               edfa.
                driverTempAlarm[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


