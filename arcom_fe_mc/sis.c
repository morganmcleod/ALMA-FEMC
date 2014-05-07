/*! \file   sis.c
    \brief  SIS functions
    \todo   Load the value of resistor somewhere during initialization. All the
            initializations should be performed in the same module if possible.

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sis.c,v 1.26 2009/08/25 21:39:39 avaccari Exp $

    This files contains all the functions necessary to handle SIS events. This
    module will perform the followng operations.
        - On reception of a CAN message it will check the availability of the
          addressed module. If the module is available the execution will be
          redirect the flow of the program to the appropriate handling function.
        - Within the handling functions different operations will be performed
          depending if the message is:
              - <em>Monitor on monitor RCA.</em><br>
                If the message is a monitor message on a monitor relative CAN
                address:
                    - The requested data is acquired from the hardware.
                    - The returned value is checked against the <i>warning</i>
                      and <i>error</i> limits.
                    - If in error some operation might be performed depending
                      on the characteristics of the hardware.
                    - The data is stored in a CAN message ready to be returned.
              - <em>Monitor on control RCA.</em><br>
                If the message is a monitor message on a control relative CAN
                address:
                    - The previously issued control request is returned.
              - <em>Control message.</em><br>
                If the message is a control relative CAN address:
                    - The content of the message is stored for future monitor
                      requests on the same RCA.
                    - The data is extracted from the CAN message.
                    - The data is checked against available limits.
                    - If the data is within the allowed limits, it is written to
                      the hardware.
                    - If no error occurs while writing to the hardware, the
                      correspondent value in the global variable frontend is
                      updated. */

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
unsigned char   currentSisModule=0;
/* Statics */
static HANDLER  sisModulesHandler[SIS_MODULES_NUMBER]={voltageHandler,
                                                       currentHandler,
                                                       openLoopHandler};

/* SIS handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the SIS. */
void sisHandler(void){

    #ifdef DEBUG
        printf("      SIS\n");
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected sideband is outfitted with the desired SIS */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sideband[currentPolarizationModule].
               sis.
                available==UNAVAILABLE){
            storeError(ERR_SIS,
                       0x01); // Error 0x01 -> SIS not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    /* The -1 is necessary because, since we cannot use a RCA of 0, the lowest
       value that we are going to receive is 1. The handler array is still
       initialized at 0 so whe have to shift the index down by one. */
    currentSisModule=((CAN_ADDRESS&SIS_MODULES_RCA_MASK)>>SIS_MODULES_MASK_SHIFT)-1;

    if(currentSisModule>=SIS_MODULES_NUMBER){
        storeError(ERR_SIS,
                   0x02); // Error 0x02 -> SIS submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (sisModulesHandler[currentSisModule])();
}

/* SIS voltage handler */
/* This function will deal with all the monitor and control requests directed
   to the SIS voltage. */
static void voltageHandler(void){

    #ifdef DEBUG
        printf("       Voltage\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sis.
                     lastVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            sis.
             lastVoltage.
              status=NO_ERROR;

        /* Extract the floating data from the CAN message */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               voltage[MIN_SET_VALUE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               voltage[MAX_SET_VALUE])){
                storeError(ERR_SIS,
                           0x03); // Error 0x03: Voltage set value out of range

                /* Store the error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sis.
                     lastVoltage.
                      status=CON_ERROR_RNG;
                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the SIS mixer bias voltage. If an error occurs, store the state
           and then return. */
        if(setSisMixerBias()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                sis.
                 lastVoltage.
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
                    sis.
                     lastVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on monitor RCA */
    /* Monitor the SIS mixer voltage */
    if(getSisMixerBias(SIS_MIXER_BIAS_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sis.
                       voltage[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sis.
                       voltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out an warning/error message depending on
           the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               voltage[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               voltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sis.
                                   voltage[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sis.
                                   voltage[HI_ERROR_RANGE])){
                    storeError(ERR_SIS,
                               0x04); // Error 0x04: Error: SIS voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SIS,
                               0x05); // Error 0x05: Warning: SIS voltage in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). It is done directly instead of using a function to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* SIS current handler */
/* This function will handle all the monitor requests directed to the junction
   current. No control messages are allowed for the junction current. */
static void currentHandler(void){

    #ifdef DEBUG
        printf("       Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_SIS,
                   0x08); // Error 0x08: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_SIS,
                   0x09); // Error 0x09: Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;

       return;
    }

    /* Monitor the SIS mixer current */
    if(getSisMixerBias(SIS_MIXER_BIAS_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sis.
                       current[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sideband[currentPolarizationModule].
                      sis.
                       current[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               current[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              sis.
                               current[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sis.
                                   current[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sideband[currentPolarizationModule].
                                  sis.
                                   current[HI_ERROR_RANGE])){
                    storeError(ERR_SIS,
                               0x06); // Error 0x06: Error: SIS current in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SIS,
                               0x07); // Error 0x07: Warning: SIS current in error range.
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
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* SIS open loop handler */
static void openLoopHandler(void){

    #ifdef DEBUG
        printf("       Open Loop\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sis.
                     lastOpenLoop,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sideband[currentPolarizationModule].
            sis.
             lastOpenLoop.
              status=NO_ERROR;

        /* Change the status of the loop according to the content of the CAN
           message. */
        if(setSisMixerLoop(CAN_BYTE?SIS_MIXER_BIAS_MODE_OPEN:
                                    SIS_MIXER_BIAS_MODE_CLOSE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                sis.
                 lastOpenLoop.
                  status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sideband[currentPolarizationModule].
                    sis.
                     lastOpenLoop,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               polarization[currentBiasModule].
                sideband[currentPolarizationModule].
                 sis.
                  openLoop;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}
