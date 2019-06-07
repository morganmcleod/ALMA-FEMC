/*! \file   biasSerialInterface.c
    \brief  BIAS serial interface functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to control and operate the
    BIAS serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote BIAS monitor and control interface.
    The functions in this module will perform a series of register manipulations
    and remote access operations to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

/* Includes */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */

#include "biasSerialInterface.h"
#include "error.h"
#include "serialInterface.h"
#include "timer.h"
#include "frontend.h"
#include "debug.h"

/* Globals */
/* Externs */
/* Statics */
BIAS_REGISTERS biasRegisters[CARTRIDGES_NUMBER];

/* A static variable to hold the temperature calibration curve. The cuvre is
   stored in couple of values: {Temperature, Voltage}.
   The first value of {0,1.64654} is added to allow the software to register an
   error if the voltage is greater then the one relaive to the lowest
   temperature. In this case the search stops at index 0 which means error. */
static float calibrationCurve[CARTRIDGE_TEMP_TBL_SIZE][2]=
    {{0.000,1.64654},{1.200,1.64654},{1.400,1.64429},{1.500,1.64299},
     {1.600,1.64157},{1.700,1.64003},{1.800,1.63837},{1.900,1.63660},
     {2.000,1.63472},{2.100,1.63274},{2.200,1.63067},{2.300,1.62852},
	 {2.400,1.62629},{2.500,1.62400},{2.600,1.62166},{2.700,1.61928},
     {2.800,1.61687},{2.900,1.61445},{3.000,1.61200},{3.100,1.60951},
     {3.200,1.60697},{3.300,1.60438},{3.400,1.60173},{3.500,1.59902},
     {3.600,1.59626},{3.700,1.59344},{3.800,1.59057},{3.900,1.58764},
     {4.000,1.58465},{4.200,1.57848},{4.400,1.57202},{4.600,1.56533},
     {4.800,1.55845},{5.000,1.55145},{5.200,1.54436},{5.400,1.53721},
     {5.600,1.53000},{5.800,1.52273},{6.000,1.51541},{6.500,1.49698},
     {7.000,1.47868},{7.500,1.46086},{8.000,1.44374},{8.500,1.42747},
     {9.000,1.41207},{9.500,1.39751},{10.000,1.3837},{10.500,1.37065},
     {11.000,1.35820},{11.500,1.34632},{12.000,1.33499},{12.500,1.32416},
     {13.000,1.31381},{13.500,1.30390},{14.000,1.29439},{14.500,1.28526},
     {15.000,1.27645},{15.500,1.26794},{16.000,1.25967},{16.500,1.25161},
     {17.000,1.24372},{17.500,1.23596},{18.000,1.22830},{18.500,1.22070},
     {19.000,1.21311},{19.500,1.20548},{20.000,1.197748},{21.000,1.181548},
     {22.000,1.162797},{23.000,1.140817},{24.000,1.125923},{25.000,1.119448},
     {26.000,1.115658},{27.000,1.112810},{28.000,1.110421},{29.000,1.108261},
     {30.000,1.106244},{31.000,1.104324},{32.000,1.102476},{33.000,1.100681},
     {34.000,1.098930},{35.000,1.097216},{36.000,1.095534},{37.000,1.093878},
     {38.000,1.092244},{39.000,1.090627},{40.000,1.089024},{42.000,1.085842},
     {44.000,1.082669},{46.000,1.079492},{48.000,1.076303},{50.000,1.073099},
     {52.000,1.069881},{54.000,1.066650},{56.000,1.063403},{58.000,1.060141},
     {60.000,1.056862},{65.000,1.048584},{70.000,1.040183},{75.000,1.031651},
     {77.350,1.027594},{80.000,1.022984},{85.000,1.014181},{90.000,1.005244},
     {95.000,0.996174},{100.000,0.986974},{105.000,0.977650},{110.000,0.968209},
     {115.000,0.958657},{120.000,0.949000},{125.000,0.939242},{130.000,0.929390},
     {135.000,0.919446},{140.000,0.909416},{145.000,0.899304},{150.000,0.889114},
     {155.000,0.878851},{160.000,0.868518},{165.000,0.858120},{170.000,0.847659},
     {175.000,0.837138},{180.000,0.826560},{185.000,0.815928},{190.000,0.805242},
     {195.000,0.794505},{200.000,0.783720},{205.000,0.772886},{210.000,0.762007},
     {215.000,0.751082},{220.000,0.740115},{225.000,0.729105},{230.000,0.718054},
     {235.000,0.706964},{240.000,0.695834},{245.000,0.684667},{250.000,0.673462},
     {255.000,0.662223},{260.000,0.650949},{265.000,0.639641},{270.000,0.628302},
     {273.150,0.621141},{275.000,0.616930},{280.000,0.605528},{285.000,0.594097},
     {290.000,0.582637},{295.000,0.571151},{300.000,0.559639},{305.000,0.548102},
     {310.000,0.536542},{315.000,0.524961},{320.000,0.513361},{325.000,0.501744},
     {330.000,0.490106},{335.000,0.478442},{340.000,0.466760},{345.000,0.455067},
     {350.000,0.443371},{355.000,0.431670},{360.000,0.419960},{365.000,0.408237},
     {370.000,0.396503},{375.000,0.384757},{380.000,0.373002},{385.000,0.361235},
     {390.000,0.349453},{395.000,0.337654},{400.000,0.325839},{405.000,0.314008},
     {410.000,0.302161},{415.000,0.290298},{420.000,0.278416},{425.000,0.266514},
     {430.000,0.254592},{435.000,0.242653},{440.000,0.230697},{445.000,0.218730},
     {450.000,0.206758},{455.000,0.194789},{460.000,0.182832},{465.000,0.170901},
     {470.000,0.159010},{475.000,0.147191},{480.000,0.135480},{485.000,0.123915},
     {490.000,0.112553},{495.000,0.101454},{500.000,0.090681}};

