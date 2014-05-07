/*! \file   polarization.c
    \brief  Polarization functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: polarization.c,v 1.15 2007/08/28 21:46:47 avaccari Exp $

    This files contains all the functions necessary to handle polarization
    events. */

/* Includes */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "serialInterface.h"
#include "biasSerialInterface.h"
#include "error.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentPolarizationModule=0;
/* Statics */
static HANDLER  polarizationModulesHandler[POLARIZATION_MODULES_NUMBER]={sidebandHandler,
                                                                         sidebandHandler,
                                                                         lnaLedHandler,
                                                                         sisHeaterHandler,
                                                                         schottkyMixerHandler,
                                                                         polSpecialMsgsHandler};

/* Polarization init */
/*! This function performs the operations necessary to initialize a polarization
    at runtime.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int polarizationInit(void){
        printf(" - Initializing polarization %d...\n",
               currentBiasModule);

        #ifdef DATABASE_HARDW
            if(frontend.
                cartridge[currentModule].
                 polarization[currentBiasModule].
                  available==AVAILABLE){

                printf("   - 10MHz...\n");

                if(serialAccess(BIAS_10MHZ_MODE(currentBiasModule),
                            NULL,
                            BIAS_10MHZ_MODE_SIZE,
                            BIAS_10MHZ_MODE_SHIFT_SIZE,
                            BIAS_10MHZ_MODE_SHIFT_DIR,
                            SERIAL_WRITE)==ERROR){
                    return ERROR;
                }
                frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   ssi10MHzEnable=ENABLE;

                printf("     done!\n"); // 10MHz
            }
        #else

            printf("   - 10MHz...\n");

            if(serialAccess(BIAS_10MHZ_MODE(currentBiasModule),
                        NULL,
                        BIAS_10MHZ_MODE_SIZE,
                        BIAS_10MHZ_MODE_SHIFT_SIZE,
                        BIAS_10MHZ_MODE_SHIFT_DIR,
                        SERIAL_WRITE)==ERROR){
                return ERROR;
            }
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               ssi10MHzEnable=ENABLE;

            printf("     done!\n"); // 10MHz

        #endif /* DATABASE_HARDW */

        printf("   done!\n"); // Polarization
    return NO_ERROR;
}

/* Polarization handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the polarization. */
void polarizationHandler(void){
    #ifdef DEBUG
        printf("   Polarization: %d (currentBiasModule)\n",
               currentBiasModule);
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected receiver is outfitted with the desired polarization */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              available==UNAVAILABLE){
            storeError(ERR_POLARIZATION,
                       0x01); // Error 0x01 -> Polarization not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

            return;
        }
    #endif /* DATABASE_HARDW */


    /* Check if the submodule is in range */
    currentPolarizationModule=(CAN_ADDRESS&POLARIZATION_MODULES_RCA_MASK)>>POLARIZATION_MODULES_MASK_SHIFT;
    if(currentPolarizationModule>=POLARIZATION_MODULES_NUMBER){
        storeError(ERR_POLARIZATION,
                   0x02); // Error 0x02 -> Polrization submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (polarizationModulesHandler[currentPolarizationModule])();
}

