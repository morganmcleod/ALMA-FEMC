/*! \file   cryostatSerialInterface.c
    \brief  Cryostat serial interface functions

    <b> File informations: </b><br>
    Created: 2007/04/10 11:26:37 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cryostatSerialInterface.c,v 1.11 2008/02/28 22:15:05 avaccari Exp $

    This files contains all the functions necessary to control and operate the
    cryostat serial interface.

    This module is a software implementation of the hardware description
    document relative to the remote cryostat monitor and control interface.
    The functions in this module will perform a series of register manipulations
    and remote access operations to achieve the desired result.
    Every function in this module will assumes that a check on the validity of
    the data has been performed before the function call.
    This module doesn't know the details on how the communication to the remote
    monitor and control device takes place. */

    /* Includes */
#include <stddef.h>     /* NULL */
#include <stdio.h>      /* printf */
#include <math.h>       /* pow */
#include <errno.h>      /* errno */


#include "cryostatSerialInterface.h"
#include "error.h"
#include "debug.h"
#include "database.h"
#include "frontend.h"
#include "serialInterface.h"
#include "timer.h"

/* Globals */
/* Externs */
/* Statics */
CRYO_REGISTERS cryoRegisters;
/* The following statics are necessary to fix a problem with the way the ADC
   circuitry work in the CRYO M&C boards.
   When quickly switching between the multiplexed analog channels the
   electronics lags behing and the first few value read are not correct.
   To fix this problem, we make sure that a value is read a sufficient number
   of time to assure that the read value is stable. If the electronics is
   changes, this part of code can be removed. */
static unsigned char lastCryostatModule;
static unsigned char readouts=CRYOSTAT_ANALOG_READOUTS;


/* CRYO analog monitor request core.
   This function performs the core operation that are common to all the analog
   monitor requests for the CRYO module:
       - Write the CRYO AREG with a parallel output write cycle
       - Initiate an ADC conversion:
           - with a convert strobe command
       - Wait on ADC ready status:
           - with a parallel input read cycle
       - Execute an ADC read cycle that gets the raw data.

   If an error happens during the process it will return ERROR, otherwise
   NO_ERROR will be returned. */
