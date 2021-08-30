/*! \file   vacuumController.c
    \brief  Vacuum controller functions

    <b> File information: </b><br>
    Created: 2007/03/14 17:17:31 by avaccari

    This file contains all the functions necessary to handle vacuum controller
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
unsigned char   currentVacuumControllerModule=0;
/* Statics */
static HANDLER  vacuumControllerModulesHandler[VACUUM_CONTROLLER_MODULES_NUMBER]={vacuumSensorHandler,
                                                                                  vacuumSensorHandler,
                                                                                  enableHandler,
                                                                                  stateHandler};

/* Vacuum controller handler */
/*! This function will be called by the CAN message handling subroutine when the
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
        storeError(ERR_VACUUM_CONTROLLER, ERC_MODULE_RANGE); //Vacuum Controller submodule out of range
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
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cryostat.
                                    vacuumController.
                                     lastEnable)

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
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cryostat.
                                      vacuumController.
                                       lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the vacuum controller is changed by a control command. */
    CAN_BYTE=frontend.
              cryostat.
               vacuumController.
                enable;
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
        storeError(ERR_VACUUM_CONTROLLER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_VACUUM_CONTROLLER, ERC_RCA_RANGE); //Monitor message out or range
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
                    state;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     vacuumController.
                      state;
    }
    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               vacuumController.
                state;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
