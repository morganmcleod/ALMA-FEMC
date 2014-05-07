/*! \file   turboPump.c
    \brief  Turbo pump functions

    <b> File informations: </b><br>
    Created: 2007/03/14 17:11:40 by avaccari

    <b> CVS informations: </b><br>
    \$Id: turboPump.c,v 1.6 2011/11/09 00:40:30 avaccari Exp $

    This files contains all the functions necessary to handle turbo pump
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
unsigned char   currentTurboPumpModule=0;
/* Statics */
static HANDLER turboPumpModulesHandler[TURBO_PUMP_MODULES_NUMBER]={enableHandler,
                                                                   stateHandler,
                                                                   speedHandler};

/* Turbo pump handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cryostat turbo pump. */
void turboPumpHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  Turbo Pump\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Check if the backing pump is enabled. If it's not then the electronics to
       monitor and control the turbo pump is off. In that case, return the
       HARDW_BLKD_ERR and return. */
    if(frontend.
        cryostat.
         backingPump.
          enable[CURRENT_VALUE]==BACKING_PUMP_DISABLE){
        storeError(ERR_TURBO_PUMP,
                   0x08); // Error 0x08 -> Backing Pump off -> Turbo pump disabled
        CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
        return;
    }

    /* Since the cryostat is always outfitted with the turbo pump, no hardware
       check is required. */

    /* Check if the submodule is in range */
    currentTurboPumpModule=(CAN_ADDRESS&TURBO_PUMP_MODULES_RCA_MASK);
    if(currentTurboPumpModule>=TURBO_PUMP_MODULES_NUMBER){
        storeError(ERR_TURBO_PUMP,
                   0x01); // Error 0x01 -> Turbo Pump submodule out of range
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
    if(CAN_SIZE){
        /* If FETIM available and external sensors temperature < 15C, do not start
           and return HARDW_BLK_ERROR. */
        if(frontend.
            fetim.
             available==AVAILABLE){
            if((frontend.
                 fetim.
                  compressor.
                   temp[COMP_TEMP_SENSOR_TURBO].
                    temp[CURRENT_VALUE]<TURBO_PUMP_MIN_TEMPERATURE)||(frontend.
                                                                       fetim.
                                                                        compressor.
                                                                         temp[COMP_TEMP_SENSOR_TURBO].
                                                                          temp[CURRENT_VALUE]>TURBO_PUMP_MAX_TEMPERATURE)){

                storeError(ERR_TURBO_PUMP,
                           0x09); // Error 0x09 -> Temperature below allowed range -> Turbo pump disabled
                frontend.
                 cryostat.
                  turboPump.
                   lastEnable.
                    status=HARDW_BLKD_ERR; // Store the status in the last control message

                return;
            }
        }

        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cryostat.
                  turboPump.
                   lastEnable,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cryostat.
          turboPump.
           lastEnable.
            status=NO_ERROR;

        /* Change the status of the turbo pump according to the content of the
           CAN message. */
        if(setTurboPumpEnable(CAN_BYTE?TURBO_PUMP_ENABLE:
                                       TURBO_PUMP_DISABLE)==ERROR){
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
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized. */
        memcpy(&CAN_SIZE,
               &frontend.
                 cryostat.
                  turboPump.
                   lastEnable,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the turbo pump is changed by a control command. */
    CAN_BYTE=frontend.
              cryostat.
               turboPump.
                enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}







/* Turbo pump state handler */
/* This function deals with the message directed to the error state of the
   turbo pump in the cryostat module. */
static void stateHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("   Turbo state\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA */
    if(CAN_SIZE){
        storeError(ERR_TURBO_PUMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_TURBO_PUMP,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Get the turbo pump error state */
    if(getTurboPumpStates()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cryostat.
                   turboPump.
                    state[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE = frontend.
                    cryostat.
                     turboPump.
                      state[CURRENT_VALUE];
        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cryostat.
                            turboPump.
                             state[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cryostat.
                            turboPump.
                             state[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cryostat.
                                turboPump.
                                 state[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cryostat.
                                turboPump.
                                 state[HI_ERROR_RANGE])){
                    storeError(ERR_TURBO_PUMP,
                               0x04); // Error 0x04 -> Error: Turbo pump state in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_TURBO_PUMP,
                               0x05); // Error 0x05 -> Warning: Turbo pump state in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               turboPump.
                state[CURRENT_VALUE];
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
        storeError(ERR_TURBO_PUMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_TURBO_PUMP,
                   0x03); // Error 0x03 -> Monitor message out or range
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
                    speed[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=frontend.
                  cryostat.
                   turboPump.
                    speed[CURRENT_VALUE];

        /* Check the results against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the results but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cryostat.
                            turboPump.
                             speed[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cryostat.
                            turboPump.
                             speed[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cryostat.
                                turboPump.
                                 speed[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cryostat.
                                turboPump.
                                 speed[HI_ERROR_RANGE])){
                    storeError(ERR_TURBO_PUMP,
                               0x06); // Error 0x06 -> Error: Turbo pump speed in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_TURBO_PUMP,
                               0x07); // Error 0x07 -> Warning: Turbo pump speed in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size */
    CAN_BYTE=frontend.
              cryostat.
               turboPump.
                speed[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
