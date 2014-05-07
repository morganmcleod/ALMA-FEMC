/*! \file   solenoidValve.c
    \brief  Solenoid valve functions

    <b> File informations: </b><br>
    Created: 2007/03/14 17:15:48 by avaccari

    <b> CVS informations: </b><br>
    \$Id: solenoidValve.c,v 1.5 2007/08/09 16:06:01 avaccari Exp $

    This files contains all the functions necessary to handle solenoid valve
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

    /* Check if the backing pump is enabled. If it's not then the electronics to
       monitor and control the solenoid valve is off. In that case, return the
       HARDW_BLKD_ERR and return. */
    if(frontend.
        cryostat.
         backingPump.
          enable[CURRENT_VALUE]==BACKING_PUMP_DISABLE){
        storeError(ERR_SOLENOID_VALVE,
                   0x03); // Error 0x03 -> Backing Pump off -> Solenoid valve disabled
        CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
        return;
    }

    /* Since there is only one submodule in the turbo valve, the check to see if
       the desired submodule is in range, is not neede and we can directly call
       the correct handler. */
    /* Call the correct handler */
    (solenoidValveModulesHandler[currentSolenoidValveModule])();

    return;
}

/* Solenoid valve state handler */
/* This function deals with the messages directed to the solenoid valve
   state. */
static void stateHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   State\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cryostat.
                  solenoidValve.
                   lastState,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status */
        frontend.
         cryostat.
          solenoidValve.
           lastState.
            status=NO_ERROR;

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
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized. */
        memcpy(&CAN_SIZE,
               &frontend.
                 cryostat.
                  solenoidValve.
                   lastState,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

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
                    state[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     solenoidValve.
                      state[CURRENT_VALUE];
        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cryostat.
                            solenoidValve.
                             state[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cryostat.
                            solenoidValve.
                             state[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cryostat.
                                solenoidValve.
                                 state[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cryostat.
                                solenoidValve.
                                 state[HI_ERROR_RANGE])){
                    storeError(ERR_SOLENOID_VALVE,
                               0x01); // Error 0x01 -> Error: Solenoid valve state in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SOLENOID_VALVE,
                               0x02); // Error 0x02 -> Warning: Solenoid valve state in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* If the monitor state return differs from the control register status,
       then the interlock has closed the valve: return a warning. */
    if(CAN_BYTE!=cryoRegisters.
                  bReg.
                   bitField.
                    solenoidValve){
        storeError(ERR_SOLENOID_VALVE,
                   0x04); // Error 0x04 -> Warning: Solenoid valve closed by interlock
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               solenoidValve.
                state[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}