/* Temperature interpolation */
/* This function preform the interpolation with the standard curve for the
   cartridge temperature sensors. */
static float temperatureConversion(float voltage){

	float temperature, slope;
	unsigned char i;

	/* Check if voltage is lower than lowest limit. The highest limit is
	   checked by the search algorithm. */
	if(voltage<0.090681){
	    return CARTRIDGE_TEMP_CONV_ERR;
	}

	/* Find position in the calibration curve */
	for(i=0;
        (i<CARTRIDGE_TEMP_TBL_SIZE)&&(voltage<=calibrationCurve[i][1]);
        i++);

	/* If not found return error */
	if((i==CARTRIDGE_TEMP_TBL_SIZE)||(i==0)){
        return CARTRIDGE_TEMP_CONV_ERR;
	}

	/* Calculate the slope in the interval containing the voltage */
	slope =(calibrationCurve[i+1][0]-calibrationCurve[i][0])/(calibrationCurve[i+1][1]-calibrationCurve[i][1]);

	/* Calculate the temperature */
	temperature = calibrationCurve[i][0]+slope*(voltage-calibrationCurve[i][1]);

	return temperature;
}

/* BIAS analog monitor request core.
   This function performs the core operations that are common to all the analog
   monitor request for the BIAS module:
       - Write the BIAS AREG with a parallel output write cycle
       - Initiate an ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle that get the raw data

   If an error happens during the process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getBiasAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of which the first two are
       to be ignored. This variable allowes manipulation of data so that the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];


    /* Parallel write AREG */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing AREG\n");
    #endif /* DEBUG_BIAS_SERIAL */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule, BIAS_AREG),
                    &biasRegisters[currentModule].aReg.integer,
                    BIAS_AREG_SIZE,
                    BIAS_AREG_SHIFT_SIZE,
                    BIAS_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }


