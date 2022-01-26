/*! \file   turboPump.c
    \brief  Turbo pump functions

    <b> File information: </b><br>
    Created: 2007/03/14 17:11:40 by avaccari

    This file contains all the functions necessary to handle turbo pump
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
unsigned char   currentTurboPumpModule=0;
/* Statics */
static HANDLER turboPumpModulesHandler[TURBO_PUMP_MODULES_NUMBER]={enableHandler,
                                                                   stateHandler,
                                                                   speedHandler};

/* Turbo pump handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the cryostat turbo pump. */
void turboPumpHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Turbo Pump\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Since the cryostat is always outfitted with the turbo pump, no hardware
       check is required. */

    /* Check if the submodule is in range */
    currentTurboPumpModule=(CAN_ADDRESS&TURBO_PUMP_MODULES_RCA_MASK);
    if(currentTurboPumpModule>=TURBO_PUMP_MODULES_NUMBER){
        storeError(ERR_TURBO_PUMP, ERC_MODULE_RANGE); //Turbo Pump submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (turboPumpModulesHandler[currentTurboPumpModule])();

    return;
}

/* Turbo pump enable handler */
/* This function deals with the messages directed to the enable state of the
   turbo pump in the cryostat module. */
static void enableHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Turbo Enable\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE) {
        
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cryostat.
                                    turboPump.
                                     lastEnable)

        /* Check if the backing pump is enabled. If it's not then the electronics to
           control the turbo pump are off.  Store HARDW_BLKD_ERR and return. */

        if (frontend.
             cryostat.
              backingPump.
               enable == BACKING_PUMP_DISABLE) 
        {
            frontend.
             cryostat.
              turboPump.
               lastEnable.
                status=HARDW_BLKD_ERR; // Store the status in the last control message

            // if the command was to enable, register an error too:
            if (CAN_BYTE) {
                storeError(ERR_TURBO_PUMP, ERC_MODULE_POWER); //Turbo pump disabled
            }
            return;
        }

        /* If FETIM available and external sensors temperature out of range, return HARDW_BLK_ERROR. */
        if (CAN_BYTE &&
            frontend.
             fetim.
              available==AVAILABLE) 
        {
            if((frontend.
                 fetim.
                  compressor.
                   temp[FETIM_EXT_SENSOR_TURBO].
                    temp < TURBO_PUMP_MIN_TEMPERATURE) ||
               (frontend.
                fetim.
                 compressor.
                  temp[FETIM_EXT_SENSOR_TURBO].
                   temp > TURBO_PUMP_MAX_TEMPERATURE)) 
            {
                storeError(ERR_TURBO_PUMP, ERC_HARDWARE_BLOCKED); //Temperature below allowed range -> Turbo pump disabled
                frontend.
                 cryostat.
                  turboPump.
                   lastEnable.
                    status=HARDW_BLKD_ERR; // Store the status in the last control message
             
                frontend.
                 cryostat.
                  turboPump.
                   enable = TURBO_PUMP_DISABLE;
                return;
            }
        }

        /* Change the status of the turbo pump according to the content of the
           CAN message. */
        if(setTurboPumpEnable(CAN_BYTE?TURBO_PUMP_ENABLE:
                                       TURBO_PUMP_DISABLE)==ERROR)
        {
            /* Store the ERROR state in the last control message variable */
            frontend.
             cryostat.
              turboPump.
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
                                      turboPump.
                                       lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    if (frontend.
         cryostat.
          backingPump.
           enable == BACKING_PUMP_DISABLE) 
    {
        // always return HARDW_BLKD when the backing pump is off
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    // return whatever was the last command sent:
    CAN_BYTE = frontend.
                cryostat.
                 turboPump.
                  enable;
    CAN_SIZE = CAN_BOOLEAN_SIZE;
}


/* Turbo pump state handler */
/* This function deals with the message directed to the error state of the
   turbo pump in the cryostat module. */
static void stateHandler(void){

    unsigned char prevErrorState;

    #ifdef DEBUG_CRYOSTAT
        printf("   Turbo state\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_TURBO_PUMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_TURBO_PUMP, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Cache the previous error state to detect change to ERROR */
    prevErrorState = frontend.
                      cryostat.
                       turboPump.
                        state=cryoRegisters.
                                              statusReg.
                                               bitField.
                                                turboPumpError;

    /* Get the turbo pump error state */
    if(getTurboPumpStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   turboPump.
                    state;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     turboPump.
                      state;
    }

    /* If the monitor state is not the same as previous and is ERROR: return a warning. */
    if(prevErrorState != frontend.
                          cryostat.
                           turboPump.
                            state)
    {   
        if(frontend.
            cryostat.
             turboPump.
              state == 1)
        {
            storeError(ERR_TURBO_PUMP, ERC_HARDWARE_ERROR); // The turbo pump state is ERROR.
        }
    }

    /* If monitor on a monitor RCA */
    if (frontend.
         cryostat.
          backingPump.
           enable == BACKING_PUMP_DISABLE) 
    {
        // always return HARDW_BLKD when the backing pump is off
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               turboPump.
                state;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


/* Turbo pump speed handler */
/* This function deals with the messages directed to the speed state of the
   turbo pump in the cryostat module. */
static void speedHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Turbo speed\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_TURBO_PUMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_TURBO_PUMP, ERC_RCA_RANGE); //Monitor message out or range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the turbo pump speed */
    if(getTurboPumpStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   turboPump.
                    speed;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=frontend.
                  cryostat.
                   turboPump.
                    speed;
    }

    /* If monitor on a monitor RCA */
    if (frontend.
         cryostat.
          backingPump.
           enable == BACKING_PUMP_DISABLE) 
    {
        // always return HARDW_BLKD when the backing pump is off
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               turboPump.
                speed;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
