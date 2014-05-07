/*! \file   lna.c
    \brief  LNA functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lna.c,v 1.23 2006/10/20 20:03:04 avaccari Exp $

    This files contains all the functions necessary to handle LNA events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentLnaModule=0;
/* Statics */
static HANDLER  lnaModulesHandler[LNA_MODULES_NUMBER]={lnaStageHandler,
                                                       lnaStageHandler,
                                                       lnaStageHandler,
                                                       lnaStageHandler,
                                                       lnaStageHandler,
                                                       lnaStageHandler,
                                                       enableHandler};

/* LNA handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the LNA. */
void lnaHandler(void){

    #ifdef DEBUG
        printf("      LNA\n");
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected sideband is outfitted with the desired LNA */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sideband[currentPolarizationModule].
               lna.
                available==UNAVAILABLE){
            storeError(ERR_LNA,
                       0x01); // Error 0x01 -> LNA not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    currentLnaModule=(CAN_ADDRESS&LNA_MODULES_RCA_MASK)>>LNA_MODULES_MASK_SHIFT;
    if(currentLnaModule>=LNA_MODULES_NUMBER){
        storeError(ERR_LNA,
                   0x02); // Error 0x02 -> LNA submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (lnaModulesHandler[currentLnaModule])();
}

/* LNA enable handler */
static void enableHandler(void){

    #ifdef DEBUG
        printf("       enable\n");
    #endif /* DEBUG */

    /* If it's a control message (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     lastEnable,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            lna.
             lastEnable.
              status=NO_ERROR;

        /* Change the status of the LNA according to the content of the CAN
           message. */
        if(setLnaBiasEnable(CAN_BYTE?LNA_BIAS_ENABLE:
                                     LNA_BIAS_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                lna.
                 lastEnable.
                  status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     lastEnable,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the mixer bias mode is changed by a control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               polarization[currentBiasModule].
                sideband[currentPolarizationModule].
                 lna.
                  enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