static int getCryoAnalogMonitor(void){

    /* A temporary variable to deal with the timer. */
    int timedOut;

    /* A temporary variable to hold the ADC value. This is necessary because
       the returned ADC value is actually 18 bits of which the first two are
       to be ignore. This variable allowes manipulation of data so that the
       stored one is only the real 16 bit value. */
    int tempAdcValue[2];


    /* Check if this is the last analog channel read. If not, then reload
       the readouts count. */
    if(currentCryostatModule!=lastCryostatModule){
        lastCryostatModule=currentCryostatModule;
        readouts=CRYOSTAT_ANALOG_READOUTS;
    }

    /* Parallel write AREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing AREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    /* The function to write the data to the hardware is called passing the
       intermediate buffer. If an error occurs, notify the calling function. */
    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_AREG),
                    &cryoRegisters.
                      aReg.
                       integer,
                    CRYO_AREG_SIZE,
                    CRYO_AREG_SHIFT_SIZE,
                    CRYO_AREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Initiate ADC conversion:
       - send ADC convert strobe command */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Initiating ADC conversion\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    /* If an error occurs, notify the calling function */
    if(serialAccess(CRYO_ADC_CONVERT_STROBE,
                    NULL,
                    CRYO_ADC_STROBE_SIZE,
                    CRYO_ADC_STROBE_SHIFT_SIZE,
                    CRYO_ADC_STROBE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }

    /* Wait on ADC ready status:
       - parallel input */
    /* Setup for 1 second and start th asynchronous timer */
    if(startAsyncTimer(TIMER_CRYO_ADC_RDY,
                       TIMER_CRYO_TO_ADC_RDY)==ERROR){
        return ERROR;
    }

    do {
        #ifdef DEBUG_CRYOSTAT_SERIAL
            printf("         - Waiting on ADC ready\n");
        #endif /* DEBUG_CRYOSTAT_SERIAL */

        /* If an error occurs, notify the calling function */
        if(serialAccess(CRYO_PARALLEL_READ,
                        &cryoRegisters.
                          statusReg.
                           integer,
                        CRYO_STATUS_REG_SIZE,
                        CRYO_STATUS_REG_SHIFT_SIZE,
                        CRYO_STATUS_REG_SHIFT_DIR,
                        SERIAL_READ)==ERROR){
            return ERROR;
        }
        timedOut=queryAsyncTimer(TIMER_CRYO_ADC_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((cryoRegisters.
               statusReg.
                bitField.
                 adcReady==CRYO_ADC_BUSY)&&(timedOut==TIMER_RUNNING));

    /* If timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_CRYO_SERIAL,
                   0x01); // Error 0x01 -> Timeout while waiting for the ADC to become ready
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer */
    if(stopAsyncTimer(TIMER_CRYO_ADC_RDY)==ERROR){
        return ERROR;
    }

    /* ADC read cycle */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Reading ADC value\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    /* If error return the state to the calling function */
    if(serialAccess(CRYO_ADC_DATA_READ,
                    &tempAdcValue,
                    CRYO_ADC_DATA_SIZE,
                    CRYO_ADC_DATA_SHIFT_SIZE,
                    CRYO_ADC_DATA_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Drop the not needed bits and store the data */
    cryoRegisters.
     adcData = (unsigned int)tempAdcValue[0];

    return NO_ERROR;
}


/* Set backing pump enable */
/*! This function controls the backing pump status.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param enable   This is the state of the backinb pump. It can take the
                        following:
                            - \ref BACKING_PUMP_ENABLE  -> to enable the backing
                                                           pump
                            - \ref BACKING_PUMP_DISABLE -> to disable the
                                                           backing pump

        \return
            - \ref NO_ERROR -> if no error occured
            - \ref ERROR    -> if something wrong happened */
int setBackingPumpEnable(unsigned char enable){

    /* Store the current value of the cryostat in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=cryoRegisters.
                  bReg.
                   integer;

    /* Update BREG. */
    cryoRegisters.
     bReg.
      bitField.
       backingPump=(enable==BACKING_PUMP_ENABLE)?BACKING_PUMP_ENABLE:
                                                 BACKING_PUMP_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_BREG),
                    &cryoRegisters.
                      bReg.
                       integer,
                    CRYO_BREG_SIZE,
                    CRYO_BREG_SHIFT_SIZE,
                    CRYO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        cryoRegisters.
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cryostat.
      backingPump.
       enable[CURRENT_VALUE]=(enable==BACKING_PUMP_ENABLE)?BACKING_PUMP_ENABLE:
                                                           BACKING_PUMP_DISABLE;

    return NO_ERROR;

}

/* Get 230V supply current */
/*! This function returns the operating 230V current of the cryostat. The
    resulting scaled value is stored in the frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of functions common to all the analog monitor
           requests for the cryostat module (verify that this is true)
        -# Scale the raw binary data to with the correct unit and store the
           result in the \ref frontend variable

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getSupplyCurrent230V(void){

    /* A float to hold the voltage in */
    float vin=0.0;

    /* Clear the CRYO AREG */
    cryoRegisters.
     aReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    cryoRegisters.
     aReg.
      bitField.
       monitorPoint=CRYO_AREG_SUPPLY_CURRENT_230V;

    /* 2->5 Call the getCryoAnalogMonitor function */
    if(getCryoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* Scale the input voltage to the right value: vin=10*(adcData/65536) */
    vin=(CRYO_ADC_VOLTAGE_IN_SCALE*cryoRegisters.
                                    adcData)/CRYO_ADC_RANGE;
    /* The current is given by 1.488645855*vin */
    frontend.
     cryostat.
      supplyCurrent230V[CURRENT_VALUE]=CRYO_ADC_SUPPLY_CURRENT_SCALE*vin;

    /* If the total number of reads before the current reading is stable is
       not reached then return the retry code. */
    if(readouts){
        --readouts;
        CAN_STATUS = HARDW_RETRY;
        return ERROR;
    }

    return NO_ERROR;
}

/* Set turbo pump enable */
/*! This function controls the turbo pump status.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param enable   This is the state of the turbo pump. It can take the
                        following:
                            - \ref TURBO_PUMP_ENABLE  -> to enable the turbo
                                                         pump
                            - \ref TURBO_PUMP_DISABLE -> to disable the turbo
                                                         pump

        \return
            - \ref NO_ERROR -> if no error occured
            - \ref ERROR    -> if something wrong happened */
int setTurboPumpEnable(unsigned char enable){

    /* Store the current value of the cryostat in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=cryoRegisters.
                  bReg.
                   integer;

    /* Update BREG. */
    cryoRegisters.
     bReg.
      bitField.
       turboPump=(enable==TURBO_PUMP_ENABLE)?TURBO_PUMP_ENABLE:
                                             TURBO_PUMP_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_BREG),
                    &cryoRegisters.
                      bReg.
                       integer,
                    CRYO_BREG_SIZE,
                    CRYO_BREG_SHIFT_SIZE,
                    CRYO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        cryoRegisters.
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cryostat.
      turboPump.
       enable[CURRENT_VALUE]=(enable==TURBO_PUMP_ENABLE)?TURBO_PUMP_ENABLE:
                                                         TURBO_PUMP_DISABLE;

    return NO_ERROR;

}




/* Get turbo pump states */
/*! This function monitors the current error and busy states of the selected
    turbo pump. This is a read-back real hardware status bits.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getTurboPumpStates(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(CRYO_PARALLEL_READ,
                    &cryoRegisters.
                      statusReg.
                       integer,
                    CRYO_STATUS_REG_SIZE,
                    CRYO_STATUS_REG_SHIFT_SIZE,
                    CRYO_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Store the turbo pump error */
    frontend.
     cryostat.
      turboPump.
       state[CURRENT_VALUE]=cryoRegisters.
                             statusReg.
                              bitField.
                               turboPumpError;

    /* Store the turbo pump speed */
    frontend.
     cryostat.
      turboPump.
       speed[CURRENT_VALUE]=cryoRegisters.
                             statusReg.
                              bitField.
                               turboPumpSpeed;

    return NO_ERROR;
}




/* Get gate valve state */
/*! This function monitors the current state of the gate valve. This is a
    read-back of an hardware status bit.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getGateValveState(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(CRYO_PARALLEL_READ,
                    &cryoRegisters.
                      statusReg.
                       integer,
                    CRYO_STATUS_REG_SIZE,
                    CRYO_STATUS_REG_SHIFT_SIZE,
                    CRYO_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Check the gate valve state */
    switch(cryoRegisters.
            statusReg.
             bitField.
              gateValveState){
        case GATE_VALVE_SENSORS_UNKNOWN: // If the sensors configuration correspond to an unknown gate valve
            frontend.
             cryostat.
              gateValve.
               state[CURRENT_VALUE]=GATE_VALVE_UNKNOWN;
            break;
        case GATE_VALVE_SENSORS_OPEN: // If the sensors configuration correspond to a open gate valve
            frontend.
             cryostat.
              gateValve.
               state[CURRENT_VALUE]=GATE_VALVE_OPEN;
            break;
        case GATE_VALVE_SENSORS_CLOSE: // If the sensors configuration correspond to a close gate valve
            frontend.
             cryostat.
              gateValve.
               state[CURRENT_VALUE]=GATE_VALVE_CLOSE;
            break;
        default: // Any other sensor configuration is due to hardware error
            frontend.
             cryostat.
              gateValve.
               state[CURRENT_VALUE]=GATE_VALVE_ERROR;
    }

    return NO_ERROR;
}




