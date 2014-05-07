/*! \file   fetimSerialInterface.c
    \brief  FETIM serial interface functions

    <b> File informations: </b><br>
    Created: 2011/05/04 11:26:37 by avaccari

    <b> CVS informations: </b><br>
    \$Id: fetimSerialInterface.c,v 1.3 2013/07/12 20:16:35 mmcleod Exp $

    This files contains all the functions necessary to control and operate the
    FETIM interface.

    This module is a software implementation of the hardware description
    document relative to the remote FETIM monitor and control interface.
    The functions in this module will perform a series of register manipulations
    and remote access operations to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

/* Includes */
#include <stdio.h>      /* printf */

#include "serialInterface.h"
#include "error.h"
#include "debug.h"
#include "frontend.h"
#include "fetimSerialInterface.h"
#include "async.h"



/* Globals */
/* Externs */
/* Statics */
FETIM_REGISTERS fetimRegisters;







/* Get internal interlock temperature sensors */
/*! This function returns the temperature of the addressed internal interlock
    temperature sensor.

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int getInterlockTemp(void){

    /* Clear the FETIM AREG */
    fetimRegisters.
     aRegOut.
      integer=0x0000;

    /* Select the desired monitor point */
    fetimRegisters.
     aRegOut.
      bitField.
       monitorPoint=FETIM_AREG_OUT_TEMPERATURE(currentInterlockTempModule);

    /* Call the parallel adc monitor function */
    if(getFetimParallelMonitor()==ERROR){
        return ERROR;
    }

    /* Scale the data */
    frontend.
     fetim.
      interlock.
       sensors.
        temperature.
         intrlkTempSens[currentInterlockTempModule].
          temp[CURRENT_VALUE]=FETIM_PAR_ADC_TEMP_OFFSET+(FETIM_PAR_ADC_TEMP_SCALE*fetimRegisters.
                                                                                   parAdcData)/FETIM_PAR_ADC_RANGE;

    return NO_ERROR;
}






/* Get airflow sensors */
/*! This function returns the flow of the addressed interlock airflow sensor.

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int getInterlockFlow(void){

    /* Clear the FETIM AREG */
    fetimRegisters.
     aRegOut.
      integer=0x0000;

    /* Select the desired monitor point */
    fetimRegisters.
     aRegOut.
      bitField.
       monitorPoint=FETIM_AREG_OUT_FLOW(currentInterlockFlowModule);

    /* Call the parallel adc monitor function */
    if(getFetimParallelMonitor()==ERROR){
        return ERROR;
    }

    /* Scale the data */
    frontend.
     fetim.
      interlock.
       sensors.
        flow.
         intrlkFlowSens[currentInterlockFlowModule].
          flow[CURRENT_VALUE]=(FETIM_PAR_ADC_FLOW_SCALE*fetimRegisters.
                                                         parAdcData)/FETIM_PAR_ADC_RANGE;


    return NO_ERROR;
}




/* Get interlock glitch counter analog value */
/*! This function returns the analog value of the interlock glitch counter.

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int getIntrlkGlitchValue(void){

    /* Clear the FETIM AREG */
    fetimRegisters.
     aRegOut.
      integer=0x0000;

    /* Select the desired monitor point */
    fetimRegisters.
     aRegOut.
      bitField.
       monitorPoint=FETIM_AREG_OUT_GLITCH_VALUE;

    /* Call the parallel adc monitor function */
    if(getFetimParallelMonitor()==ERROR){
        return ERROR;
    }

    /* Scale the data */
    frontend.
     fetim.
      interlock.
       state.
        glitch.
         value[CURRENT_VALUE]=FETIM_PAR_ADC_GLITCH_OFFSET+(FETIM_PAR_ADC_GLITCH_SCALE*fetimRegisters.
                                                                                       parAdcData)/(FETIM_PAR_ADC_RANGE - 1);



    return NO_ERROR;
}






