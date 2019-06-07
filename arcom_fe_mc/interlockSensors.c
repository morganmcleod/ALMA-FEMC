/*! \file   interlockSensors.c
    \brief  FETIM Interlock Sensors

    <b> File information: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    This file contains all the functions necessary to handle FETIM interlock
    sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"

/* Globals */
unsigned char   currentInterlockSensorsModule=0;
/* Statics */
static HANDLER interlockSensorsModulesHandler[INTERLOCK_SENSORS_MODULES_NUMBER]={interlockTempHandler,
                                                                                 interlockFlowHandler,
                                                                                 singleFailHandler};

/* Interlock Sensors Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock sensors */
void interlockSensorsHandler(void){
    #ifdef DEBUG_FETIM
        printf("   Sensors\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockSensorsModule=(CAN_ADDRESS&INTERLOCK_SENSORS_MODULES_RCA_MASK)>>INTERLOCK_SENSORS_MODULES_MASK_SHIFT;
    if(currentInterlockSensorsModule>=INTERLOCK_SENSORS_MODULES_NUMBER){
        storeError(ERR_INTRLK_SENS, ERC_MODULE_RANGE); //Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockSensorsModulesHandler[currentInterlockSensorsModule])();

    return;

}

/* Single failure handler */
/* Tins function handles the single failure status for the FETIM interlock
   sensors. */
static void singleFailHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Single Failure\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_SENS, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_SENS, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_SINGLE_FAIL)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    sensors.
                     singleFail;

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    sensors.
                     singleFail;
    }

    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}