/* Get solenoid valve state */
/*! This function monitors the current state of the solenoid valve. This is a
    read-back of an hardware status bit.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getSolenoidValveState(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(CRYO_PARALLEL_READ,
                    &cryoRegisters.
                      statusReg.
                       integer,
                    CRYO_STATUS_REG_SIZE,
                    CRYO_STATUS_REG_SHIFT_SIZE,
                    CRYO_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Check the solenoid valve state */
    switch(cryoRegisters.
            statusReg.
             bitField.
              solenoidValveState){
        case SOLENOID_VALVE_SENSORS_UNKNOWN: // If the sensors configuration correspond to an unknown solenoid valve
            frontend.
             cryostat.
              solenoidValve.
               state[CURRENT_VALUE]=SOLENOID_VALVE_UNKNOWN;
            break;
        case SOLENOID_VALVE_SENSORS_OPEN: // If the sensors configuration correspond to a open solenoid valve
            frontend.
             cryostat.
              solenoidValve.
               state[CURRENT_VALUE]=SOLENOID_VALVE_OPEN;
            break;
        case SOLENOID_VALVE_SENSORS_CLOSE: // If the sensors configuration correspond to a close solenoid valve
            frontend.
             cryostat.
              solenoidValve.
               state[CURRENT_VALUE]=SOLENOID_VALVE_CLOSE;
            break;
        default: // Any other sensor configuration is due to hardware error
            frontend.
             cryostat.
              solenoidValve.
               state[CURRENT_VALUE]=SOLENOID_VALVE_ERROR;
    }

    return NO_ERROR;
}



