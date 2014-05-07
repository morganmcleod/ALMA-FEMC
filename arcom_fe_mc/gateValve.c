/*! \file   gateValve.c
    \brief  Gate valve functions

    <b> File informations: </b><br>
    Created: 2007/03/14 17:13:45 by avaccari

    <b> CVS informations: </b><br>
    \$Id: gateValve.c,v 1.4 2007/05/21 15:49:26 avaccari Exp $

    This files contains all the functions necessary to handle gate valve
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "globalDefinitions.h"
#include "error.h"
#include "database.h"
#include "cryostatSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentGateValveModule=0;
/* Statics */
static HANDLER gateValveModulesHandler[GATE_VALVE_MODULES_NUMBER]={stateHandler};


/* Gate Valve handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cryostat gate valve. */
void gateValveHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Gate Valve\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Check if the backing pump is enabled. If it's not then the electronics to
       monitor and control the gate valve is off. In that case, return the
       HARDW_BLKD_ERR and return. */
    if(frontend.
        cryostat.
         backingPump.
          enable[CURRENT_VALUE]==BACKING_PUMP_DISABLE){
        storeError(ERR_GATE_VALVE,
                   0x03); // Error 0x03 -> Backing Pump off -> Gate valve disabled
        CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
        return;
    }

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
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cryostat.
                  gateValve.
                   lastState,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status */
        frontend.
         cryostat.
          gateValve.
           lastState.
            status=NO_ERROR;

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
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized. */
        memcpy(&CAN_SIZE,
               &frontend.
                 cryostat.
                  gateValve.
                   lastState,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

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
                    state[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     gateValve.
                      state[CURRENT_VALUE];
        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cryostat.
                            gateValve.
                             state[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cryostat.
                            gateValve.
                             state[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cryostat.
                                gateValve.
                                 state[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cryostat.
                                gateValve.
                                 state[HI_ERROR_RANGE])){
                    storeError(ERR_GATE_VALVE,
                               0x01); // Error 0x01 -> Error: Gate valve state in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_GATE_VALVE,
                               0x02); // Error 0x02 -> Warning: Gate valve state in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               gateValve.
                state[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}
