/*! \file   compTemp.c
    \brief  FETIM Compressor temperature sensor

    <b> File informations: </b><br>
    Created: 2011/03/28 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: compTemp.c,v 1.2 2011/11/09 00:40:30 avaccari Exp $

    This file contains all the functions necessary to handle FETIM compressor
    temperature sensor events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"
#include "async.h"

/* Globals */
unsigned char   currentCompTempModule=0;
/* Statics */
static HANDLER compTempModulesHandler[COMP_TEMP_MODULES_NUMBER]={tempHandler,
                                                                 outOfRangeHandler};

/* Compressor Temperature Sensor Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the compressor temperature sensors */
void compTempHandler(void){
    #ifdef DEBUG_FETIM
    printf("   Compressor Temperature Sensor: %d\n",
           currentCompressorModule);
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentCompTempModule=(CAN_ADDRESS&COMP_TEMP_MODULES_RCA_MASK)>>COMP_TEMP_MODULES_MASK_SHIFT;
    if(currentCompTempModule>=COMP_TEMP_MODULES_NUMBER){
        storeError(ERR_COMP_TEMP,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (compTempModulesHandler[currentCompTempModule])();

    return;

}

/* Temperature handler */
/* This function return the current temperature of the addressed compressor
   temeprature sensor */
static void tempHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Temperature\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_COMP_TEMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_COMP_TEMP,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }


    /* Monitor Interlock temperature */
    if(asyncFetimExtTempError[currentCompressorModule]==ERROR){

        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                    fetim.
                     compressor.
                      temp[currentCompressorModule].
                       temp[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                    fetim.
                     compressor.
                      temp[currentCompressorModule].
                       temp[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            compressor.
                             temp[currentCompressorModule].
                              temp[LOW_ERROR_RANGE],
                          CONV_FLOAT,
                          frontend.
                           fetim.
                            compressor.
                             temp[currentCompressorModule].
                              temp[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                compressor.
                                 temp[currentCompressorModule].
                                  temp[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               fetim.
                                compressor.
                                 temp[currentCompressorModule].
                                  temp[HI_ERROR_RANGE])){
                    storeError(ERR_COMP_TEMP,
                               0x04); // Error 0x04 -> Error: compressor external temperature in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_COMP_TEMP,
                               0x05); // Error 0x05 -> Warning: compressor external temperature in warning range
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






/* Temperature out of range handler */
/* This function return the current out of range status of the addressed
   compressor temeprature sensor */
static void outOfRangeHandler(void){

    #ifdef DEBUG_FETIM
        printf("    Temperature out of range status\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_COMP_TEMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_COMP_TEMP,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If Monitor on a Monitor RCA */
    /* Monitor Single Fail digital line */
    if(getFetimDigital(FETIM_DIG_EXT_TEMP_OOR)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  fetim.
                   compressor.
                    temp[currentCompressorModule].
                     tempOutRng[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CAN_BYTE=frontend.
                  fetim.
                   compressor.
                    temp[currentCompressorModule].
                     tempOutRng[CURRENT_VALUE];

        /* Check the result agains the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           fetim.
                            compressor.
                             temp[currentCompressorModule].
                              tempOutRng[LOW_ERROR_RANGE],
                          CAN_BYTE,
                          frontend.
                           fetim.
                            compressor.
                             temp[currentCompressorModule].
                              tempOutRng[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               fetim.
                                compressor.
                                 temp[currentCompressorModule].
                                  tempOutRng[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               fetim.
                                compressor.
                                 temp[currentCompressorModule].
                                  tempOutRng[HI_ERROR_RANGE])){
                    storeError(ERR_COMP_TEMP,
                               0x06); // Error 0x06 -> Error: temperature sensor out of range digital value in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_COMP_TEMP,
                               0x07); // Error 0x07 -> Warning: temperature sensor out of range digital value in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;


}