/* Set gate valve state */
/*! This function controls the turbo gate valve state .

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param open     This is the state of the turbo pump. It can take the
                        following:
                            - \ref GATE_VALVE_OPEN  -> to open the gate valve
                            - \ref GATE_VALVE_CLOSE -> to close the gate valve

        \return
            - \ref NO_ERROR -> if no error occured
            - \ref ERROR    -> if something wrong happened */
int setGateValveState(unsigned char state){

    /* Store the current value of the cryostat in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=cryoRegisters.
                  bReg.
                   integer;

    /* Update BREG */
    cryoRegisters.
     bReg.
      bitField.
       gateValve=(state==GATE_VALVE_OPEN)?GATE_VALVE_OPEN:
                                          GATE_VALVE_CLOSE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_BREG),
                    &cryoRegisters.
                      bReg.
                       integer,
                    CRYO_BREG_SIZE,
                    CRYO_BREG_SHIFT_SIZE,
                    CRYO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        cryoRegisters.
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    return NO_ERROR;
}




/* Set solenoid valve state */
/*! This function controls the turbo solenoid valve state .

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param enable   This is the state of the turbo pump. It can take the
                        following:
                            - \ref SOLENOID_VALVE_OPEN  -> to open the solenoid
                                                           valve
                            - \ref SOLENOID_VALVE_CLOSE -> to close the solenoid
                                                           valve

        \return
            - \ref NO_ERROR -> if no error occured
            - \ref ERROR    -> if something wrong happened */
int setSolenoidValveState(unsigned char state){

    /* Store the current value of the cryostat in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=cryoRegisters.
                  bReg.
                   integer;

    /* Update BREG */
    cryoRegisters.
     bReg.
      bitField.
       solenoidValve=(state==SOLENOID_VALVE_OPEN)?SOLENOID_VALVE_OPEN:
                                                  SOLENOID_VALVE_CLOSE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_BREG),
                    &cryoRegisters.
                      bReg.
                       integer,
                    CRYO_BREG_SIZE,
                    CRYO_BREG_SHIFT_SIZE,
                    CRYO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        cryoRegisters.
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    return NO_ERROR;
}




/* Get vacuum sensor */
/*! This function returns the pressure of the vacuum sensor. The resulting
    scaled value is stored in the frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of functions common to all the analog monitor
           requests for the cryostat module (verify that this is true)
        -# Scale the raw binary data to with the correct unit and store the
           result in the \ref frontend variable

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getVacuumSensor(void){

    /* A float to hold the voltage in and a temp */
    float vin=0.0, temp=0.0;

    /* Clear the CRYO AREG */
    cryoRegisters.
     aReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    cryoRegisters.
     aReg.
      bitField.
       monitorPoint=CRYO_AREG_PRESSURE(currentVacuumControllerModule);

    /* 2->5 Call the getCryoAnalogMonitor function */
    if(getCryoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* Scale the input voltage to the right value: vin=10*(adcData/65536) */
    vin=(CRYO_ADC_VOLTAGE_IN_SCALE*cryoRegisters.
                                    adcData)/CRYO_ADC_RANGE;
    switch(currentVacuumControllerModule){
        case CRYOSTAT_PRESSURE:
            /* The cryostat pressure is given by: 10^[(vin-7.75)/0.75] */
            temp=pow(10.0,
                     (vin+CRYO_ADC_CRYO_PRESS_OFFSET)/CRYO_ADC_CRYO_PRESS_SCALE);
            break;
        case VACUUM_PORT_PRESSURE:
            /* The vacuum port pressure is given by: 10^[(vin-6.143)/1.286] */
            temp=pow(10.0,
                     (vin+CRYO_ADC_VAC_PORT_PRESS_OFFSET)/CRYO_ADC_VAC_PORT_PRESS_SCALE);
            break;
        default:
            break;
    }

    /* Check if a domain error occurred while evaluating the pow. */
    if(errno==EDOM){
        CAN_STATUS = HARDW_CON_ERR;
        frontend.
         cryostat.
          vacuumController.
           vacuumSensor[currentVacuumControllerModule].
            pressure[CURRENT_VALUE]=CRYOSTAT_PRESS_CONV_ERR;

        return ERROR;
    }

    /* Store the data */
    frontend.
     cryostat.
      vacuumController.
       vacuumSensor[currentVacuumControllerModule].
        pressure[CURRENT_VALUE]=temp;

    /* If the total number of reads before the temperature reading is stable is
       not reached then return the retry code. */
    if(readouts){
        --readouts;
        CAN_STATUS = HARDW_RETRY;

        return ERROR;
    }


    return NO_ERROR;
}




