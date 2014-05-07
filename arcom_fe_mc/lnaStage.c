/*! \file   lnaStage.c
    \brief  LNA Stage functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lnaStage.c,v 1.24 2007/06/08 22:48:57 avaccari Exp $

    This files contains all the functions necessary to handle the LNA Stage
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "can.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"
#include "database.h"

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

    #ifdef DATABASE_HARDW
        /* Check if the selected LNA is outfitted with the desired stage */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sideband[currentPolarizationModule].
               lna.
                stage[currentLnaModule].
                 available==UNAVAILABLE){

            storeError(ERR_LNA_STAGE,
                       0x01); // Error 0x01 -> LNA stage not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    currentLnaStageModule=(CAN_ADDRESS&LNA_STAGE_MODULES_RCA_MASK);
    if(currentLnaStageModule>=LNA_STAGE_MODULES_NUMBER){

        storeError(ERR_LNA_STAGE,
                   0x02); // Error 0x02 -> LNA stage submodule out of range

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
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            lna.
             stage[currentLnaModule].
              lastDrainVoltage.
               status=NO_ERROR;

        /* Extract the float from the can message. */
        changeEndian(convert.
                      chr,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainVoltage[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainVoltage[MAX_SET_VALUE])){

                storeError(ERR_LNA_STAGE,
                           0x09); // Error 0x09: Drain voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainVoltage.
                       status=CON_ERROR_RNG;
                return;
            }
        #endif /* DATABASE_RANGE */

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
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the LNA stage drain voltage */
    if(getLnaStage()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainVoltage[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  lna.
                                   stage[currentLnaModule].
                                    drainVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  lna.
                                   stage[currentLnaModule].
                                    drainVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_LNA_STAGE,
                               0x05); // Error 0x05: Error: LNA stage drain voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_LNA_STAGE,
                               0x06); // Error 0x06: Warning: LNA stage drain voltage in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 convert.
                  chr);
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
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainCurrent,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            lna.
             stage[currentLnaModule].
              lastDrainCurrent.
               status=NO_ERROR;

        /* Extract the float from the can message. */
        changeEndian(convert.
                      chr,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainCurrent[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainCurrent[MAX_SET_VALUE])){

                storeError(ERR_LNA_STAGE,
                           0x0A); // Error 0x0A: Drain current set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainCurrent.
                       status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

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
        /* Return last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    lna.
                     stage[currentLnaModule].
                      lastDrainCurrent,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }


    /* If monitor on a monitor RCA */
    /* Monitor the LNA stage drain current */
    if(getLnaStage()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainCurrent[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        drainCurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainCurrent[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                drainCurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  lna.
                                   stage[currentLnaModule].
                                    drainCurrent[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  lna.
                                   stage[currentLnaModule].
                                    drainCurrent[HI_ERROR_RANGE])){
                    storeError(ERR_LNA_STAGE,
                               0x07); // Error 0x05: Error: LNA stage drain current in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_LNA_STAGE,
                               0x08); // Error 0x06: Warning: LNA stage drain current in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 convert.
                  chr);
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
        storeError(ERR_LNA_STAGE,
                   0x10); // Error 0x10: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LNA_STAGE,
                   0x11); // Error 0x11: Monitor message out of range
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
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        gateVoltage[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      lna.
                       stage[currentLnaModule].
                        gateVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                gateVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                gateVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  lna.
                                   stage[currentLnaModule].
                                    gateVoltage[LOW_ERROR_RANGE],
                             CAN_FLOAT,
                             frontend.
                              cartridge[currentModule].
                               polarization[currentBiasModule].
                                sideband[currentPolarizationModule].
                                 lna.
                                  stage[currentLnaModule].
                                   gateVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_LNA_STAGE,
                               0x03); // Error 0x03: Error: LNA stage gate voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_LNA_STAGE,
                               0x04); // Error 0x04: Warning: LNA stage gate voltage in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}


