/*! \file   interlock.c
    \brief  FETIM Interlock

    <b> File informations: </b><br>
    Created: 2011/03/25 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlock.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This file contains all the functions necessary to handle FETIM interlock
    events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"

/* Globals */
unsigned char   currentInterlockModule=0;
/* Statics */
static HANDLER interlockModulesHandler[INTERLOCK_MODULES_NUMBER]={interlockSensorsHandler,
                                                                  interlockStateHandler};

/* Interlock Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock */
void interlockHandler(void){

    #ifdef DEBUG_FETIM
        printf("  Interlock\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockModule=(CAN_ADDRESS&INTERLOCK_MODULES_RCA_MASK)>>INTERLOCK_MODULES_MASK_SHIFT;
    if(currentInterlockModule>=INTERLOCK_MODULES_NUMBER){
        storeError(ERR_INTERLOCK,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockModulesHandler[currentInterlockModule])();

    return;

}