/* Set vacuum controller enable */
/*! This function controls the vacuum controller status.

    The function will perform the following operations:
        -# Perform a parallel write of the new BREG
        -# If no error occurs, update BREG and the frontend variable with the
           new state

        \param enable   This is the state of the vacuum controller. It can take
                        the following:
                            - \ref VACUUM_CONTROLLER_ENABLE  -> to enable the
                                                                vacuum
                                                                controller
                            - \ref VACUUM_CONTROLLER_DISABLE -> to disable the
                                                                vacuum
                                                                controller

        \return
            - \ref NO_ERROR -> if no error occured
            - \ref ERROR    -> if something wrong happened */
int setVacuumControllerEnable(unsigned char enable){

    /* Store the current value of the cryostat in a temporary variable. We use
       a temporary variable so that if any error occurs during the update of the
       hardware state, we don't end up with a BREG describing a different state
       than the hardware one. */
    int tempBReg=cryoRegisters.
                  bReg.
                   integer;

    /* Update BREG. */
    cryoRegisters.
     bReg.
      bitField.
       vacuumController=(enable==VACUUM_CONTROLLER_ENABLE)?VACUUM_CONTROLLER_HRDW_ENABLE:
                                                           VACUUM_CONTROLLER_HRDW_DISABLE;

    /* 1 - Parallel write BREG */
    #ifdef DEBUG_CRYOSTAT_SERIAL
        printf("         - Writing BREG\n");
    #endif /* DEBUG_CRYOSTAT_SERIAL */

    if(serialAccess(CRYO_PARALLEL_WRITE(CRYO_BREG),
                    &cryoRegisters.
                      bReg.
                       integer,
                    CRYO_BREG_SIZE,
                    CRYO_BREG_SHIFT_SIZE,
                    CRYO_BREG_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        /* Restore BREG to its original saved value */
        cryoRegisters.
         bReg.
          integer = tempBReg;
        return ERROR;
    }

    /* Since there is no real hardware read back, if no error occurred the
       current state is updated to reflect the issued command. */
    frontend.
     cryostat.
      vacuumController.
       enable[CURRENT_VALUE]=(enable==VACUUM_CONTROLLER_ENABLE)?VACUUM_CONTROLLER_ENABLE:
                                                                VACUUM_CONTROLLER_DISABLE;

    return NO_ERROR;

}




/* Get vacuum controller state */
/*! This function monitors the current state of the vacuum controller. This is a
    read-back of an hardware status bit.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int getVacuumControllerState(void){

    /* Read the status register, if an error occurs, notify the calling
       function. */
    if(serialAccess(CRYO_PARALLEL_READ,
                    &cryoRegisters.
                      statusReg.
                       integer,
                    CRYO_STATUS_REG_SIZE,
                    CRYO_STATUS_REG_SHIFT_SIZE,
                    CRYO_STATUS_REG_SHIFT_DIR,
                    SERIAL_READ)==ERROR){
        return ERROR;
    }

    /* Store the vacuum controller state */
    frontend.
     cryostat.
      vacuumController.
       state[CURRENT_VALUE]=(cryoRegisters.
                              statusReg.
                               bitField.
                               vacuumControllerState==VACUUM_CONTROLLER_HRDW_OK?VACUUM_CONTROLLER_OK:
                                                                                VACUUM_CONTROLLER_ERROR);

    return NO_ERROR;
}





