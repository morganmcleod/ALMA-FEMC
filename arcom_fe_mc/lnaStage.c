/*! \file   lnaStage.c
    \brief  LNA Stage functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle the LNA Stage
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "can.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentLnaStageModule=0;
/* Statics */
static HANDLER  lnaStageModulesHandler[LNA_STAGE_MODULES_NUMBER]={drainVoltageHandler,
                                                                  drainCurrentHandler,
                                                                  gateVoltageHandler};

/* LNA stage Channel handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the LNA Stage.
    The function will perform a check for the existance of the addressed
    hardware then call the appropriate handler. */
void lnaStageHandler(void){

    #ifdef DEBUG
        printf("       LNA Stage: %d (currentLnaModule)\n",
               currentLnaModule);
    #endif /* DEBUG */
    
    /* Check if the submodule is in range */
    currentLnaStageModule=(CAN_ADDRESS&LNA_STAGE_MODULES_RCA_MASK);
    if(currentLnaStageModule>=LNA_STAGE_MODULES_NUMBER)
    {

        storeError(ERR_LNA_STAGE, ERC_MODULE_RANGE); //LNA stage submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }
    /* Call the correct handler */
    (lnaStageModulesHandler[currentLnaStageModule])();
}

/* Drain Voltage Handler */
/* This function will deal with monitor and control requests to the drain
   voltage of the addressed stage. */
static void drainVoltageHandler(void){

    #ifdef DEBUG
        printf("        Drain Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    polarization[currentBiasModule].
                                     sideband[currentPolarizationModule].
                                      lna.
                                       stage[currentLnaModule].
                                        lastDrainVoltage)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.
             cartridge[currentModule].
              standby2) 
        {            
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                lna.
                 stage[currentLnaModule].
                  lastDrainVoltage.
                   status=HARDW_BLKD_ERR;
            
            return;
        }

        /* Set the lna stage. If an error occurs then store the state and
           then return. */
        if(setLnaStage()==ERROR){
            /* Store the ERROR state in the last control message varibale */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                lna.
                 stage[currentLnaModule].
                  lastDrainVoltage.
                   status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      polarization[currentBiasModule].
                                       sideband[currentPolarizationModule].
                                        lna.
                                         stage[currentLnaModule].
                                          lastDrainVoltage)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the LNA stage drain voltage */
    if(getLnaStage()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainVoltage;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainVoltage;
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}


/* Drain Current Handler */
/* This function will deal with monitor and control request to the drain
   current of the addressed stage. */
static void drainCurrentHandler(void){

    #ifdef DEBUG
        printf("        Drain Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    polarization[currentBiasModule].
                                     sideband[currentPolarizationModule].
                                      lna.
                                       stage[currentLnaModule].
                                        lastDrainCurrent)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.
             cartridge[currentModule].
              standby2) 
        {
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                lna.
                 stage[currentLnaModule].
                  lastDrainCurrent.
                   status=HARDW_BLKD_ERR;
            
            return;
        }
        /* Set the lna stage. If and error occurs then store the state and
           return the error state then return. */
        if(setLnaStage()==ERROR){
            /* Store the ERROR state in the last control message varibale */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                lna.
                 stage[currentLnaModule].
                  lastDrainCurrent.
                   status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      polarization[currentBiasModule].
                                       sideband[currentPolarizationModule].
                                        lna.
                                         stage[currentLnaModule].
                                          lastDrainCurrent)
        return;
    }


    /* If monitor on a monitor RCA */
    /* Monitor the LNA stage drain current */
    if(getLnaStage()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainCurrent;
    } else {

        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainCurrent;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}


/* Gate Voltage Handler */
/* This function will deal with monitor requests to the gate voltage of the
   addressed stage. */
static void gateVoltageHandler(void){

    #ifdef DEBUG
        printf("        Gate Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_LNA_STAGE, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LNA_STAGE, ERC_RCA_RANGE); //Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;

       return;
    }

    /* Monitor the LNA stage gate voltage */
    if(getLnaStage()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        gateVoltage;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        gateVoltage;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}