/********* TEST FIX ****************/
/*** Repeat the previous operation to add 40us */
if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule, BIAS_AREG),
                &biasRegisters[currentModule].aReg.integer,
                BIAS_AREG_SIZE,
                BIAS_AREG_SHIFT_SIZE,
                BIAS_AREG_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
    return ERROR;
}
if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_AREG),
                &biasRegisters[currentModule].aReg.integer,
                BIAS_AREG_SIZE,
                BIAS_AREG_SHIFT_SIZE,
                BIAS_AREG_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
    return ERROR;
}
if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_AREG),
                &biasRegisters[currentModule].aReg.integer,
                BIAS_AREG_SIZE,
                BIAS_AREG_SHIFT_SIZE,
                BIAS_AREG_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
    return ERROR;
}
if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_AREG),
                &biasRegisters[currentModule].aReg.integer,
                BIAS_AREG_SIZE,
                BIAS_AREG_SHIFT_SIZE,
                BIAS_AREG_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
    return ERROR;
}

    /* Initiate ADC conversion
        - send ADC convert strobe command */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG_BIAS_SERIAL */
    /* If an error occurs, notify the calling function */
    if(serialAccess(BIAS_ADC_CONVERT_STROBE(currentBiasModule),
                    NULL,
                    BIAS_ADC_STROBE_SIZE,
                    BIAS_ADC_STROBE_SHIFT_SIZE,
                    BIAS_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status
        - parallel input */
    /* Setup for 1 seconds and start the asynchronous timer */
    if(startAsyncTimer(TIMER_BIAS_ADC_RDY,
                       TIMER_BIAS_TO_ADC_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG_BIAS_SERIAL
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG_BIAS_SERIAL */

        /* If an error occurs, notify the calling funtion */
        if(serialAccess(BIAS_PARALLEL_READ(currentBiasModule),
                        &biasRegisters[currentModule].statusReg.integer,
                        BIAS_STATUS_REG_SIZE,
                        BIAS_STATUS_REG_SHIFT_SIZE,
                        BIAS_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){

            /* Stop the timer. */
            if(stopAsyncTimer(TIMER_BIAS_ADC_RDY)==ERROR){
                return ERROR;
            }

            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_BIAS_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((biasRegisters[currentModule].statusReg.bitField.
                 adcReady==BIAS_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If the timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_BIAS_SERIAL, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer. */
    if(stopAsyncTimer(TIMER_BIAS_ADC_RDY)==ERROR){
        return ERROR;
    }


    /* ADC read cycle */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Reading ADC value\n");
    #endif /* DEBUG_BIAS_SERIAL */

    /* If error return the state to the calling function */
    if(serialAccess(BIAS_ADC_DATA_READ(currentBiasModule),
                    &tempAdcValue,
                    BIAS_ADC_DATA_SIZE,
                    BIAS_ADC_DATA_SHIFT_SIZE,
                    BIAS_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    biasRegisters[currentModule].
     adcData = tempAdcValue[0];

    return NO_ERROR;
}

/* Get SIS mixer bias */
/*! This function return the operating voltage and current of the addressed
    SIS mixer. The resulting scaled value is stored in the frontend status
    structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the BIAS module.
        -# Scale the raw binary data the the correct unit and store the result
           in the \ref frontend variable

    \param current  This is the port to monitor. It can take the following
                    values:
                        - \ref SIS_MIXER_BIAS_VOLTAGE -> monitor the voltage
                        - \ref SIS_MIXER_BIAS_CURRENT -> monitor the current

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getSisMixerBias(unsigned char current){

    /* Clear the BIAS AREG */
    biasRegisters[currentModule].aReg.integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    biasRegisters[currentModule].aReg.bitField.monitorPoint =
        BIAS_AREG_SIS_MIXER(currentPolarizationModule, current);

    /* 2->5 Call the getBiasAnalogMonitor function */
    if(getBiasAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(current){
        /* The current is given by: 20*(adcData/65536)/Rs */
        case SIS_MIXER_BIAS_CURRENT:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                sis.current = (BIAS_ADC_SIS_I_SCALE * biasRegisters[currentModule].adcData / 
                (frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                sis.resistor * BIAS_ADC_RANGE));
            break;
        /* The voltage is given by: 50*(adcData/65536) */
        case SIS_MIXER_BIAS_VOLTAGE:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                sis.voltage = (BIAS_ADC_SIS_V_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}

/* Set SIS mixer bias */
/*! This function control the bias voltage setpoint for the currently addressed
    SIS mixer. The scaling factor is ture only when the mixer is operated in
    closed-loop mode.

    This function performs the following operations:
        -# Scale the analog control parameter from float to raw 16-bit data.
        -# Execute a DAC2 write cycle.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setSisMixerBias(void){

    /* Setup the DAC2 message */
    /* Select the register to address */
    biasRegisters[currentModule].dac2Reg.bitField.
       inputRegister=BIAS_DAC2_REGISTER(BIAS_DAC2_SIS_MIXER_VOLTAGE(currentPolarizationModule));
    /* Setup quick load */
    biasRegisters[currentModule].dac2Reg.bitField.quickLoad=NO;
    /* 1 - Format the data according to the dac specifications. */
    biasRegisters[currentModule].dac2Reg.bitField.data=BIAS_DAC2_SIS_MIXER_V_SCALE(CONV_FLOAT);

    /* 2 - Write the data to the serial access function */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing DAC2: %u\n",
               biasRegisters[currentModule].dac2Reg.bitField.data);
    #endif /* DEBUG_BIAS_SERIAL */

    /* If there is a problem writing DAC2, return the error. */
    if(serialAccess(BIAS_DAC_DATA_WRITE(currentBiasModule,BIAS_DAC2),
                    biasRegisters[currentModule].dac2Reg.integer,
                    BIAS_DAC2_DATA_SIZE,
                    BIAS_DAC2_DATA_SHIFT_SIZE,
                    BIAS_DAC2_DATA_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}

/* Set SIS mixer bias loop */
/*! This function control the bias loop mode for the currently addressed SIS
    mixer.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param biasMode This is the mode of operation of the SIS mixer. The possible
                    states are:
                        - \ref SIS_MIXER_BIAS_MODE_OPEN     -> open loop
                        - \ref SIS_MIXER_BIAS_MODE_CLOSE    -> closed loop
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setSisMixerLoop(unsigned char biasMode){

    /* Store the current value of the bias mode in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=biasRegisters[currentModule].bReg.integer;

    /* Update BREG. */
    switch(biasMode){
        case SIS_MIXER_BIAS_MODE_OPEN:
            biasRegisters[currentModule].bReg.bitField.
               sisBiasMode|=BIAS_BREG_SIS_MODE(currentPolarizationModule);
            break;
        case SIS_MIXER_BIAS_MODE_CLOSE:
            biasRegisters[currentModule].bReg.bitField.
               sisBiasMode&=~BIAS_BREG_SIS_MODE(currentPolarizationModule);
            break;
        default:
            break;
    }
    /* 1 - Parallel write BREG */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_BIAS_SERIAL */
    if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_BREG),
                    &biasRegisters[currentModule].bReg.integer,
                    BIAS_BREG_SIZE,
                    BIAS_BREG_SHIFT_SIZE,
                    BIAS_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        biasRegisters[currentModule].bReg.integer = tempBReg;

        return ERROR;
    }

    /* Since there is no real hardware read-back, if no erro occurred, the
       current state is updated to reflect the issued command. */
    frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
        sis.openLoop = (biasMode == SIS_MIXER_BIAS_MODE_OPEN) ? SIS_MIXER_BIAS_MODE_OPEN : SIS_MIXER_BIAS_MODE_CLOSE;

    return NO_ERROR;
}

/* Get SIS magnet bias */
/*! This function return the operating voltage and current of the addressed
    SIS magnet. The resulting scaled value is stored in the frontend status
    structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the BIAS module.
        -# Scale the raw binary data the the correct unit and store the result
           in the \ref frontend variable.

    \param  current This is the port to monitor. It can take the following
                    values:
                        - \ref SIS_MAGNET_BIAS_VOLTAGE -> monitor the voltage
                        - \ref SIS_MAGNET_BIAS_CURRENT -> monitor the current

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */

int getSisMagnetBias(unsigned char current){

    /* Clear the BIAS AREG */
    biasRegisters[currentModule].aReg.integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    biasRegisters[currentModule].aReg.bitField.monitorPoint = 
        BIAS_AREG_SIS_MAGNET(currentPolarizationModule, current);

    /* 2->5 Call the getBiasAnalogMonitor function */
    if(getBiasAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(current){
        /* The current is given by: 20*(adcData/65536)/Rs */
        case SIS_MAGNET_BIAS_CURRENT:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                sisMagnet.current = (BIAS_ADC_SIS_MAG_I_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        /* The voltage is given by: 50*(adcData/65536) */
        case SIS_MAGNET_BIAS_VOLTAGE:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                sisMagnet.voltage = (BIAS_ADC_SIS_MAG_V_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}

/* Set SIS Maqnet bias */
/*! This function control the bias current setpoint for the currently addressed
    SIS magnet.

    This function performs the following operations:
        -# Scale the analog control parameter from float to raw 16-bit data.
        -# Execute a DAC2 write cycle.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setSisMagnetBias(void){

    /* Setup the DAC2 message */
    /* Select the register to address */
    biasRegisters[currentModule].dac2Reg.bitField.inputRegister =
        BIAS_DAC2_REGISTER(BIAS_DAC2_SIS_MAGNET_CURRENT(currentPolarizationModule));
    /* Setup quick load */
    biasRegisters[currentModule].dac2Reg.bitField.quickLoad = NO;
    /* 1 - Format the data according to the dac specifications. */
    biasRegisters[currentModule].dac2Reg.bitField.data = BIAS_DAC2_SIS_MAGNET_C_SCALE(CONV_FLOAT);

    /* 2 - Write the data to the serial access function */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing DAC2: %u\n",
               biasRegisters[currentModule].dac2Reg.bitField.data);
    #endif /* DEBUG_BIAS_SERIAL */
    if(serialAccess(BIAS_DAC_DATA_WRITE(currentBiasModule,BIAS_DAC2),
                    biasRegisters[currentModule].dac2Reg.integer,
                    BIAS_DAC2_DATA_SIZE,
                    BIAS_DAC2_DATA_SHIFT_SIZE,
                    BIAS_DAC2_DATA_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}

/* Set LNA bias enable */
/*! This function control the bias enable line for the currently addressed LNA

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable   This is the state to set the lna bias to:
                        - \ref LNA_BIAS_ENABLE   -> to enable the bias
                        - \ref LNA_BIAS_DISABLE  -> to disable the bias

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setLnaBiasEnable(unsigned char enable){

    /* Store the current value of the bias mode in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=biasRegisters[currentModule].bReg.integer;

    /* Update BREG. */
    switch(enable){
        case LNA_BIAS_ENABLE:
            biasRegisters[currentModule].bReg.bitField.lnaBiasEnable |= 
                BIAS_BREG_LNA_ENABLE(currentPolarizationModule);
            break;
        case LNA_BIAS_DISABLE:
            biasRegisters[currentModule].bReg.bitField.lnaBiasEnable &= 
                ~BIAS_BREG_LNA_ENABLE(currentPolarizationModule);
            break;
        default:
            break;
    }

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_BIAS_SERIAL */

    /* If there is a problem writing BREG, restore BREG and return the ERROR. */
    if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule, BIAS_BREG),
                    &biasRegisters[currentModule].bReg.integer,
                    BIAS_BREG_SIZE,
                    BIAS_BREG_SHIFT_SIZE,
                    BIAS_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){

        /* Restore BREG to its original saved value */
        biasRegisters[currentModule].bReg.integer = tempBReg;

        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occured the
       current state is updated to reflect the issued command. */
    frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
        lna.enable = (enable == LNA_BIAS_ENABLE) ? LNA_BIAS_ENABLE : LNA_BIAS_DISABLE;

    return NO_ERROR;
}

/* Get LNA stage */
/*! This function return the operating information about the addressed LNA
    stage. The resulting scaled value is stored in the \ref frontend status
    structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the BIAS module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getLnaStage(void){

    /* Clear the BIAS AREG */
    biasRegisters[currentModule].aReg.integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    biasRegisters[currentModule].aReg.bitField.lnaStage=BIAS_AREG_LNA_STAGE(currentLnaModule);

    biasRegisters[currentModule].aReg.bitField.
        lnaPoint = BIAS_AREG_LNA_STAGE_PORT(currentPolarizationModule, currentLnaStageModule);

    /* 2->5 Call the getBiasAnalogMonitor function */
    if(getBiasAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    switch(currentLnaStageModule){
        /* The LNA drain voltage is given by: 10*(adcData/65536) */
        case LNA_STAGE_DRAIN_V:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                lna.stage[currentLnaModule].drainVoltage = 
                (BIAS_ADC_LNA_DRAIN_V_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        /* The LNA drain current is given by: 100*(adcData/65536) */
        case LNA_STAGE_DRAIN_C:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                lna.stage[currentLnaModule].drainCurrent =
                (BIAS_ADC_LNA_DRAIN_C_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        /* The LNA gate voltage is given by: 10*(adcData/65536) */
        case LNA_STAGE_GATE_V:
            frontend.cartridge[currentModule].polarization[currentBiasModule].sideband[currentPolarizationModule].
                lna.stage[currentLnaModule].gateVoltage = 
                (BIAS_ADC_LNA_GATE_V_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;
            break;
        default:
            break;
    }
    return NO_ERROR;
}

/* Set LNA stage */
/*! This function allow the user to set different values for the drain current
    and drain voltage of the addressed lna stage.

    This function performs the following operations:
        -# Scale the analog control parameter from float to raw 14-bit data.
        -# Wait for DAC1 to be ready
        -# Execute a DAC1 write cycle.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setLnaStage(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* 1 - Setup the DAC1 message */
    /* Set the toggle mode. Since we're not using the toggle function, this bit
       is always to 0. */
    biasRegisters[currentModule].dac1Reg.bitField.toggleABSelect=BIAS_DAC1_TOGGLE_OFF;
    /* Set the read/_write mode. Since the read mode is not supported by the
       hardware, this bit is always set to 0. */
    biasRegisters[currentModule].dac1Reg.bitField.readWrite=BIAS_DAC1_READ_WRITE;
    /* Select the channel to write to. */
    biasRegisters[currentModule].dac1Reg.bitField.channel = 
        BIAS_DAC1_LNA_STAGE_PORT(currentPolarizationModule, currentLnaStageModule, currentLnaModule);
    /* Select the input register to update. */
    biasRegisters[currentModule].dac1Reg.bitField.inputRegister=BIAS_DAC1_INPUT_DATA_REGISTER;
    /* Scale the data to conform to the DAC1 requirements */
    switch(currentLnaStageModule){
        /* The LNA drain voltage is given by: (Vds/5)*16384 */
        case LNA_STAGE_DRAIN_V:
            biasRegisters[currentModule].dac1Reg.bitField.data = BIAS_DAC1_LNA_STAGE_DRAIN_V_SCALE(CONV_FLOAT);
            break;
        /* The LNA drain voltage is given by: (Ids/50)*16384 */
        case LNA_STAGE_DRAIN_C:
            biasRegisters[currentModule].dac1Reg.bitField.data=  BIAS_DAC1_LNA_STAGE_DRAIN_C_SCALE(CONV_FLOAT);
            break;
        default:
            break;
   }

   /* 2 - Wait on DAC1 ready status
       - parallel input */

   /* Setup for 1 seconds and start the asynchronous timer */
   if(startAsyncTimer(TIMER_BIAS_DAC1_RDY,
                      TIMER_BIAS_TO_DAC1_RDY,
                      FALSE)==ERROR){
       return ERROR;
   }

   do {
       #ifdef DEBUG_BIAS_SERIAL
           printf("         - Waiting on DAC1 ready\n");
       #endif /* DEBUG_BIAS_SERIAL */

       /* If there is a problem writing to DAC1, return error so that the
          frontend variable is not going to be updated. */
       if(serialAccess(BIAS_PARALLEL_READ(currentBiasModule), 
                       &biasRegisters[currentModule].statusReg.integer,
                       BIAS_STATUS_REG_SIZE,
                       BIAS_STATUS_REG_SHIFT_SIZE,
                       BIAS_STATUS_REG_SHIFT_DIR,
                       SERIAL_READ)==ERROR){

           /* Stop the timer. */
           if(stopAsyncTimer(TIMER_BIAS_DAC1_RDY)==ERROR){
               return ERROR;
           }

           return ERROR;
       }
       timedOut=queryAsyncTimer(TIMER_BIAS_DAC1_RDY);
       if(timedOut==ERROR){
           return ERROR;
       }
   } while ((biasRegisters[currentModule].statusReg.bitField.dac1Ready == BIAS_DAC1_BUSY) 
        && (timedOut == TIMER_RUNNING));

   /* If the timer has expired signal the error */
   if(timedOut==TIMER_EXPIRED){
       storeError(ERR_BIAS_SERIAL, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the DAC1 to become ready
       return ERROR;
   }

   /* In case of no error, clear the asynchronous timer. */
   if(stopAsyncTimer(TIMER_BIAS_DAC1_RDY)==ERROR){
       return ERROR;
   }

   /* 3 - Write the data to the serial access function */
   #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing DAC1: %u\n",
               biasRegisters[currentModule].dac1Reg.bitField.data);
   #endif /* DEBUG_BIAS_SERIAL */

   /* If there is a problem writing DAC1, return it so that the value in the
      frontend variable is not going to be updated. */
   if(serialAccess(BIAS_DAC_DATA_WRITE(currentBiasModule, BIAS_DAC1),
                   biasRegisters[currentModule].dac1Reg.integer,
                   BIAS_DAC1_DATA_SIZE,
                   BIAS_DAC1_DATA_SHIFT_SIZE,
                   BIAS_DAC1_DATA_SHIFT_DIR,
                   SERIAL_WRITE)==ERROR){
       return ERROR;
   }

   return NO_ERROR;
}

/* Set LNA led enable */
/*! This function control the LNA led status for the currently addressed
    polarization.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param enable   This is the state for the LNA led. It can take the
                        following:
                            - \ref LNA_LED_ENABLE  -> to enable the led
                            - \ref LNA_LED_DISABLE -> to disable the led
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setLnaLedEnable(unsigned char enable){

    /* Store the current value of the LNA led state in a temporary variable. We
       use a temporary variable so that if any error occurs during the update of
       the hardware state, we don't end up with a BREG describing a different
       state than the hardware one. */
    int tempBReg=biasRegisters[currentModule].bReg.integer;

    /* Update BREG. */
    biasRegisters[currentModule].bReg.bitField.lnaLedControl = 
        (enable == LNA_LED_ENABLE) ? LNA_LED_ENABLE : LNA_LED_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_BIAS_SERIAL */

    /* If there is a problem writing BREG, restore BREG and return the ERROR */
    if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_BREG),
                    &biasRegisters[currentModule].bReg.integer,
                    BIAS_BREG_SIZE,
                    BIAS_BREG_SHIFT_SIZE,
                    BIAS_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){

        /* Restore BREG to its original saved value */
        biasRegisters[currentModule].bReg.integer = tempBReg;

        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occured the
       current state is updated to reflect the issued command. */
    frontend.cartridge[currentModule].polarization[currentBiasModule].lnaLed.
        enable = (enable == LNA_LED_ENABLE) ? LNA_LED_ENABLE : LNA_LED_DISABLE;

    return NO_ERROR;
}

/* Set SIS heater enable */
/*! This function controls the SIS heater status for the currently addressed
    polarization.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

    \param enable   This is the state for the SIS heater. It can take the
                    following:
                        - \ref SIS_HEATER_ENABLE  -> enable the heater
                        - \ref SIS_HEATER_DISABLE -> disable the heater

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setSisHeaterEnable(unsigned char enable){

    /* Store the current value of the LNA led state in a temporary variable. We
       use a temporary variable so that if any error occurs during the update of
       the hardware state, we don't end up with a BREG describing a different
       state than the hardware one. */
    int tempBReg=biasRegisters[currentModule].bReg.integer;

    /* Update BREG. */
    biasRegisters[currentModule].bReg.bitField.sisHeaterControl = 
        (enable == SIS_HEATER_ENABLE) ? SIS_HEATER_ENABLE : SIS_HEATER_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_BIAS_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_BIAS_SERIAL */

    if(serialAccess(BIAS_PARALLEL_WRITE(currentBiasModule,BIAS_BREG),
                    &biasRegisters[currentModule].bReg.integer,
                    BIAS_BREG_SIZE,
                    BIAS_BREG_SHIFT_SIZE,
                    BIAS_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        biasRegisters[currentModule].bReg.integer = tempBReg;

        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occured the
       current state is updated to reflect the issued command. */
    frontend.cartridge[currentModule].polarization[currentBiasModule].sisHeater.enable = 
        (enable == SIS_HEATER_ENABLE) ? SIS_HEATER_ENABLE : SIS_HEATER_DISABLE;

    return NO_ERROR;
}

/* Get SIS heater info */
/*! This function return the operating current of the addressed SIS heater.
    The resulting scaled value is stored in the frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the BIAS module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getSisHeater(void){

    /* Clear the BIAS AREG */
    biasRegisters[currentModule].aReg.integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    biasRegisters[currentModule].aReg.bitField.monitorPoint=BIAS_AREG_SIS_HEATER;

    /* 2->5 Call the getBiasAnalogMonitor function */
    if(getBiasAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* The current is given by: 4166.67*(adcData/65536) */
    frontend.cartridge[currentModule].polarization[currentBiasModule].sisHeater.current = 
        (BIAS_ADC_SIS_HEATER_I_SCALE * biasRegisters[currentModule].adcData / BIAS_ADC_RANGE);

    return NO_ERROR;
}

/* Send DAC strobes */
/*! This function will send the required strobe to the currently addressed DAC.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int setBiasDacStrobe(void){
    switch(currentPolDacModule){
        case DAC_RESET_STROBE:
            /* Send the reset strobe */
            if(serialAccess(BIAS_DAC_RESET_STROBE(currentBiasModule,currentPolSpecialMsgsModule),
                            NULL,
                            BIAS_DAC_STROBE_SIZE,
                            BIAS_DAC_STROBE_SHIFT_SIZE,
                            BIAS_DAC_STROBE_SHIFT_DIR,
                            SERIAL_WRITE)==ERROR){
                return ERROR;
            }
            break;

        case DAC_CLEAR_STROBE:
            /* Send the clear strobe.*/
            if(serialAccess(BIAS_DAC_CLEAR_STROBE(currentBiasModule),
                            NULL,
                            BIAS_DAC_STROBE_SIZE,
                            BIAS_DAC_STROBE_SHIFT_SIZE,
                            BIAS_DAC_STROBE_SHIFT_DIR,
                            SERIAL_WRITE)==ERROR){
                return ERROR;
            }
            break;

        default:
            break;
    }

    return NO_ERROR;
}

/* Get Temperature sensor */
/*! This function return the operating information about the addressed
    temperature sensor. The resulting scaled value is stored in the
    \ref frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of funtions common to all the analog monitor
           requests for the BIAS module.
        -# Scale the raw binary data with the correct unit and store the result
           in the \ref frontend variable

    \param  polarization    This is the selected polariation that contains the
                            bias module to which the sensor is connected. It can
                            get the following values:
                                - \ref POLARIZATION0 -> Polarization 0
                                - \ref POLARIZATION1 -> Polarization 1

    \param sensor           This is the actual sensor we want to monitor. There
                            are three sensor connected to each polarizazion.
                            For the mapping of the sensor with an actual spot
                            inside the receiver, please refer to the ICD. It
                            can get the following value:
                                - \ref SENSOR0  -> Sensor 0
                                - \ref SENSOR1  -> Sensor 1
                                - \ref SENSOR2  -> Sensor 2

    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened
        - \ref HARDW_RNG_ERR    -> if there were problems with the conversion.
                                   This should only happen if a sensor is not
                                   installed since the conversion covers from
                                   1.2 to 500 K. */
int getTemp(unsigned char polarization, unsigned char sensor){

    /* A temporary variable to hold the read-back voltage and the converted
       temperature */
    float voltage, temperature;

    /* Set the polarization to the correct one. */
    currentBiasModule = polarization;

    /* Clear the BIAS AREG */
    biasRegisters[currentModule].aReg.integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    biasRegisters[currentModule].aReg.bitField.monitorPoint = BIAS_AREG_CARTRIDGE_TEMP;

    biasRegisters[currentModule].aReg.bitField.tempSensor = BIAS_AREG_TEMP_SENSOR(sensor);

    /* 2->5 Call the getBiasAnalogMonitor function */
    if(getBiasAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    voltage = (BIAS_ADC_CART_TEMP_V_SCALE * biasRegisters[currentModule].adcData) / BIAS_ADC_RANGE;

    /* Apply the interpolation and add the offset */
    temperature=temperatureConversion(voltage);

    /* If error during interpolation, return conversion error */
    if(temperature==CARTRIDGE_TEMP_CONV_ERR){
        CAN_STATUS = HARDW_CON_ERR;
        frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].
            temp = temperature;

        return ERROR;
    }

    /* Store the data */
    frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].
        temp = temperature + 
        frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].offset;

    return NO_ERROR;
}
