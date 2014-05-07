/*! \file   interlockGlitch.c
    \brief  FETIM Interlock glitch

    <b> File informations: </b><br>
    Created: 2011/04/08 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockGlitch.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This file contains all the functions necessary to handle FETIM interlock
    glitch events. */

/* includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"

/* Globals */
unsigned char   currentInterlockGlitchModule=0;
/* Statics */
static HANDLER interlockGlitchModulesHandler[INTERLOCK_GLITCH_MODULES_NUMBER]={valueHandler,
                                                                               countTrigHandler};

/* Interlock Glitch Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock glitch */
void interlockGlitchHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Glitch\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockGlitchModule=(CAN_ADDRESS&INTERLOCK_GLITCH_MODULES_RCA_MASK)>>INTERLOCK_GLITCH_MODULES_MASK_SHIFT;
    if(currentInterlockGlitchModule>=INTERLOCK_GLITCH_MODULES_NUMBER){
        storeError(ERR_INTRLK_GLITCH,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockGlitchModulesHandler[currentInterlockGlitchModule])();

    return;

}





/* Glitch analog value handler */
/* Deal with the analog value of the glitch counter */
static void valueHandler(void){

    #ifdef DEBUG_FETIM
        printf("     Analog value\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_GLITCH,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_GLITCH,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }


    /* Monitor Interlock temperature */
    if(getIntrlkGlitchValue()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      state.
                       glitch.
                        value[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      state.
                       glitch.
                        value[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            interlock.
                             state.
                              glitch.
                               value[LOW_ERROR_RANGE],
                          CONV_FLOAT,
                          frontend.
                           fetim.
                            interlock.
                             state.
                              glitch.
                               value[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                interlock.
                                 state.
                                  glitch.
                                   value[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               fetim.
                                interlock.
                                 state.
                                  glitch.
                                   value[HI_ERROR_RANGE])){
                    storeError(ERR_INTRLK_GLITCH,
                               0x04); // Error 0x04 -> Error: interlock glitch value in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_INTRLK_GLITCH,
                               0x05); // Error 0x05 -> Warning: interlock glitch value in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big enadian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

}





/* Glitch triggered handler */
/* Deal with the glitch counter triggered value */
static void countTrigHandler(void){

    #ifdef DEBUG_FETIM
        printf("     Counter triggered\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_GLITCH,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_GLITCH,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_GLITCH_CNT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     glitch.
                      countTrig[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   interlock.
                    state.
                     glitch.
                      countTrig[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            interlock.
                             state.
                              glitch.
                               countTrig[LOW_ERROR_RANGE],
                          CAN_BYTE,
                          frontend.
                           fetim.
                            interlock.
                             state.
                              glitch.
                               countTrig[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                interlock.
                                 state.
                                  glitch.
                                   countTrig[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               fetim.
                                interlock.
                                 state.
                                  glitch.
                                   countTrig[HI_ERROR_RANGE])){
                    storeError(ERR_INTRLK_GLITCH,
                               0x06); // Error 0x06 -> Error: interlock glitch counter trigger digital value in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_INTRLK_GLITCH,
                               0x07); // Error 0x07 -> Warning: interlock glitch counter trigger digital value in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;


}


