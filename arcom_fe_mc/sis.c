/*! \file   sis.c
    \brief  SIS functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle SIS events. This
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

/* Globals */
/* Externs */
unsigned char   currentSisModule=0;
/* Statics */
static HANDLER  sisModulesHandler[SIS_MODULES_NUMBER]={senseResistorHandler,
                                                       voltageHandler,
                                                       currentHandler,
                                                       openLoopHandler};

/* SIS handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the SIS. */
void sisHandler(void){

    #ifdef DEBUG
        printf("      SIS\n");
    #endif /* DEBUG */

    /* Check if the selected sideband is outfitted with the desired SIS */
    if(frontend.cartridge[currentModule].polarization[currentBiasModule].
           sideband[currentPolarizationModule].sis.available == UNAVAILABLE)
    {
        storeError(ERR_SIS, ERC_MODULE_ABSENT); //SIS not installed
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Check if the submodule is in range */
    /* Note that this monitor RCA is not supported for band 1 = RCA 0x00000 */
    currentSisModule=((CAN_ADDRESS&SIS_MODULES_RCA_MASK)>>SIS_MODULES_MASK_SHIFT);

    if(currentSisModule>=SIS_MODULES_NUMBER){
        storeError(ERR_SIS, ERC_MODULE_RANGE); //SIS submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (sisModulesHandler[currentSisModule])();
}

/* SIS sense resistor handler */
static void senseResistorHandler(void) {
    unsigned char pol, sb;
    #ifdef DEBUG
        printf("       senseResistor\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.lastSenseResistor)

        /* Extract the floating data from the CAN message */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);

        /* Save the new value to all four sidebands */
        for(pol = 0; pol < POLARIZATIONS_NUMBER; pol++) {
            for(sb = 0; sb < SIDEBANDS_NUMBER; sb++) {
                /* SIS current sense resistor */
                frontend.cartridge[currentModule].polarization[pol].sideband[sb].
                    sis.resistor = CONV_FLOAT;
            }
        }
        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.lastSenseResistor)
        return;
    }

    /* If monitor on monitor RCA */
    /* Return the value from memory */
    CONV_FLOAT=frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.resistor;

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). It is done directly instead of using a function to save some time. */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* SIS voltage handler */
/* This function will deal with all the monitor and control requests directed
   to the SIS voltage. */
static void voltageHandler(void){

    #ifdef DEBUG
        printf("       Voltage\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.lastVoltage)

        /* Extract the floating data from the CAN message */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.cartridge[currentModule].standby2) {
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
               sideband[currentPolarizationModule].sis.lastVoltage.status=HARDW_BLKD_ERR;
            return;
        }

        /* Set the SIS mixer bias voltage. If an error occurs, store the state
           and then return. */
        if(setSisMixerBias()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].sis.lastVoltage.status=ERROR;
            return;
        }
        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS) {
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
                                        sideband[currentPolarizationModule].sis.lastVoltage)
        return;
    }

    /* If monitor on monitor RCA */
    /* Monitor the SIS mixer voltage */
    if(getSisMixerBias(SIS_MIXER_BIAS_VOLTAGE) == ERROR) {
        /* If error during monitoring, store the ERROR state in the outgoing
           can message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.voltage;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.voltage;
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). It is done directly instead of using a function to save some time. */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
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
    if(CAN_SIZE) {
        storeError(ERR_SIS, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS) { // If monitor on a control RCA
        storeError(ERR_SIS, ERC_RCA_RANGE); //Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;
       return;
    }

    /* Monitor the SIS mixer current */
    if(getSisMixerBias(SIS_MIXER_BIAS_CURRENT)==ERROR) {
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.current;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.current;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* SIS open loop handler */
static void openLoopHandler(void){

    #ifdef DEBUG
        printf("       Open Loop\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.lastOpenLoop)

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.cartridge[currentModule].standby2) {
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].sis.lastOpenLoop.status=HARDW_BLKD_ERR;
            return;
        }

        /* Change the status of the loop according to the content of the CAN
           message. */
        if(setSisMixerLoop(CAN_BYTE ? SIS_MIXER_BIAS_MODE_OPEN : SIS_MIXER_BIAS_MODE_CLOSE) == ERROR) {
            /* Store the ERROR state in the last control message variable */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].sis.lastOpenLoop.status=ERROR;
            return;
        }
        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].polarization[currentBiasModule].
            sideband[currentPolarizationModule].sis.lastOpenLoop)
        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.cartridge[currentModule].polarization[currentBiasModule].
        sideband[currentPolarizationModule].sis.openLoop;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

// set the specified SIS to STANDBY2 mode
void sisGoStandby2() {
    int ret;

    /* Check if the selected sideband is outfitted with the desired SIS */
    if(frontend.cartridge[currentModule].polarization[currentBiasModule].
           sideband[currentPolarizationModule].sis.available == UNAVAILABLE) 
    {
        // nothing to do:
        return;
    }

    ret = 0;

    #ifdef DEBUG_GO_STANDBY2
        printf(" - sisGoStandby2 pol=%d sb=%d\n", currentBiasModule, currentPolarizationModule);
    #endif // DEBUG_GO_STANDBY2

    // set the SIS voltage to 0:
    CONV_FLOAT = 0.0;
    ret = setSisMixerBias();

    #ifdef DEBUG_GO_STANDBY2
        if (ret)
            printf(" -- ret=%d\n", ret);
    #endif // DEBUG_GO_STANDBY2
}
