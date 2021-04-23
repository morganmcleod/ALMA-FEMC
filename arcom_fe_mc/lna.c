/*! \file   lna.c
    \brief  LNA functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle LNA events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentLnaModule=0;
/* Statics */
static HANDLER  lnaModulesHandler[LNA_MODULES_NUMBER]={lnaStageHandler,
                                                       lnaStageHandler,
                                                       lnaStageHandler,
                                                       RESERVEDLNAHandler,
                                                       RESERVEDLNAHandler,
                                                       RESERVEDLNAHandler,
                                                       enableHandler};

/* LNA handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the LNA. */
void lnaHandler(void){

    #ifdef DEBUG
        printf("      LNA\n");
    #endif /* DEBUG */

    /* Check if the submodule is in range */
    currentLnaModule=(CAN_ADDRESS&LNA_MODULES_RCA_MASK)>>LNA_MODULES_MASK_SHIFT;
    if(currentLnaModule>=LNA_MODULES_NUMBER)
    {
        storeError(ERR_LNA, ERC_MODULE_RANGE); //LNA submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (lnaModulesHandler[currentLnaModule])();
}

void RESERVEDLNAHandler(void) {
    // Handler for LNA stages 4,5,6 which don't exist
    storeError(ERR_LNA, ERC_MODULE_ABSENT); //LNA not installed
    CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
    return;
}

/* LNA enable handler */
static void enableHandler(void){

    #ifdef DEBUG
        printf("       enable\n");
    #endif /* DEBUG */

    /* If it's a control message (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
                                      sideband[currentPolarizationModule].lna.lastEnable)

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.
             cartridge[currentModule].
              standby2) 
        {
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].lna.lastEnable.status=HARDW_BLKD_ERR;
            return;
        }

        /* Change the status of the LNA according to the content of the CAN
           message. */
        if (setLnaBiasEnable(CAN_BYTE ? LNA_BIAS_ENABLE : LNA_BIAS_DISABLE) == ERROR) {
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].lna.lastEnable.status=ERROR;
            return;
        }

        /* If this is band 1 or 2, and if this is SB1, also set SB2:    */
        if ((currentModule == BAND1 || currentModule == BAND2) && currentPolarizationModule == SIDEBAND0) {
            currentPolarizationModule = SIDEBAND1;
            setLnaBiasEnable(CAN_BYTE ? LNA_BIAS_ENABLE : LNA_BIAS_DISABLE);
            currentPolarizationModule = SIDEBAND0;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // Return the last control message and status:
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
                                        sideband[currentPolarizationModule].lna.lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the mixer bias mode is changed by a control command. */
    CAN_BYTE=frontend.cartridge[currentModule].polarization[currentBiasModule].
                 sideband[currentPolarizationModule].lna.enable;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

// set the specified LNA to STANDBY2 mode
void lnaGoStandby2() {
    int ret;

    #ifdef DEBUG_GO_STANDBY2
        printf(" - lnaGoStandby2 pol=%d sb=%d\n", currentBiasModule, currentPolarizationModule);
    #endif // DEBUG_GO_STANDBY2

    // disable the LNA:
    ret = setLnaBiasEnable(LNA_BIAS_DISABLE);

    #ifdef DEBUG_GO_STANDBY2
        if (ret)
            printf(" -- ret=%d\n", ret);
    #endif // DEBUG_GO_STANDBY2
}
