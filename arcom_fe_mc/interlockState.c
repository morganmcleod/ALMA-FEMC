/*! \file   interlockState.c
    \brief  FETIM Interlock State

    <b> File informations: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockState.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This file contains all the functions necessary to handle FETIM interlock
    state events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"

/* Globals */
unsigned char   currentInterlockStateModule=0;
/* Statics */
static HANDLER interlockStateModulesHandler[INTERLOCK_STATE_MODULES_NUMBER]={interlockGlitchHandler,
                                                                             multiFailHandler,
                                                                             tempOutOfRangeHandler,
                                                                             flowOutOfRangeHandler,
                                                                             delayTrigHandler,
                                                                             shutdownTrigHandler};

/* Interlock State Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock state */
void interlockStateHandler(void){

    #ifdef DEBUG_FETIM
        printf("   State\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockStateModule=(CAN_ADDRESS&INTERLOCK_STATE_MODULES_RCA_MASK)>>INTERLOCK_STATE_MODULES_MASK_SHIFT;
    if(currentInterlockStateModule>=INTERLOCK_STATE_MODULES_NUMBER){
        storeError(ERR_INTRLK_STATE,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockStateModulesHandler[currentInterlockStateModule])();

    return;

}










/* Multi failure handler */
/* This function handles the single failure status for the FETIM interlock
   sensors. */
static void multiFailHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Multi Failure\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_STATE,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_STATE,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_MULTI_FAIL)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     multiFail[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     multiFail[CURRENT_VALUE];
    }

    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;


}


/* Temperature sensor out of range handler */
/* Deal with the temperature sensor out of range signal */
static void tempOutOfRangeHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Temperature Sensor Out Of Range\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_STATE,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_STATE,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_TEMP_OOR)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     tempOutRng[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     tempOutRng[CURRENT_VALUE];
    }
    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


/* Flow sensor out of range handler */
/* Deal with the flow sensor out of range signal */
static void flowOutOfRangeHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Flow Sensor Out Of Range\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_STATE,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_STATE,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_FLOW_OOR)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     flowOutRng[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     flowOutRng[CURRENT_VALUE];
   }
    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


/* Delay Shutdown Trigger handler */
/* Deal with the delay shutdown triggered signal */
static void delayTrigHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Delay Shutdown Trigger\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_STATE,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_STATE,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor shutdown delay triggered digital line */
    if(getFetimDigital(FETIM_DIG_SHTDWN_DELAY)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     delayTrig[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     delayTrig[CURRENT_VALUE];
    }
    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


/* Shutdown Trigger handler */
/* Deal with the shutdown triggered signal */
static void shutdownTrigHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Shutdown Trigger\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_STATE,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_STATE,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_SHTDWN_TRIG)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     shutdownTrig[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     shutdownTrig[CURRENT_VALUE];
    }
    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
