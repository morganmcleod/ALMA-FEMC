/*! \file   ifSerialInterface.c
    \brief  IF Switch serial interface functions

    <b> File information: </b><br>
    Created: 2006/11/30 16:49:28 by avaccari

    This file contains all the functions necessary to control and operate the
    IF Switch serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote IF Switch monitor and control interface.
    The functions in this module will perform a series of register manipulations
    and remote access operations to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */
#include <math.h>       /* log */
#include <errno.h>      /* errno */

#include "ifSerialInterface.h"
#include "error.h"
#include "serialInterface.h"
#include "timer.h"
#include "frontend.h"
#include "debug.h"

/* Globals */
/* Externs */
/* Statics */
IF_REGISTERS ifRegisters;

/* IF switch analog monitor request core.
   This function performs the core operations that are common to all the analog
   monitor request for the IF switch module:
       - Write the IF switch GREG with a parallel output write cycle
       - Initialte an ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle to get the raw data

   If an error happens during he process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getIfAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of with the first two are
       to be ignored. This variable allowes manipulation of data so thata the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];

    /* Parallel write GREG */
    #ifdef DEBUG_IFSWITCH_SERIAL
        printf("         - Writing GREG\n");
    #endif /* DEBUG_IFSWITCH_SERIAL */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(IF_PARALLEL_WRITE(IF_GREG),
                    &ifRegisters.
                      gReg,
                    IF_GREG_SIZE,
                    IF_GREG_SHIFT_SIZE,
                    IF_GREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Initiate ADC conversion:
       - send ADC convert strobe command */
    #ifdef DEBUG_IFSWITCH_SERIAL
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG_IFSWITCH_SERIAL */

    /* If an error occurs, notify the calling function */
    if(serialAccess(IF_ADC_CONVERT_STROBE,
                    NULL,
                    IF_ADC_STROBE_SIZE,
                    IF_ADC_STROBE_SHIFT_SIZE,
                    IF_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status:
       - parallel input */
    /* Setup for 1 second and start the asynchronous timer */
    if(startAsyncTimer(TIMER_IF_ADC_RDY,
                       TIMER_IF_TO_ADC_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG_IFSWITCH_SERIAL
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG_SWITCH_SERIAL */

        /* If an error occurs, notify the calling function */
        if(serialAccess(IF_PARALLEL_READ,
                        &ifRegisters.
                          statusReg.
                           integer,
                        IF_STATUS_REG_SIZE,
                        IF_STATUS_REG_SHIFT_SIZE,
                        IF_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){

            /* Stop the timer */
            if(stopAsyncTimer(TIMER_IF_ADC_RDY)==ERROR){
                return ERROR;
            }

            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_IF_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((ifRegisters.
               statusReg.
                bitField.
                 adcReady==IF_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_IF_SERIAL, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer */
    if(stopAsyncTimer(TIMER_IF_ADC_RDY)==ERROR){
        return ERROR;
    }

    /* ADC read cycle */
    #ifdef DEBUG_IFSWITCH_SERIAL
        printf("         - Reading ADC value\n");
    #endif /* DEBUG_SWITCH_SERIAL */

    /* If error return the state to the calling function */
    if(serialAccess(IF_ADC_DATA_READ,
                    &tempAdcValue,
                    IF_ADC_DATA_SIZE,
                    IF_ADC_DATA_SHIFT_SIZE,
                    IF_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    ifRegisters.
     adcData = (unsigned int)tempAdcValue[0];

    return NO_ERROR;
}


/* Set the IF switch temperature servo enable */
/*! This function controls the enable line for the IF switch temperature servo.

    The function will perform the following operations:
        -# Perform a parallel write of the new FREG
        -# If no error occurs, update FREG and the frontend variable with the
           new state

    \param enable   This is the state to set the IF switch temperature servo to
                        - \ref IF_TEMP_SERVO_ENABLE -> to enable the servo
                        - \ref IF_TEMP_SERVO_DIABLE -> to disable the servo

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setIfTempServoEnable(unsigned char enable){
    /* Store the current value of the temperature servo mode in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with FREG describing a
       different state than the hardware one. */
    int tempFReg=ifRegisters.
                  fReg;

    if (frontend.mode != SIMULATION_MODE) {
        /* Update FREG */
        ifRegisters.
         fReg=(enable==IF_TEMP_SERVO_ENABLE)?SET_IF_TEMP_SERVO_ENABLE(currentIfSwitchModule):
                                             SET_IF_TEMP_SERVO_DISABLE(currentIfSwitchModule);

        /* 2 - Parallel write FREG */
        #ifdef DEBUG_IFSWITCH_SERIAL
            printf("         - Writing FREG\n");
        #endif /* DEBUG_IFSWITCH_SERIAL */

        /* If there is a problem writing FREG, restore FREG and return the ERROR */
        if(serialAccess(IF_PARALLEL_WRITE(IF_FREG),
                        &ifRegisters.
                          fReg,
                        IF_FREG_SIZE,
                        IF_FREG_SHIFT_SIZE,
                        IF_FREG_SHIFT_DIR,
                        SERIAL_WRITE)==ERROR){
            /* Restore FREG to its original saved value */
            ifRegisters.
             fReg = tempFReg;

            return ERROR;
        }
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     ifSwitch.
      ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
               [currentIfChannelSideband[currentIfSwitchModule]].
       ifTempServo.
        enable=(enable==IF_TEMP_SERVO_ENABLE)?IF_TEMP_SERVO_ENABLE:
                                                             IF_TEMP_SERVO_DISABLE;

    return NO_ERROR;
}




/* Get IF channel temperature */
/*! This function gets the temperature of the selected IF switch channel.
    The temperatures are computed in different ways depending on the hardware
    revision level of the IF switch M&C board.

    In order to measure the temperature, it's necessary to acquire two voltages,
    so this function will perform the following operations
        -# Set the desired monitor point by:
            - updating GREG
        -# Execute the core get function common to all the analog monitor
           requests for the IF switch
        -# Scale the raw binary data with the correct unit and store the result
           in a temporary variable
        -# Repeat 1-3 for the following voltage (only for old revision hardware)
        -# Scale the raw data with the correct unit and store the result in the
           \ref frontend variable.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getIfChannelTemp(void){

    /* Variables to store the temporary data */
    float v = 0.0, v1 = 0.0, v2 = 0.0, rTermistor=0.0, temperature=0.0;

    if (frontend.mode != SIMULATION_MODE) {
        /* Clear the IF switch GREG */
        ifRegisters.
         gReg=0x0000;

        /* If the IF switch M&C module is the old hardware revision */
        if(frontend.
            ifSwitch.
             hardwRevision==IF_SWITCH_HRDW_REV0){
            /* 1 - Select the desired monitor point
                   a - update GREG */
            ifRegisters.
             gReg=IF_GREG_SELECT_TEMP_MONITOR_V1(currentIfSwitchModule);

            /* 2 - Call the getIfAnalogMonitor function */
            if(getIfAnalogMonitor()==ERROR){
                return ERROR;
            }

            /* 3 - Scale the first data and store in temporary variable */
            v1=(IF_ADC_TEMP_V_SCALE*ifRegisters.
                                     adcData)/IF_ADC_RANGE;

            /* 4 - Repeat 1-3 for the next voltage */
            /* Select the desired monitor point
                   - update GREG */
            ifRegisters.
             gReg=IF_GREG_SELECT_TEMP_MONITOR_V2(currentIfSwitchModule);

            /* Call the getIfAnalogMonitor function */
            if(getIfAnalogMonitor()==ERROR){
                return ERROR;
            }

            /* Scale the data and store in temporary variable */
            v2=(IF_ADC_TEMP_V_SCALE*ifRegisters.
                                     adcData)/IF_ADC_RANGE;

            /* 5 - Scale the data */
            /* Find the termistor resistance */
            rTermistor = BRIDGE_RESISTOR*(v1+v2-2.0*VREF)/(VREF-v1);

            /* Find the temperature in K */
            temperature = BETA_NORDEN*298.15/(298.15*log(rTermistor/10000.0)+BETA_NORDEN);

        } else { // If it is the new hardware

            /* 1 - Select the desired monitor point
                   a - update GREG */
            ifRegisters.
             gReg=IF_GREG_SELECT_TEMP_MONITOR_V_NEW_HARDW(currentIfSwitchModule);

            /* 2 - Call the getIfAnalogMonitor function */
            if(getIfAnalogMonitor()==ERROR){
                return ERROR;
            }

            /* 3 - Scale the first data and store in temporary variable */
            v=(IF_ADC_TEMP_V_SCALE*ifRegisters.
                                    adcData)/IF_ADC_RANGE;

            /* 4 - Scale the data */
            /* Find the termistor resistance */
            rTermistor = BRIDGE_RESISTOR_NEW_HARDW*(v/VREF_NEW_HARDW);

            /* Find the temperature in K */
            temperature = BETA_NORDEN*298.15/(298.15*log(rTermistor/10000.0)+BETA_NORDEN);
        }

        /* Check if a domain error occurred while evaluating the log. */
        if(errno==EDOM){
            return HARDW_CON_ERR;
        }

        /* If no error in evaluating the temperature, store the data */
        frontend.
         ifSwitch.
          ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
                   [currentIfChannelSideband[currentIfSwitchModule]].
           assemblyTemp=temperature-TEMP_OFFSET;
    } else {
        //SIMULATION_MODE
        frontend.ifSwitch.ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
           [currentIfChannelSideband[currentIfSwitchModule]].assemblyTemp = 23.4;
    }
    return NO_ERROR;
}






/* Set IF Channel attenuation */
/*! This function control the attenuation of the IF switch channels.

    This function will perform the following operations:
        -# perform a parallel write of the new register:
            - BREG for channel 0
            - CREG for channel 1
            - DREG for channel 2
            - EREG for channel 3
        -# if no error occurs, update the register and the frontend variable
           with the new state.

    \return
        - \ref NO_ERROR -> if no erro occurres
        - \ref ERROR    -> if something wrong happened */
int setIfChannelAttenuation(void){

    /* Depending on the selected channel, perform the following operations:
            - Store the current value of the attenuation in a temporary
              variable. We use a temporary variable so that if any error occurs
              during the update of the hardware state, we don't end up with a
              register describing a state different from the hardware one.
            - Update the selected register
            - Parallel write the new register and if everything goes fine
            - update the frontend variable. */
    if (frontend.mode != SIMULATION_MODE) {
        switch(currentIfSwitchModule){
            case IF_CHANNEL0:
                {
                    int tempBReg = ifRegisters.
                                    bReg;

                    /* Update BREG */
                    ifRegisters.
                     bReg = CAN_BYTE;

                    /* 2 - Parallel write BREG */
                    #ifdef DEBUG_IFSWITCH_SERIAL
                        printf("         - Writing BREG\n");
                    #endif /* DEBUG_IFSWITCH_SERIAL */

                    /* If there is a problem writing BREG, restore BREG and return ERROR */
                    if(serialAccess(IF_PARALLEL_WRITE(IF_BREG),
                                    &ifRegisters.
                                      bReg,
                                    IF_BREG_SIZE,
                                    IF_BREG_SHIFT_SIZE,
                                    IF_BREG_SHIFT_DIR,
                                    SERIAL_WRITE)==ERROR){
                        /* Restore BREG to its original save value */
                        ifRegisters.
                         bReg = tempBReg;

                        return ERROR;
                    }
                }
                break;
            case IF_CHANNEL1:
                {
                    int tempCReg = ifRegisters.
                                    cReg;

                    /* Update CREG */
                    ifRegisters.
                     cReg = CAN_BYTE;

                    /* 2 - Parallel write CREG */
                    #ifdef DEBUG_IFSWITCH_SERIAL
                        printf("         - Writing CREG\n");
                    #endif /* DEBUG_IFSWITCH_SERIAL */

                    /* If there is a problem writing CREG, restore CREG and return ERROR */
                    if(serialAccess(IF_PARALLEL_WRITE(IF_CREG),
                                    &ifRegisters.
                                      cReg,
                                    IF_CREG_SIZE,
                                    IF_CREG_SHIFT_SIZE,
                                    IF_CREG_SHIFT_DIR,
                                    SERIAL_WRITE)==ERROR){
                        /* Restore CREG to its original save value */
                        ifRegisters.
                         cReg = tempCReg;

                        return ERROR;
                    }
                }
                break;
            case IF_CHANNEL2:
                {
                    int tempDReg = ifRegisters.
                                    dReg;

                    /* Update DREG */
                    ifRegisters.
                     dReg = CAN_BYTE;

                    /* 2 - Parallel write DREG */
                    #ifdef DEBUG_IFSWITCH_SERIAL
                        printf("         - Writing DREG\n");
                    #endif /* DEBUG_IFSWITCH_SERIAL */

                    /* If there is a problem writing DREG, restore DREG and return ERROR */
                    if(serialAccess(IF_PARALLEL_WRITE(IF_DREG),
                                    &ifRegisters.
                                      dReg,
                                    IF_DREG_SIZE,
                                    IF_DREG_SHIFT_SIZE,
                                    IF_DREG_SHIFT_DIR,
                                    SERIAL_WRITE)==ERROR){
                        /* Restore DREG to its original save value */
                        ifRegisters.
                         dReg = tempDReg;

                        return ERROR;
                    }
                }
                break;
            case IF_CHANNEL3:
                {
                    int tempEReg = ifRegisters.
                                    eReg;

                    /* Update EREG */
                    ifRegisters.
                     eReg = CAN_BYTE;

                    /* 2 - Parallel write EREG */
                    #ifdef DEBUG_SWITCH_SERIAL
                        printf("         - Writing EREG\n");
                    #endif /* DEBUG_FI_SWITCH_SERIAL */

                    /* If there is a problem writing EREG, restore EREG and return ERROR */
                    if(serialAccess(IF_PARALLEL_WRITE(IF_EREG),
                                    &ifRegisters.
                                      eReg,
                                    IF_EREG_SIZE,
                                    IF_EREG_SHIFT_SIZE,
                                    IF_EREG_SHIFT_DIR,
                                    SERIAL_WRITE)==ERROR){
                        /* Restore EREG to its original save value */
                        ifRegisters.
                         eReg = tempEReg;

                        return ERROR;
                    }
                }
                break;
            default:
                break;
        }
    }

    /* Since there is no real hardware read back, if no error
       occurred the current state is updated to reflect the issued
       command. */
    frontend.
     ifSwitch.
      ifChannel[currentIfChannelPolarization[currentIfSwitchModule]]
               [currentIfChannelSideband[currentIfSwitchModule]].
       attenuation=CAN_BYTE;

    return NO_ERROR;
}

/* Set IF switch band select */
/*! This function controls the selection of the way within the IF switch module.
    A "way" is defined as one of the 10:1 channels.

    This function will perform the following operations:
        -# Perform a parallel write of the new AREG
        -# If no error occurs, update AREG and the frontend variable with the
           new state.

    \return
        - \ref NO_ERROR -> if no error occurres
        - \ref ERROR    -> if something wrong happened */
int setIfSwitchBandSelect(void){

    /* Store the current value of the temperature servo mode in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with AREG describing a
       different state than the hardware one. */
    int tempAReg=ifRegisters.
                  aReg;

    if (frontend.mode != SIMULATION_MODE) {
        /* Update AREG */
        ifRegisters.
         aReg = IF_AREG_SELECT_WAY(CAN_BYTE);

        /* 2 - Parallel write AREG */
        #ifdef DEBUG_IFSWITCH_SERIAL
            printf("         - Writing AREG\n");
        #endif /* DEBUG_IFSWITCH_SERIAL */

        /* If there is a problem writing AREG, restore AREG and return the ERROR */
        if(serialAccess(IF_PARALLEL_WRITE(IF_AREG),
                        &ifRegisters.
                          aReg,
                        IF_AREG_SIZE,
                        IF_AREG_SHIFT_SIZE,
                        IF_AREG_SHIFT_DIR,
                        SERIAL_WRITE)==ERROR){
            /* Restore AREG to its original saved value */
            ifRegisters.
             aReg = tempAReg;

            return ERROR;
        }
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     ifSwitch.
      bandSelect=CAN_BYTE;

    return NO_ERROR;
}


/* Get IF switch M&C board hardware revision level */
/*! This function reads the hardware revision level of the IF switch M&C board.
    Different revision require different handling of certain data.

    This function will perform the following operations:
        -# Perform a parallel read (the hardware revision data is included in
           the status register
        -# If no error occurs, update the frontend variable with the revision
           level

    \return
        - \ref NO_ERROR -> if no error occurres
        - \ref ERROR    -> if something wrong happened */
int getIfSwitchHardwRevision(void){

    #ifdef DEBUG_IFSWITCH_SERIAL
        printf("         - Reading hardware revision level (parallel read)\n");
    #endif /* DEBUG_SWITCH_SERIAL */

    if (frontend.mode != SIMULATION_MODE) {
        /* If an error occurs, notify the calling function */
        if(serialAccess(IF_PARALLEL_READ,
                        &ifRegisters.
                          statusReg.
                           integer,
                        IF_STATUS_REG_SIZE,
                        IF_STATUS_REG_SHIFT_SIZE,
                        IF_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){
            return ERROR;
        }

        /* If no error update frontend variable. */
        frontend.
         ifSwitch.
          hardwRevision=ifRegisters.
                         statusReg.
                          bitField.
                           hardwRev;
    } else {
        //SIMULATION_MODE
        frontend.ifSwitch.hardwRevision = 1;
    }

    return NO_ERROR;
}
