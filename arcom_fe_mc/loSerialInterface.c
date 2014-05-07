/*! \file   loSerialInterface.c
    \brief  LO serial interface functions

    <b> File Informations: </b><br>
    Created: 2006/09/06 11:52:38 by avaccari

    <b> CVS Informations: </b><br>
    \$Id: loSerialInterface.c,v 1.9 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the functions necessary to control and operate the
    LO serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote LO monitor and control interface.
    The fucntions in this module will perform a series of register manipulations
    and remote access operation to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

/* Includes */
#include <stdio.h>      /* printf */
#include <math.h>       /* sqrt */

#include "debug.h"
#include "error.h"
#include "loSerialInterface.h"
#include "can.h"
#include "serialInterface.h"
#include "frontend.h"
#include "timer.h"

/* Globals */
/* Externs */
/* Statics */
LO_REGISTERS loRegisters[CARTRIDGES_NUMBER];

/* LO analog monitor request core.
   This function performs the core operations that are common to all the analog
   monitor requests for the LO module:
       - Write the LO BREG with a parallel output write cycle
       - Initiate and ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle to get the raw data

   If an error happens during the process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getLoAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of with the first two are
       to be ignored. This variable allowes manipulation of data so thata the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];

    /* Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Initiate ADC conversion:
       - send ADC convert strobe command */
    #ifdef DEBUG
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG */

    /* If an error occurs, notify the calling function */
    if(serialAccess(LO_ADC_CONVERT_STROBE,
                    NULL,
                    LO_ADC_STROBE_SIZE,
                    LO_ADC_STROBE_SHIFT_SIZE,
                    LO_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status:
       - parallel input */
    /* Setup for 1 second and start the asynchronous timer */
    if(startAsyncTimer(TIMER_LO_ADC_RDY,
                       TIMER_LO_TO_ADC_RDY)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG */

        /* If an error occurs, notify the calling function */
        if(serialAccess(LO_PARALLEL_READ,
                        &loRegisters[currentModule].
                          statusReg.
                           integer,
                        LO_STATUS_REG_SIZE,
                        LO_STATUS_REG_SHIFT_SIZE,
                        LO_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){
            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_LO_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((loRegisters[currentModule].
               statusReg.
                bitField.
                 adcReady==LO_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_LO_SERIAL,
                   0x01); // Error 0x01 -> Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer */
    if(stopAsyncTimer(TIMER_LO_ADC_RDY)==ERROR){
        return ERROR;
    }

    /* ADC read cycle */
    #ifdef DEBUG
        printf("         - Reading ADC value\n");
    #endif /* DEBUG */

    /* If error return the state to the calling function */
    if(serialAccess(LO_ADC_DATA_READ,
                    &tempAdcValue,
                    LO_ADC_DATA_SIZE,
                    LO_ADC_DATA_SHIFT_SIZE,
                    LO_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    loRegisters[currentModule].
     adcData = tempAdcValue[0];

   return NO_ERROR;
}






/* Set YTO coarse tune */
/*! This function controls the YTO coarse tune for the currently addressed LO.

    The function will perform the following operations:
        -# Perform a parallel write of the new AREG
        -# If no error occurs, update AREG and the frontend variable with the
           new state

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */

int setYtoCoarseTune(void){
    /* Store the current value of the YTO coarse tune in a temporary variable.
       We use a temporary variable so that if any error occurs during the update
       of the hardware state, we don't end up with an AREG describing a
       different state than the hardware one. */
    int tempAReg=loRegisters[currentModule].
                  aReg.
                   integer;

    /* Update AREG */
    loRegisters[currentModule].
     aReg.
      bitField.
       ytoCoarseTune=CAN_UINT;

    /* 1 - Parallel write AREG */
    #ifdef DEBUG
        printf("         - Writing AREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_AREG),
                    &loRegisters[currentModule].
                      aReg.
                       integer,
                    LO_AREG_SIZE,
                    LO_AREG_SHIFT_SIZE,
                    LO_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore AREG to its original saved value */
        loRegisters[currentModule].
         aReg.
          integer = tempAReg;

        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cartridge[currentModule].
      lo.
       yto.
        ytoCoarseTune[CURRENT_VALUE]=CAN_UINT;

    return NO_ERROR;
}




/* Set Photomixer Enable */
/*! This function control the enable state of the photomixer for the currently
    addressed LO.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable    This is the state to set the photomixer to:
                        - \ref PHOTOMIXER_ENABLE    -> Enable photomixer bias
                        - \ref PHOTOMIXER_DISABLE   -> Disable photomixer bias

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setPhotomixerEnable(unsigned char enable){
    /* Store the current value of the photomixer enable state in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a BREG describing
       a different state than the hardware one. */
    int tempBReg = loRegisters[currentModule].
                    bReg.
                     integer;

    /* Update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       photomixerBiasEnable=(enable==PHOTOMIXER_ENABLE)?PHOTOMIXER_ENABLE:
                                                        PHOTOMIXER_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved valur */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cartridge[currentModule].
      lo.
       photomixer.
        enable[CURRENT_VALUE]=(enable==PHOTOMIXER_ENABLE)?PHOTOMIXER_ENABLE:
                                                          PHOTOMIXER_DISABLE;

    return NO_ERROR;
}








/* Get photomixer */
/*! This function return the operating values of current and voltage for the
    LO photomixer. The resulting scaled values are stored in the \ref frontend
    status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the LO module.
        -# Scale the raw binary data with the correct unit and store the results
           in the \ref frontend variable.

    \param port     This is the channel to be monitor. It can get the following
                    values:
                        - \ref PHOTOMIXER_BIAS_V -> Photomixer bias voltage
                        - \ref PHOTOMIXER_BIAS_C -> Photomixer bias current

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPhotomixer(unsigned char port){

    /* Clear the LO BREG monitor selection bitfield */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint = 0x00;

    /* 1 - Select the desired monitor point
           a - update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint=LO_BREG_PHOTOMIXER(port);

    /* 2->5 - Call the getLoAnalogMonitor function */
    if(getLoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(port){
        /* The photomixer bias voltage is given by: -10*(adcData/65536) */
        case PHOTOMIXER_BIAS_V:
            frontend.
             cartridge[currentModule].
              lo.
               photomixer.
                voltage[CURRENT_VALUE]=(LO_ADC_PHOTOMIXER_BIAS_V_SCALE*loRegisters[currentModule].
                                                                        adcData)/LO_ADC_RANGE;
            break;
        /* The photomixer bias current is given by: -10*(adcData/65536) */
        case PHOTOMIXER_BIAS_C:
            frontend.
             cartridge[currentModule].
              lo.
               photomixer.
                current[CURRENT_VALUE]=(LO_ADC_PHOTOMIXER_BIAS_C_SCALE*loRegisters[currentModule].
                                                                        adcData)/LO_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}


/* Get PLL */
/*! This function returns the operating information about the addressed PLL.
    The resulting scaled value is stored in the \ref frontend status structure.

    The function performs the following operation:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the LO module.
        -# Scale the raw binary data with the correct unit and store the results
           in the \ref frontend variable.

    \param port     This is the selected channel to be monitored. It can get the
                    following values:
                        - \ref PLL_LOCK_DETECT_VOLTAGE  -> Lock detect voltage
                                                           of the PLL
                        - \ref PLL_CORRECTION_VOLTAGE   -> Correction voltage of
                                                           the PLL
                        - \ref PLL_ASSEMBLY_TEMP        -> The temperature of
                                                           the PLL assembly
                        - \ref PLL_YIG_HEATER_CURRENT   -> Current state of the
                                                           PLL YIG heater
                                                           current
                        - \ref PLL_REF_TOTAL_POWER      -> A measure of the
                                                           reference total power
                        - \ref PLL_IF_TOTAL_POWER       -> A measure of the IF
                                                           total power

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPll(unsigned char port){

    /* Clear the LO BREG monitor selection bitfield */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint = 0x00;

    /* 1 - Select the desired monitor point
           a - update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint=LO_BREG_PLL(port);

    /* 2->5 - Call the getLoAnalogMonitor function */
    if(getLoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(port){
        /* The lock detect voltage is given by 210/11*(adcData/65536) */
        case PLL_LOCK_DETECT_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                lockDetectVoltage[CURRENT_VALUE]=(frontend.
                                                   cartridge[currentModule].
                                                    lo.
                                                     pll.
                                                      lockDetectVoltageScale*loRegisters[currentModule].
                                                                              adcData)/LO_ADC_RANGE;
            break;
        /* The correction voltage is given by 210/11*(adcData/65536) */
        case PLL_CORRECTION_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                correctionVoltage[CURRENT_VALUE]=(frontend.
                                                   cartridge[currentModule].
                                                    lo.
                                                     pll.
                                                      correctionVoltageScale*loRegisters[currentModule].
                                                                              adcData)/LO_ADC_RANGE;
            break;
        /* The assembly temperature is given by 1000*(adcData/65536) */
        case PLL_ASSEMBLY_TEMP:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                assemblyTemp[CURRENT_VALUE]=(LO_PLL_ASSEMBLY_TEMP_SCALE*loRegisters[currentModule].
                                                                         adcData)/LO_ADC_RANGE;
            break;
        /* The YIG heater current is given by 10*[(40*adcData/65536)+15] */
        case PLL_YIG_HEATER_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                YIGHeaterCurrent[CURRENT_VALUE]=frontend.
                                                   cartridge[currentModule].
                                                    lo.
                                                     pll.
                                                      YIGHeaterCurrentOffset+(frontend.
                                                                               cartridge[currentModule].
                                                                                lo.
                                                                                 pll.
                                                                                  YIGHeaterCurrentScale*loRegisters[currentModule].
                                                                                                         adcData)/LO_ADC_RANGE;
            break;
        /* The REF total power is given by 10*(adcData/65536) */
        case PLL_REF_TOTAL_POWER:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                refTotalPower[CURRENT_VALUE]=(LO_PLL_TTL_PWR_SCALE*loRegisters[currentModule].
                                                                    adcData)/LO_ADC_RANGE;
            break;
        /* The IF total power is given by 10*(adcData/65536) */
        case PLL_IF_TOTAL_POWER:
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                ifTotalPower[CURRENT_VALUE]=(LO_PLL_TTL_PWR_SCALE*loRegisters[currentModule].
                                                                   adcData)/LO_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}



/* Get PLL states */
/*! This function reads and stores all the PLL status in the frontend status
    array.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPllStates(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(LO_PARALLEL_READ,
                    &loRegisters[currentModule].
                      statusReg.
                       integer,
                    LO_STATUS_REG_SIZE,
                    LO_STATUS_REG_SHIFT_SIZE,
                    LO_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Store the PLL unlock detect latch */
    frontend.
     cartridge[currentModule].
      lo.
       pll.
        unlockDetectLatch[CURRENT_VALUE]=loRegisters[currentModule].
                                          statusReg.
                                           bitField.
                                            pllLockDetect;


    return NO_ERROR;
}






/* Set Clear Unlock Detect Latch */
/*! This function controls the state of the clear unlock detect latch bit for
    the currently addressed LO. This is a toggling function so the write will
    be repeated twice to perform the toggling.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setClearUnlockDetectLatch(void){
    /* Store the current valule of the unlock detect latch bit in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a BREG describing
       a different state than the hardware one. */
    int tempBReg = loRegisters[currentModule].
                    bReg.
                     integer;


    /* Update BREG (toggle to 0) */
    loRegisters[currentModule].
     bReg.
      bitField.
       clearUnlockDetectLatch=PLL_UNLOCK_DETECT_LATCH_CLEAR;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;

        return ERROR;
    }

    /* Update BREG (toggle back to 1) */
    loRegisters[currentModule].
     bReg.
      bitField.
       clearUnlockDetectLatch=PLL_UNLOCK_DETECT_LATCH_OPERATE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;

        return ERROR;
    }

    /* No monitor point available for this state so we don't have to update the
       frontend variable. */

    return NO_ERROR;
}



/* Set Loop Bandwidth Select */
/*! This function control the loop bandwidth pf the PLL for the currently
    addressed LO.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable    This is the state to set the photomixer to:
                        - \ref PLL_LOOP_BANDWIDTH_ALTERATE  -> Use alternate
                          loop bandwidth
                        - \ref PLL_LOOP_BANDWIDTH_DEFAULT   -> Use default loop
                          bandwidth

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setLoopBandwidthSelect(unsigned char bandwidth){
    /* Store the current value of the loop bandwidth state in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a BREG describing
       a different state than the hardware one. */

    int tempBReg = loRegisters[currentModule].
                    bReg.
                     integer;

    /* Update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       loopBandwidthSelect=(bandwidth==PLL_LOOP_BANDWIDTH_ALTERNATE)?PLL_LOOP_BANDWIDTH_ALTERNATE:
                                                                     PLL_LOOP_BANDWIDTH_DEFAULT;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cartridge[currentModule].
      lo.
       pll.
        loopBandwidthSelect[CURRENT_VALUE]=(bandwidth==PLL_LOOP_BANDWIDTH_ALTERNATE)?PLL_LOOP_BANDWIDTH_ALTERNATE:
                                                                                     PLL_LOOP_BANDWIDTH_DEFAULT;

    return NO_ERROR;
}



/* Set Sideband Lock Polarity Select */
/*! This function control the polarity of the lock sideband for the PLL in the
    currently addressed LO.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable    This is the state to set the photomixer to:
                        - \ref PLL_SIDEBAND_LOCK_POLARITY_USB  -> Upper Sideband
                        - \ref PLL_SIDEBAND_LOCK_POLARITY_LSB  -> Lower Sideband

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setSidebandLockPolaritySelect(unsigned char sideband){
    /* Store the current value of the sideband lock polarity in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a BREG describing
       a different state than the hardware one. */
    int tempBReg = loRegisters[currentModule].
                    bReg.
                     integer;

    /* Update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       sidebandLockPolaritySelect=(sideband==PLL_SIDEBAND_LOCK_POLARITY_USB)?PLL_SIDEBAND_LOCK_POLARITY_USB:
                                                                             PLL_SIDEBAND_LOCK_POLARITY_LSB;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved valur */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cartridge[currentModule].
      lo.
       pll.
        sidebandLockPolaritySelect[CURRENT_VALUE]=(sideband==PLL_SIDEBAND_LOCK_POLARITY_USB)?PLL_SIDEBAND_LOCK_POLARITY_USB:
                                                                                             PLL_SIDEBAND_LOCK_POLARITY_LSB;

    return NO_ERROR;
}




/* Set Null Loop Integrator */
/*! This function control the state of the PLL null loop integrator status bit
    for the PLL in the currently addressed LO.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable    This is the state to set the photomixer to:
                        - \ref PLL_NULL_LOOP_INTEGRATOR_NULL    -> Null/Zero
                        - \ref PLL_NULL_LOOP_INTEGRATOR_OPERATE -> Operate

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setNullLoopIntegrator(unsigned char state){
    /* Store the current value of the null loop integrator bit in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a BREG describing
       a different state than the hardware one. */
    int tempBReg = loRegisters[currentModule].
                    bReg.
                     integer;

    /* Update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       nullLoopIntegrator=(state==PLL_NULL_LOOP_INTEGRATOR_NULL)?PLL_NULL_LOOP_INTEGRATOR_NULL:
                                                                 PLL_NULL_LOOP_INTEGRATOR_OPERATE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG
        printf("         - Writing BREG\n");
    #endif /* DEBUG */

    if(serialAccess(LO_PARALLEL_WRITE(LO_BREG),
                    &loRegisters[currentModule].
                      bReg.
                       integer,
                    LO_BREG_SIZE,
                    LO_BREG_SHIFT_SIZE,
                    LO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved valur */
        loRegisters[currentModule].
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cartridge[currentModule].
      lo.
       pll.
        nullLoopIntegrator[CURRENT_VALUE]=(state==PLL_NULL_LOOP_INTEGRATOR_NULL)?PLL_NULL_LOOP_INTEGRATOR_NULL:
                                                                                 PLL_NULL_LOOP_INTEGRATOR_OPERATE;

    return NO_ERROR;
}




/* Get AMC */
/*! This function returns the operating information about the addressed AMC.
    The resulting scaled value is stored in the \ref frontend status structure.

    The function performs the following operation:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the LO module.
        -# Scale the raw binary data with the correct unit and store the results
           in the \ref frontend variable.

    \param port     This is the selected channel to be monitored. It can get the
                    following values:
                        - \ref AMC_GATE_A_VOLTAGE       -> Gate voltage for
                                                           stage A of the AMC
                        - \ref AMC_DRAIN_A_VOLTAGE      -> Drain voltage for
                                                           stage A of the AMC
                        - \ref AMC_DRAIN_A_CURRENT      -> Drain current for
                                                           stage A of the AMC
                        - \ref AMC_GATE_B_VOLTAGE       -> Gate voltage for
                                                           stage B of the AMC
                        - \ref AMC_DRAIN_B_VOLTAGE      -> Drain voltage for
                                                           stage B of the AMC
                        - \ref AMC_DRAIN_B_CURRENT      -> Drain current for
                                                           stage B of the AMC
                        - \ref AMC_MULTIPLIER_D_CURRENT -> Multiplier current
                                                           for stage D of the
                                                           AMC
                        - \ref AMC_GATE_E_VOLTAGE       -> Gate voltage for
                                                           stage E of the AMC
                        - \ref AMC_DRAIN_E_VOLTAGE      -> Drain voltage for
                                                           stage E of the AMC
                        - \ref AMC_DRAIN_E_CURRENT      -> Drain current for
                                                           stage E of the AMC
                        - \ref AMC_5V_SUPPLY_VOLTAGE    -> 5V supply for the AMC

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getAmc(unsigned char port){

    /* Clear the LO BREG monitor selection bitfield */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint = 0x00;

    /* 1 - Select the desired monitor point
           a - update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint=LO_BREG_AMC(port);

    /* 2->5 - Call the getLoAnalogMonitor function */
    if(getLoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(port){
        /* The A gate voltage is given by  10*(adcData/65536) */
        case AMC_GATE_A_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                gateAVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                    adcData)/LO_ADC_RANGE;
            break;
        /* The A drain voltage is given by  10*(adcData/65536) */
        case AMC_DRAIN_A_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainAVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                     adcData)/LO_ADC_RANGE;
            break;
        /* The A drain current is given by  1000*(adcData/65536) */
        case AMC_DRAIN_A_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainACurrent[CURRENT_VALUE]=(LO_DRAIN_C_SCALE*loRegisters[currentModule].
                                                                adcData)/LO_ADC_RANGE;
            break;
        /* The B gate voltage is given by  10*(adcData/65536) */
        case AMC_GATE_B_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                gateBVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                    adcData)/LO_ADC_RANGE;
            break;
        /* The B drain voltage is given by  10*(adcData/65536) */
        case AMC_DRAIN_B_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainBVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                     adcData)/LO_ADC_RANGE;
            break;
        /* The B drain current is given by  1000*(adcData/65536) */
        case AMC_DRAIN_B_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainBCurrent[CURRENT_VALUE]=(LO_DRAIN_C_SCALE*loRegisters[currentModule].
                                                                adcData)/LO_ADC_RANGE;
            break;
        /* The 5V supply voltage is given by 10*(adcData/65536) */
        case AMC_5V_SUPPLY_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                supplyVoltage5V[CURRENT_VALUE]=(frontend.
                                                 cartridge[currentModule].
                                                  lo.
                                                   supplyVoltagesScale*loRegisters[currentModule].
                                                                        adcData)/LO_ADC_RANGE;
            break;
        /* The D multiplier current is given by 100*(adcData/65536) */
        case AMC_MULTIPLIER_D_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                multiplierDCurrent[CURRENT_VALUE]=(LO_AMC_MULTIPLIER_C_SCALE*loRegisters[currentModule].
                                                                              adcData)/LO_ADC_RANGE;
            break;
        /* The E gate voltage is given by  10*(adcData/65536) */
        case AMC_GATE_E_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                gateEVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                    adcData)/LO_ADC_RANGE;
            break;
        /* The E drain voltage is given by  10*(adcData/65536) */
        case AMC_DRAIN_E_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainEVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                     adcData)/LO_ADC_RANGE;
            break;
        /* The E drain current is given by  1000*(adcData/65536) */
        case AMC_DRAIN_E_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                drainECurrent[CURRENT_VALUE]=(LO_DRAIN_C_SCALE*loRegisters[currentModule].
                                                                adcData)/LO_ADC_RANGE;
            break;
        default:
            break;
    }

    return NO_ERROR;
}





