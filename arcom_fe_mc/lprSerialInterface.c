/*! \file   lprSerialInterface.c
    \brief  LPR serial interface functions

    <b> File informations: </b><br>
    Created: 2007/06/05 14:59:17 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lprSerialInterface.c,v 1.10 2009/10/22 14:52:09 avaccari Exp $

    This files contains all the functions necessary to control and operate the
    LPR serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote LPR monitor and control interface.
    The functions in this module will perform a series of register monipulations
    and remote access operations to achieve the desired result.
    Every function in this module will assume that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

/* Includes */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */
#include <math.h>       /* pow */
#include <errno.h>      /* errno */

#include "lprSerialInterface.h"
#include "error.h"
#include "serialInterface.h"
#include "timer.h"
#include "frontend.h"
#include "debug.h"
#include "laser.h"

/* Globals */
/* Externs */
/* Statics */
LPR_REGISTERS lprRegisters;


/* LPR analog monitor request core.
   This function performs the core operations that are common to all the analog
   monitor request for the LPR module:
       - Write the LPR BREG with a parallel output write cycle
       - Initiate an ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle that get the raw data

   If an error happens during the process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getLprAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of which the first two are
       to be ignored. This variable allowes manipulation of data so that the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];


    /* Parallel write BREG */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(LPR_PARALLEL_WRITE(LPR_BREG),
                    &lprRegisters.
                      bReg.
                       integer,
                    LPR_BREG_SIZE,
                    LPR_BREG_SHIFT_SIZE,
                    LPR_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Initiate ADC conversion
        - send ADC convert strobe command */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG */
    /* If an error occurs, notify the calling function */
    if(serialAccess(LPR_ADC_CONVERT_STROBE,
                    NULL,
                    LPR_ADC_STROBE_SIZE,
                    LPR_ADC_STROBE_SHIFT_SIZE,
                    LPR_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status
        - parallel input */
    /* Setup for 1 seconds and start the asynchronous timer */
    if(startAsyncTimer(TIMER_LPR_ADC_RDY,
                       TIMER_LPR_TO_ADC_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG_LPR_SERIAL
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG */

        /* If an error occurs, notify the calling funtion */
        if(serialAccess(LPR_PARALLEL_READ,
                        &lprRegisters.
                          statusReg.
                           integer,
                        LPR_STATUS_REG_SIZE,
                        LPR_STATUS_REG_SHIFT_SIZE,
                        LPR_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){
            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_LPR_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((lprRegisters.
               statusReg.
                bitField.
                 adcReady==LPR_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If the timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_LPR_SERIAL,
                   0x01); // Error 0x01 -> Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer. */
    if(stopAsyncTimer(TIMER_LPR_ADC_RDY)==ERROR){
        return ERROR;
    }


    /* ADC read cycle */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Reading ADC value\n");
    #endif /* DEBUG */

    /* If error return the state to the calling function */
    if(serialAccess(LPR_ADC_DATA_READ,
                    &tempAdcValue,
                    LPR_ADC_DATA_SIZE,
                    LPR_ADC_DATA_SHIFT_SIZE,
                    LPR_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    lprRegisters.
     adcData = tempAdcValue[0];

    return NO_ERROR;
}


/* Get LPR temperatures */
/*! This function return the operating informations about the addressed
    temperature sensor. The resulting scaled value is stored in the
    \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */
int getLprTemp(void){

    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_TEMPERATURE(currentLprModule);

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The temperature is given by: 5.0e+6*(adcData/65536)/5110 */
    frontend.
     lpr.
      lprTemp[currentLprModule].
       temp[CURRENT_VALUE]=(LPR_ADC_TEMP_SCALE*lprRegisters.
                                                adcData)/LPR_ADC_RANGE;

    return NO_ERROR;
}



/* Set optical switch port */
/*! This function controls the optical switch port for the LPR.

    The function will perform the following operations:
        -# Perform a parallel write of the new AREG
        -# If no error occurs, update AREG and the frontend variable with the
           new state

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setOpticalSwitchPort(void){
    /* Store the current value of the optical switch port in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with an AREG describing
       a different state than the hardware one. */
    int tempAReg=lprRegisters.
                  aReg.
                   integer;

    /* Update AREG */
    lprRegisters.
     aReg.
      bitField.
       port=LPR_AREG_SWITCH_PORT(CAN_BYTE);

    /* Get the LPR states. */
    if(getLprStates()==ERROR){
        return ERROR;
    }

    /* If no error check the idle state. If busy, return error. */
    if(frontend.
        lpr.
         opticalSwitch.
          state[CURRENT_VALUE]==OPTICAL_SWITCH_BUSY){
            storeError(ERR_LPR_SERIAL,
                       0x02); // Error 0x02 -> Optical switch busy
            return ERROR;
    }

    /* 1 - Parallel write AREG */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Writing AREG\n");
    #endif /* DEBUG */

    if(serialAccess(LPR_PARALLEL_WRITE(LPR_AREG),
                    &lprRegisters.
                      aReg.
                       integer,
                    LPR_AREG_SIZE,
                    LPR_AREG_SHIFT_SIZE,
                    LPR_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore AREG to its original saved value */
        lprRegisters.
         aReg.
          integer = tempAReg;

        return ERROR;
    }

    /* Send optical switch strobe */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Sending optical switch strobe\n");
    #endif /* DEBUG_LPR_SERIAL */
    /* If an error occurs, notify the calling function */
    if(serialAccess(LPR_OPTICAL_SWITCH_STROBE,
                    NULL,
                    LPR_OPTICAL_SWITCH_STROBE_SIZE,
                    LPR_OPTICAL_SWITCH_STROBE_SHIFT_SIZE,
                    LPR_OPTICAL_SWITCH_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     lpr.
      opticalSwitch.
       port[CURRENT_VALUE]=CAN_BYTE;

    return NO_ERROR;
}



/* Set optical switch shutter */
/*! This function controls the optical switch shutter for the LPR.

    The function will perform the following operations:
        -# Perform a parallel write of the new AREG
        -# If no error occurs, update AREG and the frontend variable with the
           new state.

    \param mode     This defines the kind of shutter mode. If forced, no check
                    is performed on the optical switch busy state before
                    switching to shutter mode. This is used when a reset for
                    the switch is necessary. It can take the following values:
                        - \ref FORCED   -> use forced mode
                        - \ref STANDARD -> check for busy state

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setOpticalSwitchShutter(unsigned char mode){
    /* Store the current value of the optical switch port in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with an AREG describing
       a different state than the hardware one. */
    int tempAReg=lprRegisters.
                  aReg.
                   integer;

    /* Update AREG */
    lprRegisters.
     aReg.
      bitField.
       port=LPR_AREG_SWITCH_SHUTTER;

    /* If the shutter mode is not forced then check for the busy state */
    if(mode==STANDARD){
        /* Get the LPR states. */
        if(getLprStates()==ERROR){
            return ERROR;
        }

        /* If no error check the idle state. If busy, return error. */
        if(frontend.
            lpr.
             opticalSwitch.
              state[CURRENT_VALUE]==OPTICAL_SWITCH_BUSY){
                storeError(ERR_LPR_SERIAL,
                           0x02); // Error 0x02 -> Optical switch busy
                return ERROR;
        }
    }

    /* 1 - Parallel write AREG */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Writing AREG\n");
    #endif /* DEBUG */

    if(serialAccess(LPR_PARALLEL_WRITE(LPR_AREG),
                    &lprRegisters.
                      aReg.
                       integer,
                    LPR_AREG_SIZE,
                    LPR_AREG_SHIFT_SIZE,
                    LPR_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore AREG to its original saved value */
        lprRegisters.
         aReg.
          integer = tempAReg;

        return ERROR;
    }

    /* Send optical switch strobe */
    #ifdef DEBUG_LPR_SERIAL
        printf("         - Sending optical switch strobe\n");
    #endif /* DEBUG_LPR_SERIAL */
    /* If an error occurs, notify the calling function */
    if(serialAccess(LPR_OPTICAL_SWITCH_STROBE,
                    NULL,
                    LPR_OPTICAL_SWITCH_STROBE_SIZE,
                    LPR_OPTICAL_SWITCH_STROBE_SHIFT_SIZE,
                    LPR_OPTICAL_SWITCH_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     lpr.
      opticalSwitch.
       shutter[CURRENT_VALUE]=SHUTTER_ENABLE;

    return NO_ERROR;
}





/* Get laser drive current */
/*! This function returns the current of the laser drive. The resulting scaled
    value is stored in the \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */
int getLaserDriveCurrent(void){
    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_LASER_DRIVE_CURRENT;

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The laser drive current is given by: 800*(adcData/65536) */
    frontend.
     lpr.
      edfa.
       laser.
        driveCurrent[CURRENT_VALUE]=(LPR_ADC_DRIVE_CURRENT_SCALE*lprRegisters.
                                                                  adcData/LPR_ADC_RANGE);

    return NO_ERROR;
}




/* Get laser photo detector current */
/*! This function returns the current of the laser photo detector. The resulting
    scaled value is stored in the \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */
int getLaserPhotoDetectCurrent(void){
    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_LASER_PD_CURRENT;

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The laser drive current is given by: 2.5-[5000.0*(adcData/65536)] */
    frontend.
     lpr.
      edfa.
       laser.
        photoDetectCurrent[CURRENT_VALUE]=LPR_ADC_LASER_PD_CURRRENT_OFFSET-(LPR_ADC_LASER_PD_CURRENT_SCALE*lprRegisters.
                                                                                                            adcData)/LPR_ADC_RANGE;

    return NO_ERROR;
}





/* Get EDFA photo detector current */
/*! This function returns the current of the EDFA photo detector. The resulting
    scaled value is stored in the \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */
int getPhotoDetectorCurrent(void){
    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_EDFA_PD_CURRENT;

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The laser drive current is given by: 500.0*(adcData/65536) */
    frontend.
     lpr.
      edfa.
       photoDetector.
        current[CURRENT_VALUE]=(LPR_ADC_EDFA_PD_CURRENT_SCALE*lprRegisters.
                                                               adcData)/LPR_ADC_RANGE;

    return NO_ERROR;
}




/* Send DAC strobes */
/*! This function will send the required strobe to the currently addressed DAC.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setLprDacStrobe(void){
    /* Send the reset strobe */
    if(serialAccess(LPR_DAC_RESET_STROBE,
                    NULL,
                    LPR_DAC_STROBE_SIZE,
                    LPR_DAC_STROBE_SHIFT_SIZE,
                    LPR_DAC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}




/* Set modulation input value */
/*! This function allow the user to set different values for the modulation
    input.

    This function performs the following operations:
        -# Scale the analog control paramenter from float to raw 16-bit data
        -# execute the DAC write cycle

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setModulationInputValue(void){

    /* Setup the DAC2 message */
    /* Select the register to address */
    lprRegisters.
     dacReg.
      bitField.
       dacSelect=LPR_DAC_REGISTER(LPR_DAC_A_REG);
    /* Setup quick load */
    lprRegisters.
     dacReg.
      bitField.
       quickLoad=NO;
    /* 1 - Format the data according to the dac specifications. */
    lprRegisters.
     dacReg.
      bitField.
       data=LPR_DAC_MOD_INPUT_SCALE(CONV_FLOAT);

    /* 2 - Write the data to the serial access function */
    #ifdef DEBUG
        printf("         - Writing DAC\n");
    #endif /* DEBUG */
    if(serialAccess(LPR_DAC_DATA_WRITE,
                    lprRegisters.
                     dacReg.
                      integer,
                    LPR_DAC_DATA_SIZE,
                    LPR_DAC_DATA_SHIFT_SIZE,
                    LPR_DAC_DATA_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}






/* Get EDFA photo detector power */
/*! This function returns the power of the EDFA photo detector. The resulting
    scaled value is stored in the \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */

int getPhotoDetectorPower(void){
    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_EDFA_PD_POWER;

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The laser drive current is given by: coeff*(adcData/65536)
       The coefficient is stored in the configuration database and loaded at
       initialization time. */
    frontend.
     lpr.
      edfa.
       photoDetector.
        power[CURRENT_VALUE]=(frontend.
                               lpr.
                                edfa.
                                 photoDetector.
                                  coeff*lprRegisters.
                                         adcData/LPR_ADC_RANGE);

    return NO_ERROR;
}






/* Get laser pump temperature */
/*! This function returns the pump temperature of the laser. The resulting
    scaled value is stored in the \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the LPR module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened */
int getLaserPumpTemperature(void){

    /* Float to help perform the temperature evaluation */
    float vin=0.0, temperature=0.0;

    /* Clear the LPR BREG */
    lprRegisters.
     bReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update BREG */
    lprRegisters.
     bReg.
      bitField.
       monitorPoint=LPR_BREG_PUMP_TEMP;

    /* 2->5 Call the getLprAnalogMonitor funtion */
    if(getLprAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* Scale the input voltage to the right value: vin=5*(adcData/65536) */
    vin=(LPR_ADC_VOLTAGE_IN_SCALE*lprRegisters.
                                   adcData)/LPR_ADC_RANGE;
    /* Apply the interpolation */
    temperature=PTMP_0+
                PTMP_1*vin+
                PTMP_2*pow(vin,
                           2.0)+
                PTMP_3*pow(vin,
                           3.0)+
                PTMP_4*pow(vin,
                           4.0)+
                PTMP_5*pow(vin,
                           5.0)+
                PTMP_6*pow(vin,
                           6.0);

    /* Check if a domain error occurred while evaluating the power. */
    if(errno==EDOM){
        return HARDW_CON_ERR;
    }

    /* Store the data */
    frontend.
     lpr.
      edfa.
       laser.
        pumpTemp[CURRENT_VALUE]=temperature;

    return NO_ERROR;
}






/* Get LPR states */
/*! This function monitors the states of several hardware in the LPR. This is a
    read-back of real hardware status bits.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getLprStates(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(LPR_PARALLEL_READ,
                    &lprRegisters.
                      statusReg.
                       integer,
                    LPR_STATUS_REG_SIZE,
                    LPR_STATUS_REG_SHIFT_SIZE,
                    LPR_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Store the optical switch error */
    frontend.
     lpr.
      opticalSwitch.
       state[CURRENT_VALUE]=lprRegisters.
                             statusReg.
                              bitField.
                               opticalSwitchError;

    /* Store the optical switch busy state */
    frontend.
     lpr.
      opticalSwitch.
       busy[CURRENT_VALUE]=lprRegisters.
                            statusReg.
                             bitField.
                              opticalSwitchState;

    /* Store the EDFA driver state */
    frontend.
     lpr.
      edfa.
       driverTempAlarm[CURRENT_VALUE]=lprRegisters.
                                       statusReg.
                                        bitField.
                                         edfaDriverState;

    return NO_ERROR;
}

