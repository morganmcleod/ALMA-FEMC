/*! \file   sisMagnet.c
    \brief  SIS magnet functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sisMagnet.c,v 1.15 2006/09/15 14:28:27 avaccari Exp $

    This files contains all the functions necessary to handle SIS magnet
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "biasSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentSisMagnetModule=0;
/* Statics */
static HANDLER  sisMagnetModulesHandler[SIS_MAGNET_MODULES_NUMBER]={voltageHandler,
                                                                    currentHandler};

/* SIS magnet handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the SIS magnet. */
void sisMagnetHandler(void){

    #ifdef DEBUG
        printf("      SIS magnet\n");
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected sideband is outfitted with the desired SIS magnet */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sideband[currentPolarizationModule].
               sisMagnet.
                available==UNAVAILABLE){
            storeError(ERR_SIS_MAGNET,
                       0x01); // Error 0x01 -> SIS magnet not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    currentSisMagnetModule=(CAN_ADDRESS&SIS_MAGNET_MODULES_RCA_MASK)>>SIS_MAGNET_MODULES_MASK_SHIFT;
    if(currentSisMagnetModule>=SIS_MAGNET_MODULES_NUMBER){
        storeError(ERR_SIS_MAGNET,
                   0x02); // Error 0x02 -> SIS magnet submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (sisMagnetModulesHandler[currentSisMagnetModule])();
}

/* SIS magnet voltage handler */
/* This function deals with all the monitor requests directed to the sis magnet
   voltage. There are no control messages allowed for the magnet voltage. */
static void voltageHandler(void){

    /* Monitor the SIS magnet voltage */
    #ifdef DEBUG
        printf("       Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_SIS_MAGNET,
                   0x08); // Error 0x08: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_SIS_MAGNET,
                   0x09); // Error 0x09: Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;

       return;
    }

    /* Monitor the SIS magnet BIAS voltage */
    if(getSisMagnetBias(SIS_MAGNET_BIAS_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sisMagnet.
                       voltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sisMagnet.
                       voltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sisMagnet.
                               voltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sisMagnet.
                               voltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sisMagnet.
                                   voltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sisMagnet.
                                   voltage[HI_ERROR_RANGE])){
                    storeError(ERR_SIS_MAGNET,
                               0x03); // Error 0x03: Error: SIS Magnet voltage in error range.
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SIS_MAGNET,
                               0x04); // Error 0x04: Warning: SIS Magnet voltage in error range.
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


/* SIS magnet current handler */
/* This function deals with all the monitor and control request directed to the
   SIS magnet current. */
static void currentHandler(void){

    #ifdef DEBUG
        printf("       Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sisMagnet.
                     lastCurrent,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            sisMagnet.
             lastCurrent.
              status=NO_ERROR;

              /* Extract the floating data from the CAN message */
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
                              sisMagnet.
                               current[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sisMagnet.
                               current[MAX_SET_VALUE])){
                storeError(ERR_SIS_MAGNET,
                           0x07); // Error 0x07: Current set value out of range
                /* Store the error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sisMagnet.
                     lastCurrent.
                      status=CON_ERROR_RNG;
                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the SIS magnet bias current. If an error occurs, then store the
           state and report the error. */
        if(setSisMagnetBias()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                sisMagnet.
                 lastCurrent.
                  status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }


    /* If Monitor on Control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sisMagnet.
                     lastCurrent,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the SIS Magnet current */
    if(getSisMagnetBias(SIS_MAGNET_BIAS_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sisMagnet.
                       current[CURRENT_VALUE];
    } else {
        /* if no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sisMagnet.
                       current[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sisMagnet.
                               current[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sisMagnet.
                               current[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sisMagnet.
                                   current[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sisMagnet.
                                   current[HI_ERROR_RANGE])){
                    storeError(ERR_SIS_MAGNET,
                               0x05); // Error 0x05: Error: SIS Magnet current in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SIS_MAGNET,
                               0x06); // Error 0x06: Warning: SIS Magnet current in warning range.
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

