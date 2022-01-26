/*! \file   sisHeater.c
    \brief  SIS heater functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle SIS heater
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"
#include "timer.h"

/* Globals */
/* Externs */
unsigned char   currentSisHeaterModule=0;
/* Statics */
static HANDLER  sisHeaterModulesHandler[SIS_HEATER_MODULES_NUMBER]={enableHandler,
                                                                    currentHandler};

/* SIS Heater handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the SIS heater. */
void sisHeaterHandler(void){

    #ifdef DEBUG
        printf("     SIS Heater\n");
    #endif /* DEBUG */

    /* Check if the selected polarization is outfitted with the desired SIS heater */
    if(frontend.cartridge[currentModule].polarization[currentBiasModule].
           sisHeater.available == UNAVAILABLE) 
    {
        storeError(ERR_SIS_HEATER, ERC_MODULE_ABSENT); //SIS heater not installed
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Check if the submodule is in range */
    currentSisHeaterModule=(CAN_ADDRESS&SIS_HEATER_MODULES_RCA_MASK)>>SIS_HEATER_MODULES_MASK_SHIFT;
    if(currentSisHeaterModule>=SIS_HEATER_MODULES_NUMBER){
        storeError(ERR_SIS_HEATER, ERC_MODULE_RANGE); //SIS heater submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (sisHeaterModulesHandler[currentSisHeaterModule])();
}

/* Heater enable handler */
static void enableHandler(void){

    #ifdef DEBUG
        printf("      Heater Enable\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    polarization[currentBiasModule].
                                     sisHeater.
                                      lastEnable)

        // If we are in STANDBY2 mode, return HARDW_BLKD_ERR
        if (frontend.
             cartridge[currentModule].
              standby2) 
        {
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sisHeater.
                lastEnable.
                 status=HARDW_BLKD_ERR;
            
            return;
        }

        /* Check if message is addressed to band 9 and if it is an enable
           message. If it is, check if the timer has expired. If not, return
           the hardware blocked message. This is necessary to prevent the
           keep-on algorithm from keeping the heater on on band 9. */
        if((currentModule==BAND9)&&CAN_BYTE){
            switch(queryAsyncTimer(TIMER_BIAS_B9_HEATER(currentBiasModule))){
                case TIMER_NOT_RUNNING:
                    /* Start timer */
                    startAsyncTimer(TIMER_BIAS_B9_HEATER(currentBiasModule),
                                    TIMER_BIAS_TO_B9_HEATER,
                                    FALSE);
                    break;
                case TIMER_EXPIRED:
                    /* Reload timer */
                    startAsyncTimer(TIMER_BIAS_B9_HEATER(currentBiasModule),
                                    TIMER_BIAS_TO_B9_HEATER,
                                    TRUE);
                    break;
                case TIMER_RUNNING:
                    /* Mark hardware as blocked */
                    frontend.
                     cartridge[currentModule].
                      polarization[currentBiasModule].
                       sisHeater.
                        lastEnable.
                         status=HARDW_BLKD_ERR;
                    /* Signal error and bail out */
                    storeError(ERR_SIS_HEATER, ERC_HARDWARE_BLOCKED); //Hardware blocked error
                    return;
                    break;
                default:
                    frontend.
                     cartridge[currentModule].
                      polarization[currentBiasModule].
                       sisHeater.
                        lastEnable.
                         status=ERROR;
                    return;
                    break;
            }
        }


        /* Change the status of the SIS heater according to the content of the CAN
           message. */
        if(setSisHeaterEnable(CAN_BYTE?SIS_HEATER_ENABLE:
                                       SIS_HEATER_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sisHeater.
                lastEnable.
                 status=ERROR;

            return;
        }

        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      polarization[currentBiasModule].
                                       sisHeater.
                                        lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Since the change to the hardware introducing the automatic shutoff of the
       SIS heater after 1 second (starting with Rev.D2 of the BIAS mdoule), this
       monitor point is no longer meaningful since is just the repetition of the
       monitor on the control RCA. */
    storeError(ERR_SIS_HEATER, ERC_RCA_RANGE); //Monitor message out of range
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;
}

/* Heater current handler */
/* This function deals with all the monitor requests directed to the sis heater
   current. There are no control messages allowed for the heater current. */
static void currentHandler(void){

    #ifdef DEBUG
        printf("      Heater Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_SIS_HEATER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_SIS_HEATER, ERC_RCA_RANGE); //Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;
       return;
    }

    /* Monitor the SIS heater current */
    if(getSisHeater()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sisHeater.
                      current;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sisHeater.
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
