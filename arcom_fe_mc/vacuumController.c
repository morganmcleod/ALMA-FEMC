/*! \file   vacuumController.c
    \brief  Vacuum controller functions

    <b> File informations: </b><br>
    Created: 2007/03/14 17:17:31 by avaccari

    <b> CVS informations: </b><br>
    \$Id: vacuumController.c,v 1.5 2007/05/21 15:49:27 avaccari Exp $

    This files contains all the functions necessary to handle vacuum controller
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
unsigned char   currentVacuumControllerModule=0;
/* Statics */
static HANDLER  vacuumControllerModulesHandler[VACUUM_CONTROLLER_MODULES_NUMBER]={vacuumSensorHandler,
                                                                                  vacuumSensorHandler,
                                                                                  enableHandler,
                                                                                  stateHandler};

/* Vacuum controller handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cryostat vacuum controller. */
void vacuumControllerHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Vacuum Controller\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Since the cryostat is always outfitted with the vacuum controller, no
       hardware check is required. */

    /* Check if the submodule is in range */
    currentVacuumControllerModule=(CAN_ADDRESS&VACUUM_CONTROLLER_MODULES_RCA_MASK);
    if(currentVacuumControllerModule>=VACUUM_CONTROLLER_MODULES_NUMBER){
        storeError(ERR_VACUUM_CONTROLLER,
                   0x01); // Error 0x01 -> Vacuum Controller submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (vacuumControllerModulesHandler[currentVacuumControllerModule])();

    return;
}

/* Vacuum Controlller Enable Handler */
/* This function deals with the monitor and control requests to the vacuum
   controller enable. */
static void enableHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Controller enable\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cryostat.
                  vacuumController.
                   lastEnable,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cryostat.
          vacuumController.
           lastEnable.
            status=NO_ERROR;

        /* Change the status of the vacuum controller according to the content of
          the CAN message. */
        if(setVacuumControllerEnable(CAN_BYTE?VACUUM_CONTROLLER_ENABLE:
                                              VACUUM_CONTROLLER_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cryostat.
              vacuumController.
               lastEnable.
                status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also the
          state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
          initialized. */
        memcpy(&CAN_SIZE,
               &frontend.
                 cryostat.
                  vacuumController.
                   lastEnable,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the vacuum controller is changed by a control command. */
    CAN_BYTE=frontend.
              cryostat.
               vacuumController.
                enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

/* Vacuum Controller State Handler */
/* This function deals with the monitor and control requests to the vacuum
   controller state. */
static void stateHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Controller state\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_VACUUM_CONTROLLER,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_VACUUM_CONTROLLER,
                   0x03); // Error 0x03 -> Monitor message out or range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Get Vacuum controller state */
    if(getVacuumControllerState()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   vacuumController.
                    state[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     vacuumController.
                      state[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cryostat.
                            vacuumController.
                             state[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cryostat.
                            vacuumController.
                             state[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cryostat.
                                vacuumController.
                                 state[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cryostat.
                                vacuumController.
                                 state[HI_ERROR_RANGE])){
                    storeError(ERR_VACUUM_CONTROLLER,
                               0x04); // Error 0x04 -> Error: Vacuum controller state in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_VACUUM_CONTROLLER,
                               0x05); // Error 0x05 -> Warining: Vacuum controller state in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               vacuumController.
                state[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

