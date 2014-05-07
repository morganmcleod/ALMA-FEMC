/*! \file   compTemp.c
    \brief  FETIM Compressor temperature sensor

    <b> File informations: </b><br>
    Created: 2011/03/28 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: he2Press.c,v 1.1 2011/08/05 19:19:53 avaccari Exp $

    This file contains all the functions necessary to handle FETIM compressor
    temperature sensor events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"

/* Globals */
unsigned char   currentHe2PressModule=0;
/* Statics */
static HANDLER he2PressModulesHandler[HE2_PRESS_MODULES_NUMBER]={pressHandler,
                                                                 outOfRangeHandler};

/* Compressor He2 Pressure Sensor Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the compressor He2 Pressure sensor */
void he2PressHandler(void){
    #ifdef DEBUG_FETIM
        printf("   Compressor He2 Pressure Sensor\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentHe2PressModule=(CAN_ADDRESS&HE2_PRESS_MODULES_RCA_MASK)>>HE2_PRESS_MODULES_MASK_SHIFT;
    if(currentHe2PressModule>=HE2_PRESS_MODULES_NUMBER){
        storeError(ERR_COMP_HE2_PRESS,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (he2PressModulesHandler[currentHe2PressModule])();

    return;

}

/* He2 Pressure handler */
/* This function return the current temperature of the addressed compressor He2
   pressure sensor */
static void pressHandler(void){

    #ifdef DEBUG_FETIM
        printf("    He2 pressure\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_COMP_HE2_PRESS,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_COMP_HE2_PRESS,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }


    /* Monitor Interlock temperature */
    if(getCompHe2Press()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                    fetim.
                     compressor.
                      he2Press.
                       pressure[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                    fetim.
                     compressor.
                      he2Press.
                       pressure[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            compressor.
                             he2Press.
                              pressure[LOW_ERROR_RANGE],
                          CONV_FLOAT,
                          frontend.
                           fetim.
                            compressor.
                             he2Press.
                              pressure[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                compressor.
                                 he2Press.
                                  pressure[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               fetim.
                                compressor.
                                 he2Press.
                                  pressure[HI_ERROR_RANGE])){
                    storeError(ERR_COMP_HE2_PRESS,
                               0x04); // Error 0x04 -> Error: compressor He2 pressure in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_COMP_HE2_PRESS,
                               0x05); // Error 0x05 -> Warning: compressor He2 pressure in warning range
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












/* He2 Pressure out of range handler */
/* This function return the current out of range status of the compressor He2
   pressure sensor */
static void outOfRangeHandler(void){

    #ifdef DEBUG_FETIM
        printf("    He2 pressure out of range status\n");
    #endif /* DEBUG_FETIM */


    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_COMP_HE2_PRESS,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_COMP_HE2_PRESS,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_HE2_PRESS_OOR)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   compressor.
                    he2Press.
                     pressOutRng[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   compressor.
                    he2Press.
                     pressOutRng[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            compressor.
                             he2Press.
                              pressOutRng[LOW_ERROR_RANGE],
                          CAN_BYTE,
                          frontend.
                           fetim.
                            compressor.
                             he2Press.
                              pressOutRng[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                compressor.
                                 he2Press.
                                  pressOutRng[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               fetim.
                                compressor.
                                 he2Press.
                                  pressOutRng[HI_ERROR_RANGE])){
                    storeError(ERR_COMP_HE2_PRESS,
                               0x06); // Error 0x06 -> Error: He2 pressure out of range digital value in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_COMP_HE2_PRESS,
                               0x07); // Error 0x07 -> Warning: He2 pressure out of range digital value in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}


