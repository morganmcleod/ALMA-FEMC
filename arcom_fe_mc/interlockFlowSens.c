/*! \file   interlockFlowSens.c
    \brief  FETIM Interlock Flow Sensors

    <b> File information: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    This file contains all the functions necessary to handle FETIM interlock
    flow sensor events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"

/* Globals */
unsigned char   currentInterlockFlowSensModule=0;
/* Statics */
static HANDLER interlockFlowSensModulesHandler[INTERLOCK_FLOW_SENS_MODULES_NUMBER]={flowHandler};

/* Interlock Flow Sensor Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock flow sensor */
void interlockFlowSensHandler(void){
    #ifdef DEBUG_FETIM
    printf("     Flow Sensor: %d\n",
           currentInterlockFlowModule);
    #endif /* DEBUG_FETIM */

    /* Since there is only one submodule in the interlock flow sensor module,
       the check to see if the desired submodule is in range, is not needed and
       we can directly call the correct handler. */
    /* Call the correct handler */
    (interlockFlowSensModulesHandler[currentInterlockFlowSensModule])();

    return;


}

/* Flow sensor handler */
/*! This function is called by the CAN message handler when the received
    message is in the address range of the interlock airflow sensor */
void flowHandler(void){
    #ifdef DEBUG_FETIM
        printf("      Flow\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_FLOW, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_FLOW, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }


    /* Monitor Interlock airflow */
    if(getInterlockFlow()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      sensors.
                       flow.
                        intrlkFlowSens[currentInterlockFlowModule].
                         flow;

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      sensors.
                       flow.
                        intrlkFlowSens[currentInterlockFlowModule].
                         flow;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big enadian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

