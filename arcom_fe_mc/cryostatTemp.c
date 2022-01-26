/*! \file   cryostatTemp.c
    \brief  Cryostat temperature sensors functions

    <b> File information: </b><br>
    Created: 2007/03/14 16:44:04 by avaccari

    This file contains all the functions necessary to handle cryostat
    temperature sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "can.h"
#include "error.h"
#include "cryostatSerialInterface.h"
#include "globalDefinitions.h"
#include "async.h"

/* Globals */
/* Externs */
unsigned char   currentCryostatTempModule=0;
/* An extern to perform the mapping between the temperature sensor number and
   the bit address. This is not done with the usual define because of the non
   linearity of the mapping. */
unsigned int    currentCryostatTempSensorAdd[CRYOSTAT_TEMP_SENSORS_NUMBER]={0x0000, // Sensor 0 -> TVO Sensor 4K cryocooler
                                                                            0x0004, // Sensor 1 -> TVO Sensor 4K plate near link #1
                                                                            0x0005, // Sensor 2 -> TVO Sensor 4K plate near link #2
                                                                            0x0008, // Sensor 3 -> TVO Sensor 4K plate far side #1
                                                                            0x000A, // Sensor 4 -> TVO Sensor 4K plate far side #2
                                                                            0x000C, // Sensor 5 -> TVO Sensor 12K cryocooler
                                                                            0x0010, // Sensor 6 -> TVO Sensor 12K plate near link
                                                                            0x0014, // Sensor 7 -> TVO Sensor 12K plate far side
                                                                            0x0018, // Sensor 8 -> TVO Sensor 12K shield top
                                                                            0x0100, // Sensor 9 -> PRT Sensor 90K cryocooler
                                                                            0x0120, // Sensor 10 -> PRT Sensor 90K plate near link
                                                                            0x0140, // Sensor 11 -> PRT Sensor 90K plate far side
                                                                            0x0160};// Sensor 12 -> PRT Sensor 90K shield top
/* Statics */
static HANDLER cryostatTempModulesHandler[CRYOSTAT_TEMP_MODULES_NUMBER]={tempHandler, 
                                                                         sequentialCoeffHandler};

/* Cryostat temperatures sensors handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the cryostat temperature sensors. */
void cryostatTempHandler(void){

    #ifdef DEBUG_CRYOSTAT
    printf("  Cryostat Temperature Sensor: %d\n",
           currentCryostatModule);
    #endif /* DEBUG_CRYOSTAT */

    /* Since the cryostat is always outfitted with all the temperature sensors,
       no hardware check is performed. */

    /* Check if the submodule is in range */
    currentCryostatTempModule = (CAN_ADDRESS & CRYOSTAT_TEMP_MODULES_RCA_MASK) >> CRYOSTAT_TEMP_MODULES_MASK_SHIFT;
    if(currentCryostatTempModule >= CRYOSTAT_TEMP_MODULES_NUMBER){
        storeError(ERR_CRYOSTAT_TEMP, ERC_MODULE_RANGE); //Cryostat submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }
    /* Call the correct handler */
    (cryostatTempModulesHandler[currentCryostatTempModule])();
}

/* Temperature Value Handler */
/* This function deals with the messages directed to the cryostat temperature
   sensors. */
static void tempHandler(void) {

    #ifdef DEBUG_CRYOSTAT
        printf("   Temperature Value\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_CRYOSTAT_TEMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error snce there are no control messages
       allowed on the RCA. */
    if(currentClass == CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_CRYOSTAT_TEMP, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the sate in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor cryostat temperature */
    if(asyncCryoTempError[currentCryostatModule]==ERROR){

        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;

        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.cryostat.cryostatTemp[currentCryostatModule].temp;

        /* If the error was a conversion error, store the status in the CAN
           message. */
        if(CONV_FLOAT==CRYOSTAT_TEMP_CONV_ERR){
            CAN_STATUS = HARDW_CON_ERR;
        }

    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT=frontend.cryostat.cryostatTemp[currentCryostatModule].temp;
    }

    /* If the async monitoring is disabled, notify the monitored message */
    if(asyncState==ASYNC_OFF) {
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

static void sequentialCoeffHandler(void) {
// This implements GET_CRYOSTAT_TEMP[Se]_TVO_COEFF and SET_CRYOSTAT_TEMP[Se]_TVO_COEFF
// which set/get the coefficients in a round-robin manner and are therefore stateful.
    unsigned char coeff;

    #ifdef DEBUG_CRYOSTAT
        printf("   Temperature TVO sequential coeff\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if (CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cryostat.cryostatTemp[currentCryostatModule].lastCommand)

        /* Extract the coefficient order byte */
        coeff = CAN_DATA(4);

        /* Range-check order */
        if (coeff >= TVO_COEFFS_NUMBER) {
            storeError(ERR_CRYOSTAT_TEMP, ERC_COMMAND_VAL);

            /* Store the error in the last control message variable */
            frontend.cryostat.cryostatTemp[currentCryostatModule].lastCommand.status = CON_ERROR_RNG;

            /* Reset the last order to a legal value */
            frontend.cryostat.cryostatTemp[currentCryostatModule].nextCoeff = TVO_COEFF_0;
            return;
        }

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);
        frontend.cryostat.cryostatTemp[currentCryostatModule].coeff[coeff] = CONV_FLOAT;
        frontend.cryostat.cryostatTemp[currentCryostatModule].nextCoeff = coeff;

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if (currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cryostat.cryostatTemp[currentCryostatModule].lastCommand)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Return the next/last coeffiecent and order */
    coeff = frontend.cryostat.cryostatTemp[currentCryostatModule].nextCoeff;

    CONV_FLOAT=frontend.cryostat.cryostatTemp[currentCryostatModule].coeff[coeff];
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_DATA(4) = coeff;

    // float plus coeff byte:
    CAN_SIZE = 5;

    /* Cycle through the coeffs on subsequent reads */
    if (++coeff >= TVO_COEFFS_NUMBER)
        coeff = TVO_COEFF_0;
    frontend.cryostat.cryostatTemp[currentCryostatModule].nextCoeff = coeff;
}

void specificCoeffHandler(unsigned int sensor, unsigned int coeff) {
// This implements GET_CRYOSTAT_TEMP[Se]_TVO_COEFF[Co] and SET_CRYOSTAT_TEMP[Se]_TVO_COEFF[Co
// which set/get specific coefficients encoded in the RCA are therefore stateless.
    #ifdef DEBUG_CRYOSTAT
        printf("   Temperature TVO %d specific coeff A%d\n", sensor, coeff);
    #endif /* DEBUG_CRYOSTAT */
    
    /* Range-check sensor and coeff */
    if (sensor >= TVO_SENSORS_NUMBER || coeff >= TVO_COEFFS_NUMBER) {
        storeError(ERR_CRYOSTAT_TEMP, ERC_COMMAND_VAL);

        /* Store the error in the last control message variable */
        frontend.cryostat.cryostatTemp[currentCryostatModule].lastCommand.status = CON_ERROR_RNG;
        return;
    }

    /* If control (size !=0) */
    if (CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cryostat.cryostatTemp[sensor].lastCommand)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);
        frontend.cryostat.cryostatTemp[sensor].coeff[coeff] = CONV_FLOAT;

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if (currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cryostat.cryostatTemp[sensor].lastCommand)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Return the next/last coeffiecent and order */
    CONV_FLOAT=frontend.cryostat.cryostatTemp[sensor].coeff[coeff];
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);

    // float:
    CAN_SIZE = 4;
}
