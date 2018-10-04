/*! \file   fetimExtTemp.c
    \brief  FETIM external temperature sensor

    <b> File informations: </b><br>
    Created: 2011/03/28 17:34:50 by avaccari

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
unsigned char   currentFetimExtTempModule=0;
/* Statics */
static HANDLER fetimExtTempModulesHandler[FETIM_EXT_MODULES_NUMBER]={tempHandler,
                                                                     outOfRangeHandler};

/* Compressor Temperature Sensor Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the compressor temperature sensors */
void fetimExtTempHandler(void){
    #ifdef DEBUG_FETIM
    printf("   FETIM External Temperature Sensor: %d\n",
           currentCompressorModule);
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentFetimExtTempModule=(CAN_ADDRESS&FETIM_EXT_MODULES_RCA_MASK)>>FETIM_EXT_MODULES_MASK_SHIFT;
    if(currentFetimExtTempModule>=FETIM_EXT_MODULES_NUMBER){
        storeError(ERR_FETIM_EXT_TEMP, ERC_MODULE_RANGE); //Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (fetimExtTempModulesHandler[currentFetimExtTempModule])();

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
        storeError(ERR_FETIM_EXT_TEMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_FETIM_EXT_TEMP, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Fetch the last async monitored temperature */
    if(asyncFetimExtTempError[currentCompressorModule]==ERROR) {

        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
    }

    /* Store the last monitored value in the outgoing message */
    CONV_FLOAT=frontend.
                fetim.
                 compressor.
                  temp[currentCompressorModule].
                   temp[CURRENT_VALUE];

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big enadian
       (CAN). */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
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
        storeError(ERR_FETIM_EXT_TEMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_FETIM_EXT_TEMP, ERC_RCA_RANGE); //Monitor message out of range
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

    }
    /* The CAN message payload is already loaded. Set the size */
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
