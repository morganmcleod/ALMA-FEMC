/*! \file   polSpecialMsgs.c
    \brief  Polarization special messages functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle polarization
    special messages events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "polSpecialMsgs.h"
#include "error.h"
#include "globalDefinitions.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentPolSpecialMsgsModule=0;
/* Statics */
static HANDLER  polSpecialMsgsModulesHandler[POL_SPECIAL_MSGS_MODULES_NUMBER]={polDacHandler,
                                                                               polDacHandler};

/* Polarization special messages handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the polarization special messages. */
void polSpecialMsgsHandler(void){

    #ifdef DEBUG
        printf("    Polarization Special Message:\n");
    #endif /* DEBUG */

    /* There is no need to check for the existance of the hardware because if
       the addressed polarization exists then also the DACs are installed. */

    /* Check if the submodule is in range */
    currentPolSpecialMsgsModule=(CAN_ADDRESS&POL_SPECIAL_MSGS_MODULES_RCA_MASK)>>POL_SPECIAL_MSGS_MODULES_MASK_SHIFT;
    if(currentPolSpecialMsgsModule>=POL_SPECIAL_MSGS_MODULES_NUMBER){
        storeError(ERR_POL_SPECIAL_MSGS, ERC_MODULE_RANGE); //submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (polSpecialMsgsModulesHandler[currentPolSpecialMsgsModule])();
}

