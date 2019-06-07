/*! \file   interlockTemp.c
    \brief  FETIM Interlock Temperature

    <b> File information: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    This file contains all the functions necessary to handle FETIM interlock
    temperature events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"

/* Globals */
unsigned char   currentInterlockTempModule=0;
/* Statics */
static HANDLER interlockTempModulesHandler[INTERLOCK_TEMP_MODULES_NUMBER]={interlockTempSensHandler,
                                                                           interlockTempSensHandler,
                                                                           interlockTempSensHandler,
                                                                           interlockTempSensHandler,
                                                                           interlockTempSensHandler};

/* Interlock Temperature Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock temperature */
void interlockTempHandler(void){
    #ifdef DEBUG_FETIM
        printf("    Temperature\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockTempModule=(CAN_ADDRESS&INTERLOCK_TEMP_MODULES_RCA_MASK)>>INTERLOCK_TEMP_MODULES_MASK_SHIFT;
    if(currentInterlockTempModule>=INTERLOCK_TEMP_MODULES_NUMBER){
        storeError(ERR_INTRLK_TEMP, ERC_MODULE_RANGE); //Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockTempModulesHandler[currentInterlockTempModule])();

    return;

}


