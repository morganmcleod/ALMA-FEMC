/*! \file   lprTemp.c
    \brief  LPR temperature sensors functions

    <b> File information: </b><br>
    Created: 2007/06/02 16:29:22 by avaccari

    This file contains all the functions necessary to handle the lpr
    temperature sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "lprSerialInterface.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
unsigned char   currentLprTempModule=0;
/* Statics */
static HANDLER lprTempModulesHandler[LPR_TEMP_MODULES_NUMBER]={tempHandler};

/* LPR temperature sensors handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the LPR temperature sensors. */
void lprTempHandler(void){

    #ifdef DEBUG_LPR
        printf("  Temperature sensor: %d\n",
               currentLprModule);
    #endif /* DEBUG_LPR */

    /* Since the LPR is always outfitted with all the temperature sensors, no
       hardware check is performed. */

    /* Since there is only one submodule in the LPR temperature sensor, the
       check to see if the desired submodule is in range is not needed and we
       can directly call the correct handler. */

    /* Call the correct handler */
    (lprTempModulesHandler[currentLprTempModule])();
}

/* LPR Temperature handler */
static void tempHandler(void){
    #ifdef DEBUG_LPR
        printf("   Temperature Value\n");
    #endif /* DEBUG_LPR */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_LPR_TEMP, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LPR_TEMP, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor LPR temperature */
    if(getLprTemp()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   lpr.
                    lprTemp[currentLprModule].
                     temp;

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                   lpr.
                    lprTemp[currentLprModule].
                     temp;
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big enadian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
