/*! \file   sideband.c
    \brief  Sideband functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sideband.c,v 1.11 2009/08/03 21:59:07 avaccari Exp $

    This files contains all the functions necessary to handle sideband events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentSidebandModule=0;
/* Statics */
static HANDLER  sidebandModulesHandler[SIDEBAND_MODULES_NUMBER]={sisHandler,
                                                                 sisMagnetHandler,
                                                                 lnaHandler};

/* Sideband handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the sideband. */
void sidebandHandler(void){

    #ifdef DEBUG
        printf("    Sideband: %d (currentPolarizationModule)\n",
               currentPolarizationModule);
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected polarization is outfitted with the desired sideband */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sideband[currentPolarizationModule].
               available==UNAVAILABLE){
            storeError(ERR_SIDEBAND,
                       0x01); // Error 0x01 -> Sideband not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

            return;
        }

    #endif /* DATABASE_HARDW */
    /* Check if the submodule is in range */
    currentSidebandModule=(CAN_ADDRESS&SIDEBAND_MODULES_RCA_MASK)>>SIDEBAND_MODULES_MASK_SHIFT;
    if(currentSidebandModule>=SIDEBAND_MODULES_NUMBER){
        storeError(ERR_SIDEBAND,
                   0x02); // Error 0x02 -> Sideband submodule out of range

        CAN_STATUS = HARDW_RNG_ERR;

        return;
    }

    /* Call the correct handler */
    (sidebandModulesHandler[currentSidebandModule])();
}