/* Set AMC */
/*! This function allows the user to set different values for drain, gate and
    multiplier voltages in the addressed AMC.

    This function performs the following operations:
        -# Scale the analog control parameter from float to raw data
        -# Exacute a pot write cycle.

    \param port     This is the channel to be set. It can get the following
                    values:
                        - \ref AMC_DRAIN_B_VOLTAGE      -> Select the B drain
                                                           voltage
                        - \ref AMC_MULTIPLIER_D_VOLTAGE -> Select the D
                                                           multiplier voltage
                        - \ref AMC_GATE_E_VOLTAGE       -> Select the E gate
                                                           voltage
                        - \ref AMC_DRAIN_E_VOLTAGE      -> Select the E drain
                                                           voltage

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setAmc(unsigned char port){
    /* 1 - Setup the POT message */
    /* Set the stack bits for the 4 pots. These are always 0 because of the
       hardware configuration. */
    loRegisters[currentModule].
     amcPotReg.
      bitField.
       stackBit10=LO_AMC_POT_STACK_BIT_0;
    loRegisters[currentModule].
     amcPotReg.
      bitField.
       stackBit32=LO_AMC_POT_STACK_BIT_0;

    /* 2 - Scale the data according to the selected pot */
    switch(port){
        /* The drain B voltage is given by: (v=CAN_FLOAT)
           - 51*v */
        case AMC_DRAIN_B_VOLTAGE:
            loRegisters[currentModule].
             amcPotReg.
              bitField.
               pot3=LO_AMC_POT_DRAIN_B_V_SCALE(CAN_FLOAT);
            break;
        /* The multiplier D voltage is given by the straight count */
        case AMC_MULTIPLIER_D_VOLTAGE:
            loRegisters[currentModule].
             amcPotReg.
              bitField.
               pot2=CAN_BYTE;
        /* The gate E voltage is given by: (v=CAN_FLOAT)
           - 0 if v = 0.15
           - round(51*(5*v+4.25-((5*v+4.25)^2-4*(v-0.15)*(3.75-25*v))^0.5)/(2*(v-0.15))) (otherwise) */
        case AMC_GATE_E_VOLTAGE:
            loRegisters[currentModule].
             amcPotReg.
              bitField.
               pot0=LO_AMC_POT_GATE_E_V_SCALE(CAN_FLOAT);
        /* The drain E voltage is given by: (v=CAN_FLOAT)
           - 102*v */
        case AMC_DRAIN_E_VOLTAGE:
            loRegisters[currentModule].
             amcPotReg.
              bitField.
               pot1=LO_AMC_POT_DRAIN_E_V_SCALE(CAN_FLOAT);
            break;
        default:
            break;
    }

    /* 3 - Write the data to the serial access fuction */
    #ifdef DEBUG
        printf("         - Writing AMC POT\n");
    #endif /* DEBUG */

    /* If there is a problem writing POT, return it so that the value in the
       frontend variable is not going to be updates. */
    if(serialAccess(LO_AMC_DATA_WRITE,
                    loRegisters[currentModule].
                     amcPotReg.
                      integer,
                    LO_AMC_POT_DATA_SIZE,
                    LO_AMC_POT_DATA_SHIFT_SIZE,
                    LO_AMC_POT_DATA_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Since there is no real hardware read back for some of the control point,
       if no error occurred the current state is updated to reflect the issued
       command. */
    switch(port){
        case AMC_MULTIPLIER_D_VOLTAGE:
            frontend.
              cartridge[currentModule].
               lo.
                amc.
                 multiplierDVoltage[CURRENT_VALUE]=loRegisters[currentModule].
                                                    amcPotReg.
                                                     bitField.
                                                      pot2;
            break;
        default:
            break;
    }

    return NO_ERROR;
}









/* Get PA */
/*! This function return the operating information about the addressed PA. The
    resulting scaled value is stored in the \ref frontend status structure.

    The function performs the following operation:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the LO module.
        -# Scale the raw binary data with the correct unit and store the results
           in the \ref frontend variable.

    \param port     This is the selected channel to be monitored. It can get the
                    following values:
                        - \ref PA_3V_SUPPLY_VOLTAGE -> Current 3V supply bias
                                                       voltage
                        - \ref PA_5V_SUPPLY_VOLTAGE -> Current 5V supply bias
                                                       voltage

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPa(unsigned char port){

    /* Clear the LO BREG monitor selection bitfield */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint = 0x00;

    /* 1 - Select the desired monitor point
           a - update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint=LO_BREG_PA(port);

    /* 2->5 - Call the getLoAnalogMonitor function */
    if(getLoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(port){
        /* The 3V supply voltage is given by 10*(adcData/65536) */
        case PA_3V_SUPPLY_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                supplyVoltage3V[CURRENT_VALUE]=(frontend.
                                                 cartridge[currentModule].
                                                  lo.
                                                   supplyVoltagesScale*loRegisters[currentModule].
                                                                        adcData/LO_ADC_RANGE);
            break;
        /* The 5V supply voltage is given by 10*(adcData/65536) */
        case PA_5V_SUPPLY_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                supplyVoltage5V[CURRENT_VALUE]=(frontend.
                                                 cartridge[currentModule].
                                                  lo.
                                                   supplyVoltagesScale*loRegisters[currentModule].
                                                                        adcData/LO_ADC_RANGE);
            break;
        default:
            break;
    }

    return NO_ERROR;
}






/* Get PA Channel */
/*! This function returns the operating information about the addressed
    PA_CHANNEL. The resulting scaled value is stored in the \ref frontend status
    structure.

    The function performs the following operations:
        -# Select the desired monitor point by:
            - updating BREG
        -# Execute the core get functions common to all the analog monitor
           requests for the LO module.
        -# Scale the raw binary data with the correct unit and stroe the results
           in the \ref frontend variable.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getPaChannel(void){
    /* Clear the LO BREG monitor selection bitfield */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint = 0x00;

    /* 1 - Select the desired monitor point
           a - update BREG */
    loRegisters[currentModule].
     bReg.
      bitField.
       monitorPoint=LO_BREG_PA_CHANNEL(currentPaChannelModule);

    /* 2->5 - Call the getLoAnalogMonitor function */
    if(getLoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(currentPaChannelModule){
        /* The gate voltage is given by 10*(adcData/65536) */
        case PA_CHANNEL_GATE_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentBiasModule].
                 gateVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                    adcData/LO_ADC_RANGE);
            break;
        /* The drain voltage is given by 10*(adcData/65536) */
        case PA_CHANNEL_DRAIN_VOLTAGE:
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentBiasModule].
                 drainVoltage[CURRENT_VALUE]=(LO_GATE_DRAIN_V_SCALE*loRegisters[currentModule].
                                                                     adcData/LO_ADC_RANGE);
            break;
        /* The drain current is given by 1000*(adcData/65536) */
        case PA_CHANNEL_DRAIN_CURRENT:
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentBiasModule].
                 drainCurrent[CURRENT_VALUE]=(LO_DRAIN_C_SCALE*loRegisters[currentModule].
                                                                adcData/LO_ADC_RANGE);
            break;
        default:
            break;
    }

    return NO_ERROR;
}









