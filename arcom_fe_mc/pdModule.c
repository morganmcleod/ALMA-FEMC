/*! \file   pdModule.c
    \breif  Power Distribution Module functions

    <b> File Informations: </b><br>
    Created: 2006/10/16 16:18:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pdModule.c,v 1.13 2009/08/25 21:39:39 avaccari Exp $

    This files contains all the funcions necessary to handle the submodules of
    cartridge power distribution system. */

/* Includes */
#include <stdio.h>      /* printf */
#include <string.h>     /* memcpy */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "pdSerialInterface.h"
#include "database.h"
#include "timer.h"

/* Globals */
/* Externs */
unsigned char   currentPdModuleModule=0;
/* Statics */
static HANDLER  pdModuleModulesHandler[PD_MODULE_MODULES_NUMBER]={pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  enableHandler};
/* Power distribution module handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the power distribution modules. */
void pdModuleHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("  Power Distribution Module: %d\n",
               currentPowerDistributionModule);
    #endif /* DEBUG_POWERDIS */

    /* Check if the specified submodule is in range */
    currentPdModuleModule=(CAN_ADDRESS&PD_MODULE_MODULES_RCA_MASK)>>PD_MODULE_MODULES_MASK_SHIFT;
    if(currentPdModuleModule>=PD_MODULE_MODULES_NUMBER){
        storeError(ERR_PD_MODULE,
                   0x01); // Error 0x01 -> Power distribution module submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Check if the cartridge corresponding to the selected power distribution
       module is available. If it isn't then notify the incoming CAN message. */
    #ifdef DATABASE_HARDW
       if(frontend.
            cartridge[currentPowerDistributionModule].
             available==UNAVAILABLE){
            storeError(ERR_PD_MODULE,
                       0x02); // Error 0x02 -> corresponding cartridge not installed
            CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
            return;
        }
    #endif /* DATABASE_HARDW */
    /* Call the correct handler */
    (pdModuleModulesHandler[currentPdModuleModule])();
}

/* Power distribution module enable handler */
static void enableHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("   enable\n");
    #endif /* DEBUG_POWERDIS */

    /* If it's a control message (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message location */
        memcpy(&frontend.
                powerDistribution.
                 pdModule[currentPowerDistributionModule].
                  lastEnable,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         powerDistribution.
          pdModule[currentPowerDistributionModule].
           lastEnable.
            status=NO_ERROR;


        /* If we want to turn on a cartridge:
               - make sure that the cartridge is not already ON (this is
                 necessary because we are keeping track of the number of biased
                 cartrdiges):
                   - if it is already on, ignore the message
               - make sure that the max number of powered cartrdiges is not
                 already on
               - perform the operation
               - update the powered cartrdiges variable
               - update the cartrdige state */
        if(CAN_BYTE){

            /* Check if the cartridge is already powered and if it is, just
               return. */
            if((frontend.
                 cartridge[currentPowerDistributionModule].
                  state!=CARTRIDGE_OFF)){
                return;
            }

            /* Check against the max number of cartridge allowed on at any given
               time. If already reached, store the error in the last incoming
               CAN message variable. */
            if(frontend.
                powerDistribution.
                 poweredModules[CURRENT_VALUE]+1 > frontend.
                                                    powerDistribution.
                                                     poweredModules[MAX_SET_VALUE]){
                storeError(ERR_PD_MODULE,
                           0x03); // Error 0x03 -> Max number of powered cartridges already on
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status = HARDW_BLKD_ERR; // Store in the last CAN message variable
                return;
            }

            /* Turn on the cartrdige. */
            if(setPdModuleEnable(PD_MODULE_ENABLE)==ERROR){
                /* Store the Error state in the last control message variable */
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status=ERROR;

                return;
            }

            /* Initialize the cartrdige. If an error occurs, shut off the power
               and return. */
            waitMilliseconds(WAIT_AFTER_POWER_ON); // Dealy to allow power to settle before initializing the cartridge
            if(cartridgeInit(currentPowerDistributionModule)==ERROR){
                if(setPdModuleEnable(PD_MODULE_DISABLE)==ERROR){
                    /* Store the Error state in the last control message
                       variable */
                    frontend.
                     powerDistribution.
                      pdModule[currentPowerDistributionModule].
                       lastEnable.
                        status=ERROR;

                    return;
                }

                /* Store the Error state in the last control message variable */
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status=ERROR;

                return;
            }

            /* Increse the number of currently turned on cartridges. */
            frontend.
             powerDistribution.
              poweredModules[CURRENT_VALUE]++;

            return;
        }

        /* If we want to turn off a cartridge:
               - make sure that the cartridge is not already OFF (this is
                 necessary because we are keeping track of the number of biased
                 cartrdiges):
                   - if it is, ignore the message
               - perform the operation
               - update the powered cartrdiges variable
               - update the cartrdige state */
        /* Check if the cartridge is already powered off and if it is, just
           return. */
        if((frontend.
             cartridge[currentPowerDistributionModule].
              state==CARTRIDGE_OFF)){
            return;
        }

        /*  Turn off the cartrdige. If an error occurs continue with
            powering off. */
        if(cartridgeStop(currentPowerDistributionModule)==ERROR){
            /* Store the Error state in the last control message variable */
            frontend.
             powerDistribution.
              pdModule[currentPowerDistributionModule].
               lastEnable.
                status=ERROR;
        }

        /* Turn off the cartrdige. */
        if(setPdModuleEnable(PD_MODULE_DISABLE)==ERROR){
            /* Store the Error state in the last control message variable */
            frontend.
             powerDistribution.
              pdModule[currentPowerDistributionModule].
               lastEnable.
                status=ERROR;

            return;
        }

        /* Decrease the number of currently turned on cartridges. */
        frontend.
         powerDistribution.
          poweredModules[CURRENT_VALUE]--;

        return;
    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the power distribution module is changed by a control
       message. */
    CAN_BYTE=frontend.
              powerDistribution.
               pdModule[currentPowerDistributionModule].
                enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

