/*! \file   pdSerialInterface.c
    \brief  Power distribution serial interface functions

    <b> File Informations: </b><br>
    Created: 2006/10/18 10:11:13 by avaccari

    This file contains all the functions necessary to control and operate the
    power distribution serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote power distribution monitor and control
    interface.
    The fucntions in this module will perform a series of register manipulations
    and remote access operation to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

/* Includes */
#include <stdio.h>      /* printf */

#include "pdSerialInterface.h"
#include "debug.h"
#include "error.h"
#include "frontend.h"
#include "serialInterface.h"
#include "timer.h"

/* Globals */
/* Externs */
/* Statics */
PD_REGISTERS pdRegisters;

/* Power distribution analog monitor request core.
   This function performs the core operations that are common to all the analog
   monitor request for the power distribution module:
       - Write the power distribution BREG with a parallel output write cycle
       - Initiate an ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle to get the raw data

   If an error happens during the process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getPdAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of with the first two are
       to be ignored. This variable allowes manipulation of data so thata the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];

    /* Parallel write BREG */
    #ifdef DEBUG_POWERDIS_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_POWERDIS_SERIAL */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(PD_PARALLEL_WRITE(PD_BREG),
                    &pdRegisters.
                      bReg.
                       integer,
                    PD_BREG_SIZE,
                    PD_BREG_SHIFT_SIZE,
                    PD_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Initiate ADC conversion:
       - send ADC convert strobe command */
    #ifdef DEBUG_POWERDIS_SERIAL
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG_POWERDIS_SERIAL */

    /* If an error occurs, notify the calling function */
    if(serialAccess(PD_ADC_CONVERT_STROBE,
                    NULL,
                    PD_ADC_STROBE_SIZE,
                    PD_ADC_STROBE_SHIFT_SIZE,
                    PD_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status:
       - parallel input */
    /* Setup for 1 second and start the asynchronous timer */
    if(startAsyncTimer(TIMER_PD_ADC_RDY,
                       TIMER_PD_TO_ADC_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG_POWERDIS_SERIAL
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG_POWERDIS_SERIAL */

        /* If an error occurs, notify the calling function */
        if(serialAccess(PD_PARALLEL_READ,
                        &pdRegisters.
                          statusReg.
                           integer,
                        PD_STATUS_REG_SIZE,
                        PD_STATUS_REG_SHIFT_SIZE,
                        PD_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){

            /* Stop the timer */
            if(stopAsyncTimer(TIMER_PD_ADC_RDY)==ERROR){
                return ERROR;
            }

            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_PD_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((pdRegisters.
               statusReg.
                bitField.
                 adcReady==PD_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_PD_SERIAL, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer */
    if(stopAsyncTimer(TIMER_PD_ADC_RDY)==ERROR){
        return ERROR;
    }

    /* ADC read cycle */
    #ifdef DEBUG_POWERDIS_SERIAL
        printf("         - Reading ADC value\n");
    #endif /* DEBUG_POWERDIS_SERIAL */

    /* If error return the state to the calling function */
    if(serialAccess(PD_ADC_DATA_READ,
                    &tempAdcValue,
                    PD_ADC_DATA_SIZE,
                    PD_ADC_DATA_SHIFT_SIZE,
                    PD_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    pdRegisters.
     adcData = (unsigned int)tempAdcValue[0];

    return NO_ERROR;
}

/* Set Power Distribution Module Enable */
/*! This function controls the enable state of the currently addressed power
    distribution module.

    This function will perform the following operations:
        -# Perform a parallel write of the new AREG
        -# If no error occurs, update AREG and the frontend variable with the
           new state

    \param enable   This is the state to set the module to:
                        - \ref PD_MODULE_ENABLE     -> Enable the power
                                                       distribution module
                        - \ref PD_MODULE_DISABLE    -> Disable the power
                                                       distribution moduel

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setPdModuleEnable(unsigned char enable){

    /* Store the current value of the power distribution module enable state in
       a temporary variable so that if any error occurs during the update of
       the hardware state, we don't end up with an AREG describing a different
       state than the hardware one. */
    int tempAReg=pdRegisters.
                  aReg;

    /* Update AREG */
    pdRegisters.
     aReg=(enable==PD_MODULE_ENABLE)?SET_PD_MODULE_ENABLE(currentPowerDistributionModule):
                                     SET_PD_MODULE_DISABLE(currentPowerDistributionModule);

    /* 2 - Parallel write AREG */
    #ifdef DEBUG_POWERDIS_SERIAL
        printf("         - Writing AREG\n");
    #endif /* DEBUG_POWERDIS_SERIAL */

    /* If there is a problem writing AREG, restore AREG and return the ERROR */
    if(serialAccess(PD_PARALLEL_WRITE(PD_AREG),
                    &pdRegisters.
                      aReg,
                    PD_AREG_SIZE,
                    PD_AREG_SHIFT_SIZE,
                    PD_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore AREG to its original saved value */
        pdRegisters.
         aReg = tempAReg;

        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     powerDistribution.
      pdModule[currentPowerDistributionModule].
       enable=(enable==PD_MODULE_ENABLE)?PD_MODULE_ENABLE:
                                                        PD_MODULE_DISABLE;

    return NO_ERROR;
}

/* Get Power Distribution Channel */
/*! This function monitors the voltage and current of the currently selected
    power distribution channel.

    This function will perform the following operations:
        -# Set the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the power distribution module.
        -# Scale the raw binary data with the correct unit and store the results
           in the \ref frontend variable.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPdChannel(void){

    /* A float to hold the scaling factor */
    float scale=0.0;

    /* Clear the power distribution BREG */
    pdRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    pdRegisters.
     bReg.
      bitField.
       monitorBand=PD_BREG_SELECT_BAND(currentPowerDistributionModule);
    pdRegisters.
     bReg.
      bitField.
       monitorPoint=PD_BREG_SELECT_MONITOR(currentPdModuleModule,
                                           currentPdChannelModule);

    /* 2->5 Call the getPdAnalogMonitor function */
    if(getPdAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(currentPdChannelModule){
        case PD_CHANNEL_CURRENT:
            switch(currentPdModuleModule){
                case PLUS_6:
                    scale=PD_ADC_PLUS_6I_SCALE;
                    break;
                case MINUS_6:
                    scale=PD_ADC_MINUS_6I_SCALE;
                    break;
                case PLUS_15:
                    scale=PD_ADC_PLUS_15I_SCALE;
                    break;
                case MINUS_15:
                    scale=PD_ADC_MINUS_15I_SCALE;
                    break;
                case PLUS_24:
                    scale=PD_ADC_PLUS_24I_SCALE;
                    break;
                case PLUS_8:
                    scale=PD_ADC_PLUS_8I_SCALE;
                    break;
                default:
                    break;
            }
            frontend.
             powerDistribution.
              pdModule[currentPowerDistributionModule].
               pdChannel[currentPdModuleModule].
                current=(scale*pdRegisters.
                                               adcData)/PD_ADC_RANGE;
            break;
        case PD_CHANNEL_VOLTAGE:
            switch(currentPdModuleModule){
                case PLUS_6:
                    scale=PD_ADC_PLUS_6V_SCALE;
                    break;
                case MINUS_6:
                    scale=PD_ADC_MINUS_6V_SCALE;
                    break;
                case PLUS_15:
                    scale=PD_ADC_PLUS_15V_SCALE;
                    break;
                case MINUS_15:
                    scale=PD_ADC_MINUS_15V_SCALE;
                    break;
                case PLUS_24:
                    scale=PD_ADC_PLUS_24V_SCALE;
                    break;
                case PLUS_8:
                    scale=PD_ADC_PLUS_8V_SCALE;
                    break;
                default:
                    break;
            }
            frontend.
             powerDistribution.
              pdModule[currentPowerDistributionModule].
               pdChannel[currentPdModuleModule].
                voltage=(scale*pdRegisters.
                                               adcData)/PD_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}

