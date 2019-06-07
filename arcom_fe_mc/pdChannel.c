/*! \file   pdChannel.c
    \brief  Power Distribution Channel functions

    <b> File Informations: </b><br>
    Created: 2006/10/1 14:56:51 by avaccari

    This file contains all the functions necessary to handle the power
    distribution submodules' channels. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "pdSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentPdChannelModule=0;
/* Statics */
static HANDLER  pdChannelModulesHandler[PD_CHANNEL_MODULES_NUMBER]={currentHandler,
                                                                    voltageHandler};

/* Power distribution channel handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the power distribution channel modules. */
void pdChannelHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("   Power Distribution Channel: %d\n",
               currentPdModuleModule );
    #endif /* DEBUG_POWERDIS */

    /* Since each power distribution channel is always outfitted with all the
       modules, no hardware check is performed. */

    /* Check if the specified submodule is in range */
    currentPdChannelModule=(CAN_ADDRESS&PD_CHANNEL_MODULES_RCA_MASK);
    if(currentPdChannelModule>=PD_CHANNEL_MODULES_NUMBER){
        storeError(ERR_PD_CHANNEL, ERC_MODULE_RANGE); //Power distribution channel submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct handler */
    (pdChannelModulesHandler[currentPdChannelModule])();
}

/* Power distribution channel voltage handler */
static void voltageHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("    Voltage\n");
    #endif /* DEBUG_POWERDIS */

    /* If control (size!=0) sotre error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PD_CHANNEL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowd on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PD_CHANNEL, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the voltage for the desired channel */
    if(getPdChannel()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   powerDistribution.
                    pdModule[currentPowerDistributionModule].
                     pdChannel[currentPdModuleModule].
                      voltage;
    } else {
        /* If no error during monitor pocess, gather the stored data */
        CONV_FLOAT=frontend.
                   powerDistribution.
                    pdModule[currentPowerDistributionModule].
                     pdChannel[currentPdModuleModule].
                      voltage;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Power distribution channel current handler */
static void currentHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("    Current\n");
    #endif /* DEBUG_POWERDIS */

    /* If control (size!=0) sotre error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PD_CHANNEL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowd on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PD_CHANNEL, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the current for the desired channel */
    if(getPdChannel()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   powerDistribution.
                    pdModule[currentPowerDistributionModule].
                     pdChannel[currentPdModuleModule].
                      current;
    } else {
        /* If no error during monitor pocess, gather the stored data */
        CONV_FLOAT=frontend.
                   powerDistribution.
                    pdModule[currentPowerDistributionModule].
                     pdChannel[currentPdModuleModule].
                      current;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