/* Get cryostat temperature */
/*! This function returns the temperature mesured by the currently addressed
    cryostat temperature sensor. The resulting scaled value is stored in the
    frontend status structure.

    This function performs the following operations:
        -# Select the desired monitor point by:
            - updating AREG
        -# Execute the core set of functions common to all the analog monitor
           requests for the cryostat module (verify that this is true)
        -# Scale the raw binary data to with the correct unit and store the
           result in the \ref frontend variable

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened

    \todo Verify that point 2 is true. */
int getCryostatTemp(void){

    /* Floats to help perform the temperature evaluation */
    float vin=0.0, resistance=0.0, temperature=0.0;

    /* Clear the CRYO AREG */
    cryoRegisters.
     aReg.
      integer=0x0000;

    /* 1 - Select the desired monitor point
           a - update AREG */
    cryoRegisters.
     aReg.
      bitField.
       monitorPoint=CRYO_AREG_TEMPERATURE(currentCryostatModule);

    /* 2->5 Call the getCryoAnalogMonitor function */
    if(getCryoAnalogMonitor()==ERROR){
        return ERROR;
    }

    /* 6 - Scale the data */
    /* Scale the input voltage to the right value: vin=10*(adcData/65536) */
    vin=(CRYO_ADC_VOLTAGE_IN_SCALE*cryoRegisters.
                                    adcData)/CRYO_ADC_RANGE;

    switch(currentCryostatModule){
        case CRYOCOOLER_4K:
        case PLATE_4K_NEAR_LINK1:
        case PLATE_4K_NEAR_LINK2:
        case PLATE_4K_FAR_SIDE1:
        case PLATE_4K_FAR_SIDE2:
        case CRYOCOOLER_12K:
        case PLATE_12K_NEAR_LINK:
        case PLATE_12K_FAR_SIDE:
        case SHIELD_TOP_12K:
            /* Find the sensor resistance */
            resistance=TVO_GAIN*vin;
            /* Apply the interpolation */
            resistance=TVO_RESISTOR_SCALE/resistance;
            temperature=frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_0]+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_1]*resistance+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_2]*pow(resistance,
                                                  2.0)+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_3]*pow(resistance,
                                                  3.0)+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_4]*pow(resistance,
                                                  4.0)+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_5]*pow(resistance,
                                                  5.0)+
                        frontend.
                         cryostat.
                          cryostatTemp[currentCryostatModule].
                           coeff[TVO_COEFF_6]*pow(resistance,
                                                  6.0);
            break;
        case CRYOCOOLER_90K:
        case PLATE_90K_NEAR_LINK:
        case PLATE_90K_FAR_SIDE:
        case SHIELD_TOP_90K:
            /* Find the sensor resistance */
            resistance=PRT_GAIN*vin;
            /* Apply the interpolation */
            if(resistance>=PRT_A_SCALE){
                resistance=resistance/PRT_B_SCALE;
                temperature=PRT_B0+
                            PRT_B1*resistance+
                            PRT_B2*pow(resistance,
                                       2.0)+
                            PRT_B3*pow(resistance,
                                       3.0)+
                            PRT_B4*pow(resistance,
                                       4.0)+
                            PRT_B5*pow(resistance,
                                       5.0)+
                            PRT_B6*pow(resistance,
                                       6.0);
            } else {
                resistance=resistance/PRT_A_SCALE;
                temperature=PRT_A0+
                            PRT_A1*resistance+
                            PRT_A2*pow(resistance,
                                       2.0)+
                            PRT_A3*pow(resistance,
                                       3.0)+
                            PRT_A4*pow(resistance,
                                       4.0)+
                            PRT_A5*pow(resistance,
                                       5.0)+
                            PRT_A6*pow(resistance,
                                       6.0);
            }

            break;
        default:
            break;
    }

    /* Check if a domain error occurred while evaluating the power.
       If error, return a default value. */
    if(errno==EDOM){
        CAN_STATUS = HARDW_CON_ERR;
        frontend.
         cryostat.
          cryostatTemp[currentCryostatModule].
           temp[CURRENT_VALUE]=CRYOSTAT_TEMP_CONV_ERR;

        return ERROR;
    }

    /* Store the data */
    frontend.
     cryostat.
      cryostatTemp[currentCryostatModule].
       temp[CURRENT_VALUE]=temperature;

    /* If the total number of reads before the temperature reading is stable is
       not reached then return the retry code. */
    if(readouts){
        --readouts;
        CAN_STATUS = HARDW_RETRY;

        return ERROR;
    }


    return NO_ERROR;
}