/* Set PA channel */
/*! This function allows the suer to set different values for gare and drain
    voltage in the addressed PA_CHANNEL.

    The function performs the following operation:
        -# Scale the analog control parameter from float to raw data
        -# Execute a pot write cycle.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setPaChannel(void){

    /* A temporary variable to hold the scaled data before assigning it to the
       correct pot. */
    unsigned char scaledData;
    /* 1 - Setup the POT message */
    /* Set the stack bits for the 4 pots. These are always 0 because of the
       hardware configuration. */
    loRegisters[currentModule].
     paPotReg.
      bitField.
       stackBit10=LO_PA_POT_STACK_BIT_0;
    loRegisters[currentModule].
     paPotReg.
      bitField.
       stackBit32=LO_PA_POT_STACK_BIT_0;

    /* 2 - Scale the data according to the selected pot. */
    switch(currentPaChannelModule){
        /* The gate voltage is given by: (v=CAN_FLOAT)
           - 0 if v = 0.15
           - round(51*(5*v+4.25-((5*v+4.25)^2-4*(v-0.15)*(3.75-25*v))^0.5)/(2*(v-0.15))) (otherwise) */
        case PA_CHANNEL_GATE_VOLTAGE:
            scaledData=LO_PA_POT_GATE_V_SCALE(CAN_FLOAT);
            break;
        /* The drain voltage is given by: (v=CAN_FLOAT)
           - 102*v */
        case PA_CHANNEL_DRAIN_VOLTAGE:
            scaledData=LO_PA_POT_DRAIN_V_SCALE(CAN_FLOAT);
            break;
        default:
            break;
    }

    /* Assign the scaled data to the correct pot. This is a mapping because
       the assignment of channels 'A' and 'B' to polarizations '0' and '1' is
       dependent on the selected cartridge. */
    switch(LO_PA_CURRENT_POT(currentPaChannelModule)){
        case POT0:
            loRegisters[currentModule].
             paPotReg.
              bitField.
               pot0=scaledData;
            break;
        case POT1:
            loRegisters[currentModule].
             paPotReg.
              bitField.
               pot1=scaledData;
            break;
        case POT2:
            loRegisters[currentModule].
             paPotReg.
              bitField.
               pot2=scaledData;
            break;
        case POT3:
            loRegisters[currentModule].
             paPotReg.
              bitField.
               pot3=scaledData;
            break;
        default:
            break;
    }

    /* 3 - Write the data to the serial access fuction */
    #ifdef DEBUG
        printf("         - Writing PA POT\n");
    #endif /* DEBUG */

    /* If there is a problem writing POT, return it so that the value in the
       frontend variable is not going to be updates. */
    if(serialAccess(LO_PA_DATA_WRITE,
                    loRegisters[currentModule].
                     paPotReg.
                      integer,
                    LO_PA_POT_DATA_SIZE,
                    LO_PA_POT_DATA_SHIFT_SIZE,
                    LO_PA_POT_DATA_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}

