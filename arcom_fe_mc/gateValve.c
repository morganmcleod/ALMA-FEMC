/*! \file   gateValve.c
    \brief  Gate valve functions

    <b> File information: </b><br>
    Created: 2007/03/14 17:13:45 by avaccari

    This file contains all the functions necessary to handle gate valve
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "globalDefinitions.h"
#include "error.h"
#include "cryostatSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentGateValveModule=0;
/* Statics */
static HANDLER gateValveModulesHandler[GATE_VALVE_MODULES_NUMBER]={stateHandler};


/* Gate Valve handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the cryostat gate valve. */
void gateValveHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Gate Valve\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Since there is only one submodule in the gate valve, the check to see if
       the desired submodule is in range, is not neede and we can directly call
       the correct handler. */
    /* Call the correct handler */
    (gateValveModulesHandler[currentGateValveModule])();

    return;
}

/* Gate valve state handler */
/* This function deals with the messages directed to the gate valve state. */
static void stateHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   State\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cryostat.
                                    gateValve.
                                     lastState)

        /* Check if the backing pump is enabled. If it's not then the electronics to
           control the gate valve is off. In that case, return the HARDW_BLKD_ERR
           and return. */
        if(frontend.
            cryostat.
             backingPump.
              enable==BACKING_PUMP_DISABLE)
        {
            storeError(ERR_GATE_VALVE, ERC_MODULE_POWER); //Backing Pump off -> Gate valve disabled
            frontend.
             cryostat.
              gateValve.
               lastState.
                status=HARDW_BLKD_ERR; // Store the status in the last control message
            return;
        }

        /* Check the gate valve state. */
        if(getGateValveState()==ERROR){
            /* If error while monitoring, store the status in the last control
               message */
            frontend.
             cryostat.
              gateValve.
               lastState.
                status=ERROR; // Store the status in the last control message

            return;
        }

        /* If the gate valve is still moving, don't do anything an notify user. */
        if(frontend.
            cryostat.
             gateValve.
              state==GATE_VALVE_UNKNOWN){

            storeError(ERR_GATE_VALVE, ERC_HARDWARE_WAIT); //Valve still moving -> Wait unil stopped

            frontend.
             cryostat.
              gateValve.
               lastState.
                status=HARDW_BLKD_ERR; // Store the status in the last control message

            return;
        }

        /* Change the status of the gate valve according to the content of the
           CAN message. */
        if(setGateValveState(CAN_BYTE?GATE_VALVE_OPEN:
                                      GATE_VALVE_CLOSE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cryostat.
              gateValve.
               lastState.
                status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // Return the last control message and status:
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cryostat.
                                      gateValve.
                                       lastState)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Get the gate valve state */
    if(getGateValveState()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   gateValve.
                    state;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     gateValve.
                      state;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               gateValve.
                state;
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}

