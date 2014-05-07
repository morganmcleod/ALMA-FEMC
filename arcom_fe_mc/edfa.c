/*! \file   edfa.c
    \brief  EDFA

    <b> File informations: </b><br>
    Created: 2007/06/02 12:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: edfa.c,v 1.1 2007/06/02 21:23:58 avaccari Exp $

    This file contains all the functions necessary to handle EDFA events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Extern */
unsigned char   currentEdfaModule=0;
/* Statics */
static HANDLER  edfaModulesHandler[EDFA_MODULES_NUMBER]={laserHandler,
                                                         photoDetectorHandler,
                                                         modulationInputHandler};

/* EDFA handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the EDFA. */
void edfaHandler(void){

    #ifdef DEBUG_LPR
        printf("  EDFA\n");
    #endif /* DEBUG_LPR */

    /* Since the LPR is always outfitted with the EDFA, no hardware check is
       performed. */

    /* Check if the specified submodule is in range. */
    currentEdfaModule=(CAN_ADDRESS&EDFA_MODULES_RCA_MASK)>>EDFA_MODULES_MASK_SHIFT;
    if(currentEdfaModule>=EDFA_MODULES_NUMBER){
        storeError(ERR_EDFA,
                   0x01); // Error 0x01 -> EDFA submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (edfaModulesHandler[currentEdfaModule])();
}
