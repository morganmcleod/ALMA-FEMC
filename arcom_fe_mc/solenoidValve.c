/*! \file   solenoidValve.c
    \brief  Solenoid valve functions

    <b> File information: </b><br>
    Created: 2007/03/14 17:15:48 by avaccari

    This file contains all the functions necessary to handle solenoid valve
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
unsigned char   currentSolenoidValveModule=0;
/* Statics */
static HANDLER solenoidValveModulesHandler[SOLENOID_VALVE_MODULES_NUMBER]={stateHandler};

/* Solenoid Valve handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cryostat solenoid valve. */
void solenoidValveHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Solenoid Valve\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Call the correct handler -- there is only one for this subsystem */
    (solenoidValveModulesHandler[currentSolenoidValveModule])();

    return;
}

/* Solenoid valve state handler */
/* This function deals with the messages directed to the solenoid valve
   state. */
static void stateHandler(void) {

    #ifdef DEBUG_CRYOSTAT
        printf("   State\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE) {

        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cryostat.
                                    solenoidValve.
                                     lastState)

        /* Check if the backing pump is enabled. If it's not then the electronics to
           control the solenoid valve is off. In that case, return the
           HARDW_BLKD_ERR and return. */
        if(frontend.
            cryostat.
             backingPump.
              enable == BACKING_PUMP_DISABLE) {
            storeError(ERR_SOLENOID_VALVE, ERC_MODULE_POWER); //Backing Pump off -> Solenoid valve disabled
            
            frontend.
             cryostat.
              solenoidValve.
               lastState.
                status=HARDW_BLKD_ERR;
            return;
        }

        /* Change the status of the solenoid valve according to the content of the
           CAN message. */
        if(setSolenoidValveState(CAN_BYTE?SOLENOID_VALVE_OPEN:
                                          SOLENOID_VALVE_CLOSE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cryostat.
              solenoidValve.
               lastState.
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
                                     cryostat.
                                      solenoidValve.
                                       lastState)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Get the solenoid valve state */
    if(getSolenoidValveState()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   solenoidValve.
                    state;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     solenoidValve.
                      state;
    }

    // If the solenoid valve monitor state differs from the control register status..
    if (CAN_BYTE != cryoRegisters.
                     bReg.
                      bitField.
                       solenoidValve)
    { 
        // But the backing pump is enabled..
        if (frontend.
             cryostat.
              backingPump.
               enable == BACKING_PUMP_ENABLE)
        {
            // Report a wanrning that the interlock is in control:
            storeError(ERR_SOLENOID_VALVE, ERC_HARDWARE_BLOCKED); //Warning: Solenoid valve closed by interlock
        }
        // Either way, respond that the hardware is blocked:
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               solenoidValve.
                state;
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}

