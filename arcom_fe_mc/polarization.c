/*! \file   polarization.c
    \brief  Polarization functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle polarization
    events. */

/* Includes */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "serialInterface.h"
#include "biasSerialInterface.h"
#include "error.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentPolarizationModule=0;
/* Statics */
static HANDLER  polarizationModulesHandler[POLARIZATION_MODULES_NUMBER]={sidebandHandler,
                                                                         sidebandHandler,
                                                                         lnaLedHandler,
                                                                         sisHeaterHandler,
                                                                         RESERVEDHandler,
                                                                         polSpecialMsgsHandler};

/* Polarization init */
/*! This function performs the operations necessary to initialize a polarization
    at runtime.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int polarizationInit(void){
    #ifdef DEBUG_INIT
        printf(" - Initializing polarization %d...\n", currentBiasModule);
        printf("   - 10MHz...\n");
    #endif // DEBUG_INIT

    if(serialAccess(BIAS_10MHZ_MODE(currentBiasModule),
                NULL,
                BIAS_10MHZ_MODE_SIZE,
                BIAS_10MHZ_MODE_SHIFT_SIZE,
                BIAS_10MHZ_MODE_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
        return ERROR;
    }
    frontend.cartridge[currentModule].polarization[currentBiasModule].
        ssi10MHzEnable = ENABLE;

    #ifdef DEBUG_INIT
        printf("     done!\n"); // 10MHz
        printf("   done!\n"); // Polarization
    #endif // DEBUG_INIT

    return NO_ERROR;
}

/* Polarization handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the polarization. */
void polarizationHandler(void){
    #ifdef DEBUG
        printf("   Polarization: %d (currentBiasModule)\n", currentBiasModule);
    #endif /* DEBUG */

    /* Check if the submodule is in range */
    currentPolarizationModule=(CAN_ADDRESS&POLARIZATION_MODULES_RCA_MASK)>>POLARIZATION_MODULES_MASK_SHIFT;
    if(currentPolarizationModule>=POLARIZATION_MODULES_NUMBER){
        storeError(ERR_POLARIZATION, ERC_MODULE_RANGE); //Polarization submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct handler */
    (polarizationModulesHandler[currentPolarizationModule])();
}

// Dummy handler for where schottkyMixer used to be.
void RESERVEDHandler(void) {

}
