/*! \file   powerDistribution.c
    \brief  Power distribution functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>

    \$Id: powerDistribution.c,v 1.18 2010/11/02 14:36:29 avaccari Exp $


    This files contains all the functions necessary to handle power distribution
    events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "pdSerialInterface.h"


/* Globals */
/* Externs */
unsigned char   currentPowerDistributionModule=0;
/* Statics */
static HANDLER powerDistributionModulesHandler[POWER_DISTRIBUTION_MODULES_NUMBER]={pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   pdModuleHandler,
                                                                                   poweredModulesHandler};

/* Power distribution handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the power distribution. */
void powerDistributionHandler(void){

    #ifdef DEBUG_POWERDIS
        printf(" Power Distribution\n");
    #endif /* DEBUG_POWERDIS */

    /* Check if the specified submodule is in range */
    currentPowerDistributionModule=(CAN_ADDRESS&POWER_DISTRIBUTION_MODULES_RCA_MASK)>>POWER_DISTRIBUTION_MODULES_MASK_SHIFT;
    if(currentPowerDistributionModule>=POWER_DISTRIBUTION_MODULES_NUMBER){
        storeError(ERR_POWER_DISTRIBUTION,
                   0x01); // Error 0x01 -> Power distribution submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (powerDistributionModulesHandler[currentPowerDistributionModule])();
}

/* Powered modules handler */
static void poweredModulesHandler(void){

    #ifdef DEBUG_POWERDIS
        printf("  Powered Modules\n");
    #endif /* DEBUG_POWERDIS */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_POWER_DISTRIBUTION,
            0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_POWER_DISTRIBUTION,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the number of powered modules. This is a global variable always
       available and it doesn't require serial communication so we return it
       immediately. The value is strictly controlled when cartridges power state
       is changed so there is no need to control it here. */
    CAN_BYTE=frontend.
              powerDistribution.
               poweredModules[CURRENT_VALUE];
    CAN_SIZE=CAN_BYTE_SIZE;

}

/* Power distribution Initialization */
/*! This function performs all the necessary initialization for the power
    distribution system. These are executed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int powerDistributionStartup(void){

    /* Set the currentModule variable to reflect the fact that the CPDS is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=POWER_DIST_MODULE;

    printf(" Initializing Power Distribution System...\n\n");

    /* Power down all the cartridges to prevent misalignment between software
       status and hardware status. */
    powerDistributionStop();

    /* Now we can proceed with initialization */

    frontend.
     powerDistribution.
      poweredModules[MAX_SET_VALUE]=(frontend.
                                      mode[CURRENT_VALUE]==TROUBLESHOOTING_MODE)?MAX_POWERED_BANDS_TROUBLESHOOTING:
                                                                                 MAX_POWERED_BANDS_OPERATIONAL;

    printf(" done!\n\n");

    return NO_ERROR;
}

/* Power distribution shutdown */
/*! This function performs all the functions necessary to stop the power
    distribution system.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int powerDistributionStop(void){

    /* Set the currentModule variable to reflect the fact that the CPDS is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=POWER_DIST_MODULE;

    printf(" Powering down Power Distribution System...\n");

    /* Unconditionally turn off all the modules */
    for(currentPowerDistributionModule=0;
        currentPowerDistributionModule<CARTRIDGES_NUMBER;
        currentPowerDistributionModule++){
        printf(" - Powering down module: %d...",
               currentPowerDistributionModule);
        setPdModuleEnable(PD_MODULE_DISABLE);
        printf(" done!\n");
    }

    printf(" done!\n\n");

    return NO_ERROR;
}
