/*! \file   interlockFlow.c
    \brief  FETIM Interlock Flow sensors

    <b> File informations: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockFlow.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This file contains all the functions necessary to handle FETIM interlock
    flow sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "error.h"

/* Globals */
unsigned char   currentInterlockFlowModule=0;
/* Statics */
static HANDLER interlockFlowModulesHandler[INTERLOCK_FLOW_MODULES_NUMBER]={interlockFlowSensHandler,
                                                                           interlockFlowSensHandler};

/* Interlock Flow Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock flow sensors */
void interlockFlowHandler(void){
    #ifdef DEBUG_FETIM
        printf("    Flow\n");
    #endif /* DEBUG_FETIM */

    /* Check if the specified submodule is in range */
    currentInterlockFlowModule=(CAN_ADDRESS&INTERLOCK_FLOW_MODULES_RCA_MASK)>>INTERLOCK_FLOW_MODULES_MASK_SHIFT;
    if(currentInterlockFlowModule>=INTERLOCK_FLOW_MODULES_NUMBER){
        storeError(ERR_INTRLK_FLOW, ERC_MODULE_RANGE); //Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct function */
    (interlockFlowModulesHandler[currentInterlockFlowModule])();

    return;

}