/* Get external compressor temperature sensors */
/*! This function returns the temperature of the addressed external compressor
    temperature sensor.

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int getCompressorTemp(void){

    /* Clear the FETIM BREG */
    fetimRegisters.
     bRegOut.
      integer=0x0000;

    /* Select the desired monitor point */
    fetimRegisters.
     bRegOut.
      bitField.
       monitorPoint=FETIM_BREG_OUT_TEMPERATURE(currentCompressorModule);

    /* Call the serial adc monitor function */
    switch(getFetimSerialMonitor()){
        case NO_ERROR:
            return NO_ERROR;
            break;
        case ERROR:
            return ERROR;
            break;
        case ASYNC_DONE:
            break;
        default:
            return ERROR;
            break;
    }

    /* Scale the data */
    frontend.
     fetim.
      compressor.
       temp[currentCompressorModule].
        temp[CURRENT_VALUE]=FETIM_SER_ADC_TEMP_OFFSET+(FETIM_SER_ADC_TEMP_SCALE*fetimRegisters.
                                                                                 serAdcData)/FETIM_SER_ADC_RANGE;


    return ASYNC_DONE;
}




/* Get the compressor He2 pressure */
/*! This function returns the pressure of the addressed compressor He2
    pressure sensor.

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int getCompHe2Press(void){

    /* Clear the FETIM BREG */
    fetimRegisters.
     bRegOut.
      integer=0x0000;

    /* Select the desired monitor point */
    fetimRegisters.
     bRegOut.
      bitField.
       monitorPoint=FETIM_BREG_OUT_HE2_PRESS;

    /* Call the serial adc monitor function */
    switch(getFetimSerialMonitor()){
        case NO_ERROR:
            return NO_ERROR;
            break;
        case ERROR:
            return ERROR;
            break;
        case ASYNC_DONE:
            break;
        default:
            return ERROR;
            break;
    }

    /* Scale the data */
    frontend.
     fetim.
      compressor.
       he2Press.
        pressure[CURRENT_VALUE]=FETIM_SER_ADC_HE2_PRESS_OFFSET+(FETIM_SER_ADC_HE2_PRESS_SCALE*fetimRegisters.
                                                                                               serAdcData)/FETIM_SER_ADC_RANGE;


    return ASYNC_DONE;
}






