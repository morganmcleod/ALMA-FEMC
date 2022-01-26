/*! \file   backingPump.c
    \brief  Backing pump functions

    <b> File information: </b><br>
    Created: 2007/03/14 17:00:37 by avaccari

    This file contains all the functions necessary to handle backing pump
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
unsigned char   currentBackingPumpModule=0;
/* Statics */
static HANDLER backingPumpModulesHandler[BACKING_PUMP_MODULES_NUMBER]={enableHandler};

/* Backing pump handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the cryostat backing pump. */
void backingPumpHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Backing Pump\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Since the cryostat is always outfitted with the backing pump, no hardware
       check is required. */

    /* Since there is only one submodule in the backing pump, the check to see
       if the desired submodule is in range, is not needed and we can directly
       call the correct handler. */
    (backingPumpModulesHandler[currentBackingPumpModule])();

    return;
}

/* Backing pump enable handler */
/* This function deals with the message directed to the enable state of the
   backing pump in the cryostat module. */
static void enableHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Pump Enable\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE){
         // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cryostat.backingPump.lastEnable)

        /* If turning the backing pump off, then shut down the hardware that is
           biased by the backing pump: turbo, gate and solenoid valve. */
        if(CAN_BYTE==BACKING_PUMP_DISABLE){
            /* Close gate valve: this will not finish before the power is cut
               off, but as soon as the backing pump is restarted, the gate valve
               will finish closing. */
            if(setGateValveState(GATE_VALVE_CLOSE)==ERROR){
                return;
            }
            /* Close solenoid valve */
            if(setSolenoidValveState(SOLENOID_VALVE_CLOSE)==ERROR){
                return;
            }
            /* Turn off turbo pump */
            if(setTurboPumpEnable(TURBO_PUMP_DISABLE)==ERROR){
                return;
            }
        }

        /* Change the status of the backing pump according to the content of the
           CAN message. */
        if(setBackingPumpEnable(CAN_BYTE?BACKING_PUMP_ENABLE:
                                         BACKING_PUMP_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.cryostat.backingPump.lastEnable.status=ERROR;
            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // Return the last control message and status:
        RETURN_LAST_CONTROL_MESSAGE(frontend.cryostat.backingPump.lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the backing pump is changed by a control command. */
    CAN_BYTE=frontend.cryostat.backingPump.enable;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