/* Set N2 fill enable */
/*! This function controls the N2 fill automatic system.

    \param enable   This is the state of the N2 fill system. It can take the
                    following:
                        - \ref N2_FILL_ENABLE  -> to enable the backing
                                                  pump
                        - \ref N2_FILL_DISABLE -> to disable the
                                                  backing pump

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int setN2FillEnable(unsigned char enable){

    /* Store the current value of the N2 fill enable state in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a CREG_OUT
       describing a different state than the hardware one. */
    int tempCRegOut = fetimRegisters.
                       cRegOut.
                        integer;

    /* Update CREG_OUT */
    fetimRegisters.
     cRegOut.
      bitField.
       n2Fill=(enable==N2_FILL_ENABLE)?N2_FILL_ENABLE:
                                       N2_FILL_DISABLE;

    if(serialAccess(FETIM_PARALLEL_WRITE(FETIM_CREG_OUT),
                    &fetimRegisters.
                      cRegOut.
                       integer,
                    FETIM_CREG_OUT_SIZE,
                    FETIM_CREG_OUT_SHIFT_SIZE,
                    FETIM_CREG_OUT_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore CREG_OUT to its original saved value */
        fetimRegisters.
         cRegOut.
          integer = tempCRegOut;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     fetim.
      dewar.
       n2Fill[CURRENT_VALUE]=(enable==N2_FILL_ENABLE)?N2_FILL_ENABLE:
                                                      N2_FILL_DISABLE;

    return NO_ERROR;

}







/* Set FE safe status */
/*! This function controls the FE safe status bit. This is set by the FEMC based
    on currently monitored parameters. The message is used by the FETIM to
    control access to certain hardware. The value can be only monitored but not
    changed via the CAN.

    \param safe     This is the state of the FE. It can take the following:
                        - \ref FE_STATUS_SAFE   -> the FE can be safely operated
                        - \ref FE_STATUS_UNSAFE -> the FE cannot be safely
                                                   operated

    \return
        - \ref NO_ERROR -> if no error occured
        - \ref ERROR    -> if something wrong happened */
int setFeSafeStatus(unsigned char safe){

    /* Store the current value of the FE safe state in a temporary
       variable. We use a temporary variable so that if any error occurs during
       the update of the hardware state, we don't end up with a DREG_OUT
       describing a different state than the hardware one. */
    int tempDRegOut = fetimRegisters.
                       dRegOut.
                        integer;

    /* Update DREG_OUT */
    fetimRegisters.
     dRegOut.
      bitField.
       feStatus=(safe==FE_STATUS_SAFE)?FE_STATUS_SAFE:
                                       FE_STATUS_UNSAFE;

    if(serialAccess(FETIM_PARALLEL_WRITE(FETIM_DREG_OUT),
                    &fetimRegisters.
                      dRegOut.
                       integer,
                    FETIM_DREG_OUT_SIZE,
                    FETIM_DREG_OUT_SHIFT_SIZE,
                    FETIM_DREG_OUT_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore CREG_OUT to its original saved value */
        fetimRegisters.
         dRegOut.
          integer = tempDRegOut;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     fetim.
      compressor.
       feStatus[CURRENT_VALUE]=(safe==FE_STATUS_SAFE)?FE_STATUS_SAFE:
                                                      FE_STATUS_UNSAFE;

    return NO_ERROR;

}







/* Get FETIM digital values */
/*! This function monitors the FETIM digital inputs.

    \param port     This is the digital channel to be monitored. It gets the
                    following values:
                        - \ref FETIM_DIG_FLOW_OOR
                        - \ref FETIM_DIG_INT_TEMP_OOR
                        - \ref FETIM_DIG_GLITCH_CNT
                        - \ref FETIM_DIG_SHTDWN_TRIG
                        - \ref FETIM_DIG_SHTDWN_DELAY
                        - \ref FETIM_DIG_SINGLE_FAIL
                        - \ref FETIM_DIG_MULTI_FAIL
                        - \ref FETIM_DIG_COMP_CBL_STA
                        - \ref FETIM_DIG_INTRLK_STA
                        - \ref FETIM_DIG_HE2_PRESS_OOR
                        - \ref FETIM_DIG_EXT_TEMP_OOR

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something went wrong */
int getFetimDigital(unsigned char port){

        /* A variable to temporarily hold the read data */
    int tempDigData = 0x0000;

    /* Read the digital data */
    if(serialAccess(FETIM_PARALLEL_READ(FETIM_BREG_IN),
                    &tempDigData,
                    FETIM_BREG_IN_SIZE,
                    FETIM_BREG_IN_SHIFT_SIZE,
                    FETIM_BREG_IN_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* If no error store the data */
    fetimRegisters.
     bRegIn.
      integer = tempDigData;

    /* Assign to the correct variable depending on the port */
    switch(port){
        case FETIM_DIG_FLOW_OOR:
            frontend.
             fetim.
              interlock.
               state.
                flowOutRng[CURRENT_VALUE]=fetimRegisters.
                                           bRegIn.
                                            bitField.
                                             intrlkFlowOutRng;
            break;
        case FETIM_DIG_TEMP_OOR:
            frontend.
             fetim.
              interlock.
               state.
                tempOutRng[CURRENT_VALUE]=fetimRegisters.
                                           bRegIn.
                                            bitField.
                                             intrlkTempOutRng;
            break;
        case FETIM_DIG_GLITCH_CNT:
            frontend.
             fetim.
              interlock.
               state.
                glitch.
                 countTrig[CURRENT_VALUE]=fetimRegisters.
                                           bRegIn.
                                            bitField.
                                             glitchCntTrig;
            break;
        case FETIM_DIG_SHTDWN_TRIG:
            frontend.
             fetim.
              interlock.
               state.
                shutdownTrig[CURRENT_VALUE]=fetimRegisters.
                                             bRegIn.
                                              bitField.
                                               shutdownTrig;
            break;
        case FETIM_DIG_SHTDWN_DELAY:
            frontend.
             fetim.
              interlock.
               state.
                delayTrig[CURRENT_VALUE]=fetimRegisters.
                                          bRegIn.
                                           bitField.
                                            shutdownDelayTrig;
            break;
        case FETIM_DIG_SINGLE_FAIL:
            frontend.
             fetim.
              interlock.
               sensors.
                singleFail[CURRENT_VALUE]=fetimRegisters.
                                           bRegIn.
                                            bitField.
                                             singleFail;
            break;
        case FETIM_DIG_MULTI_FAIL:
            frontend.
             fetim.
              interlock.
               state.
                multiFail[CURRENT_VALUE]=fetimRegisters.
                                          bRegIn.
                                           bitField.
                                            multiFail;
            break;
        case FETIM_DIG_COMP_CBL_STA:
            frontend.
             fetim.
             compressor.
              cableStatus[CURRENT_VALUE]=fetimRegisters.
                                          bRegIn.
                                           bitField.
                                            compCableStatus;
            break;
        case FETIM_DIG_INTRLK_STA:
            frontend.
             fetim.
              compressor.
               intrlkStatus[CURRENT_VALUE]=fetimRegisters.
                                            bRegIn.
                                             bitField.
                                              compIntrlkStatus;
            break;
        case FETIM_DIG_HE2_PRESS_OOR:
            frontend.
             fetim.
              compressor.
               he2Press.
                pressOutRng[CURRENT_VALUE]=fetimRegisters.
                                            bRegIn.
                                             bitField.
                                              he2PressOutRng;
            break;
        case FETIM_DIG_EXT_TEMP_OOR: // currentCompressorModule
            switch(currentCompressorModule){
                case EXT_TEMP_1:
                    frontend.
                     fetim.
                      compressor.
                       temp[currentCompressorModule].
                        tempOutRng[CURRENT_VALUE]=fetimRegisters.
                                                   bRegIn.
                                                    bitField.
                                                     compExtTemp1OutRng;
                    break;
                case EXT_TEMP_2:
                    frontend.
                     fetim.
                      compressor.
                       temp[currentCompressorModule].
                        tempOutRng[CURRENT_VALUE]=fetimRegisters.
                                                   bRegIn.
                                                    bitField.
                                                     compExtTemp2OutRng;
                    break;
                default:
                    break;
            }
            break;
        default:
            break;
    }


    return NO_ERROR;
}






/* Read the paralled ADC */
int getFetimParallelMonitor(void){

    /* A variable to hold the incoming parallel ADC data */
    int tempParAdcValue = 0x0000;


    /* Perform a parallel write to select the desired monitor point */
    if(serialAccess(FETIM_PARALLEL_WRITE(FETIM_AREG_OUT),
                    &fetimRegisters.
                      aRegOut.
                       integer,
                    FETIM_AREG_OUT_SIZE,
                    FETIM_AREG_OUT_SHIFT_SIZE,
                    FETIM_AREG_OUT_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }


    /* Initiate the conversion. This includes also extra clock cycles necessary
       to allow time for the converion to complete. */
    if(serialAccess(FETIM_PAR_ADC_CONV_STROBE,
                    NULL,
                    FETIM_PAR_ADC_STROBE_SIZE,
                    FETIM_PAR_ADC_STROBE_SHIFT_SIZE,
                    FETIM_PAR_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Read the converted data */
    if(serialAccess(FETIM_PARALLEL_READ(FETIM_AREG_IN),
                    &tempParAdcValue,
                    FETIM_PAR_ADC_DATA_SIZE,
                    FETIM_PAR_ADC_DATA_SHIFT_SIZE,
                    FETIM_PAR_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the unneded bits and store the data */
    fetimRegisters.
     parAdcData=tempParAdcValue&FETIM_PAR_ADC_DATA_MASK;

    return NO_ERROR;
}




/* Read the serial ADC */
int getFetimSerialMonitor(void){

    /* A static enum to track the state of the asynchronous readout */
    static enum {
        ASYNC_FETIM_SERIAL_BREG,
        ASYNC_FETIM_SERIAL_ADC_READ
    } asyncFetimSerialState = ASYNC_FETIM_SERIAL_BREG;

    /* Switch to the correct current state */
    switch(asyncFetimSerialState){
        /* Perform a parallel write to select the desired monitor point */
        case ASYNC_FETIM_SERIAL_BREG:

            #ifdef DEBUG_FETIM_ASYNC
                if (fetimRegisters.bRegOut.bitField.monitorPoint == FETIM_BREG_OUT_HE2_PRESS)
                    printf("Async -> FETIM -> He2 Pressure -> Write BREG\n");
                else
                    printf("Async -> FETIM -> Ext Temp%d -> Write BREG\n",currentAsyncFetimExtTempModule);
            #endif /* DEBUG_FETIM_ASYNC */

            /* Parallel write BREG_OUT */
            if(serialAccess(FETIM_PARALLEL_WRITE(FETIM_BREG_OUT),
                            &fetimRegisters.
                              bRegOut.
                               integer,
                            FETIM_BREG_OUT_SIZE,
                            FETIM_BREG_OUT_SHIFT_SIZE,
                            FETIM_BREG_OUT_SHIFT_DIR,
                            SERIAL_WRITE)==ERROR){
                return ERROR;
            }

            /* Set next state */
            asyncFetimSerialState=ASYNC_FETIM_SERIAL_ADC_READ;

            break;

        /* Conversion + readback and masking of the data */
        case ASYNC_FETIM_SERIAL_ADC_READ:
            {
                /* A variable to hold the incoming serial ADC data */
                int tempSerAdcValue[2];

                #ifdef DEBUG_FETIM_ASYNC
                    if (fetimRegisters.bRegOut.bitField.monitorPoint == FETIM_BREG_OUT_HE2_PRESS)
                        printf("Async -> FETIM -> He2 Pressure -> Conv+Read\n");
                    else
                        printf("Async -> FETIM -> Ext Temp%d -> Conv+Read\n",currentAsyncFetimExtTempModule);
                #endif /* DEBUG_FETIM_ASYNC */

                /* Conversion+Read the converted data */
                if(serialAccess(FETIM_SER_ADC_ACCESS,
                                &tempSerAdcValue,
                                FETIM_SER_ADC_DATA_SIZE,
                                FETIM_SER_ADC_DATA_SHIFT_SIZE,
                                FETIM_SER_ADC_DATA_SHIFT_DIR,
                                SERIAL_READ)==ERROR){
                    /* Reset state */
                    asyncFetimSerialState=ASYNC_FETIM_SERIAL_BREG;

                    return ERROR;
                }

                /* Drop the unneded bits and store the data */
                fetimRegisters.
                 serAdcData=((unsigned int)tempSerAdcValue[0])&FETIM_SER_ADC_DATA_MASK;

                /* Set next state */
                asyncFetimSerialState=ASYNC_FETIM_SERIAL_BREG;

                return ASYNC_DONE;

                break;
            }
        default:
            /* Reset state */
            asyncFetimSerialState=ASYNC_FETIM_SERIAL_BREG;

            return ERROR;
            break;
    }

    return NO_ERROR;
}



/* Get FETIM module hardware revision level */
/*! This function reads the hardware revision level of the FETIM. If the FETIM
    is not installed, the revision level is going to be 0x1F. If this value is
    read, then the FETIM is assumed missing and it will be disabled.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something went wrong */
int getFetimHardwRevision(void){

    /* If an error occurs, notify the calling function */
    if(serialAccess(FETIM_PARALLEL_READ(FETIM_CREG_IN),
                    &fetimRegisters.
                     cRegIn.
                      integer,
                    FETIM_CREG_IN_SIZE,
                    FETIM_CREG_IN_SHIFT_SIZE,
                    FETIM_CREG_IN_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        /* If error monitoring, assume no FETIM installed */
        frontend.
         fetim.
          available=UNAVAILABLE;

        return ERROR;
    }

    /* Update the frontend variable. */
    frontend.
     fetim.
      hardwRevision=fetimRegisters.
                     cRegIn.
                      bitField.
                       hardwRev;

    /* Check if the revision correspond to NO_FETIM_HARDWARE */
    if(frontend.
        fetim.
         hardwRevision==NO_FETIM_HARDWARE){
        frontend.
         fetim.
          available=UNAVAILABLE;

          return NO_ERROR;
    }

    frontend.
     fetim.
      available=AVAILABLE;

    return NO_ERROR;
}






