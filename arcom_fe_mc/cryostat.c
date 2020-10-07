/*! \file   cryostat.c
    \brief  Cryostat functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle cryostat events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "error.h"
#include "frontend.h"
#include "cryostatSerialInterface.h"
#include "iniWrapper.h"
#include "async.h"
#include "timer.h"

#define MACRO_TVO_SENSOR_NAMES {"CRYOCOOLER_4K",    \
                                "PLATE_4K_LINK_1",  \
                                "PLATE_4K_LINK_2",  \
                                "PLATE_4K_FAR_1",   \
                                "PLATE_4K_FAR_2",   \
                                "CRYOCOOLER_12K",   \
                                "PLATE_12K_LINK",   \
                                "PLATE_12K_FAR",    \
                                "PLATE_12K_SHIELD"}  

/* Globals */
/* Externs */
unsigned char currentCryostatModule = 0;
unsigned char currentAsyncCryoTempModule = 0; /*!< This global keeps track of
                                                   the cryostat temperature
                                                   module currently addressed by
                                                   the asynchronous routine. */
int asyncCryoTempError[CRYOSTAT_TEMP_SENSORS_NUMBER]; /*!< A global to keep
                                                           track of the async
                                                           error while
                                                           monitoring cryostat
                                                           temperatures */
unsigned char currentAsyncVacuumControllerModule = 0; /*!< This global keeps
                                                           track of the cryostat
                                                           pressure module
                                                           currently addressed
                                                           by the asynchronous
                                                           routine. */
int asyncVacuumControllerError[VACUUM_SENSORS_NUMBER]; /*!< A global to keep
                                                            track of the async
                                                            error while
                                                            monitoring cryostat
                                                            pressures */
int asyncSupplyCurrent230VError; /*!< A global to keep track of the async error
                                      while monitoring the cryostat supply
                                      voltage current */

int asyncCryostaLogHoursError;   //!< Global error result from logging cold head hours


/* Statics */
static HANDLER  cryostatModulesHandler[CRYOSTAT_MODULES_NUMBER]={cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 cryostatTempHandler,
                                                                 backingPumpHandler,
                                                                 turboPumpHandler,
                                                                 gateValveHandler,
                                                                 solenoidValveHandler,
                                                                 vacuumControllerHandler,
                                                                 supplyCurrent230VHandler,
                                                                 coldHeadHoursHandler};


/* Cryostat handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the cryostat. */
void cryostatHandler(void){
    #ifdef DEBUG_CRYOSTAT
        printf(" Cryostat\n");
    #endif /* DEBUG_CRYOSTAT */

    // Check if the receiver is outfitted with the cryostat
    if(frontend.cryostat.available == UNAVAILABLE) {
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }
    // get the cryostat submodule bits:
    currentCryostatModule = (CAN_ADDRESS & CRYOSTAT_MODULES_RCA_MASK) >> CRYOSTAT_MODULES_MASK_SHIFT;

    if (currentCryostatModule < CRYOSTAT_MODULES_UNNASIGNED_RANGE_START)
        // call the cryostat subsystem handler:
        (cryostatModulesHandler[currentCryostatModule])();

    else if (currentCryostatModule < CRYOSTAT_MODULES_NUMBER &&
             currentCryostatModule >= CRYOSTAT_MODULES_TVO_RANGE_START)
    {
        // call specific TVO coeffs handler:
        specificCoeffHandler(currentCryostatModule & CRYOSTAT_TVO_SENSOR_MASK,
                             currentCryostatModule & CRYOSTAT_TVO_COEFF_MASK);
        return;
    }
    // NOTE: not checking CRYOSTAT_MODULES_TVO_RANGE_END because the check of CRYOSTAT_MODULES_NUMBER covers that.
    //  If more RCAs were added above the specific TVO coeffs then this would need to be reworked.

    // RCA is out of range:
    storeError(ERR_CRYOSTAT, ERC_MODULE_RANGE);
    CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
    return;
}

/* Cryostat initialization */
/*! This function performs all the necessary initialization for the cryostat.
    These are executed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cryostatStartup(void) {
    FILE *file;
    CFG_STRUCT  dataIn;
    unsigned char sensor, sensorNo[32];
    /* A variable to hold the section names of the cryostat configuration file
       where the TVO coefficients can be found. */
    char sensors[TVO_SENSORS_NUMBER+1][TVO_SEC_NAME_SIZE] = MACRO_TVO_SENSOR_NAMES;

    /* Cryostat availability */
    #ifndef CHECK_HW_AVAIL
        frontend.cryostat.available = TRUE;
    #else
        // get the [CRYO] available key:
        dataIn.Name=CRYO_AVAIL_KEY;
        dataIn.VarType=Cfg_Boolean;
        dataIn.DataPtr=&frontend.cryostat.available;

        if (ReadCfg(FRONTEND_CONF_FILE,
                    CRYO_CONF_FILE_SECTION,
                    &dataIn) != CRYO_AVAIL_EXPECTED) 
        {
            // not found.  Assume available for backward compat:
            frontend.cryostat.available = AVAILABLE;
        }
    #endif // CHECK_HW_AVAIL

    /* Set the currentModule variable to reflect the fact that the cryostat
       is selected. This is necessary because currentModule is the global
       variable used to select the communication channel. This is only
       necessary if the serial communication is not initiated by a CAN
       message. */
    currentModule=CRYO_MODULE;

    #ifdef DEBUG_STARTUP
        printf(" Initializing Cryostat Module...\n\n");
        printf("  - Reading Cryostat M&C module hardware revision level...\n");
    #endif

    /* Call the getCryoHardwRevision() function to read the hardware
       revision level. If error, return error and abort initialization. */
    if(getCryoHardwRevision()==ERROR){
        return ERROR;
    }

    #ifdef DEBUG_STARTUP
        printf("     Revision level: %d\n", frontend.cryostat.hardwRevision);
        printf("    done!\n\n"); // Hardware Revision Level
    #endif

    /* CRYO.INI file name, no longer loaded from INI */
    strcpy(frontend.cryostat.configFile, "CRYO.INI");

    /* Cryostat cold head hours file, no longer loaded from INI */
    strcpy(frontend.cryostat.coldHeadHoursFile, "CRYO_HRS.INI");

    // Read the cold head hours:
    /* Configure the read array */
    dataIn.Name = CRYO_HOURS_KEY;
    dataIn.VarType = Cfg_Ulong;
    dataIn.DataPtr = &frontend.cryostat.coldHeadHours;

    // Start assuming read cold head hours will succeed:
    frontend.cryostat.coldHeadHoursDirty = 0;


    // Check whether the cold head hours file exists
    if (file = fopen(frontend.cryostat.coldHeadHoursFile, "r")) {
        fclose(file);
    } else {
        // no, create it:
        file = fopen(frontend.cryostat.coldHeadHoursFile, "w");
        fprintf(file, "[%s]\n", CRYO_HOURS_FILE_SECTION);
        fclose(file);
        // Set the dirty bit since hours is by definition zero:
        frontend.cryostat.coldHeadHoursDirty = 1;
    }

    // Read the previous hours from the config file:
    //  if error, assume 0 hours and set the dirty bit.
    if (myReadCfg(frontend.cryostat.coldHeadHoursFile,
                  CRYO_HOURS_FILE_SECTION,
                  &dataIn,
                  CRYO_HOURS_FILE_EXPECTED) != NO_ERROR) 
    {
        frontend.cryostat.coldHeadHours = 0;
        frontend.cryostat.coldHeadHoursDirty = 1;
    }

    printf("Cryostat - Cold head hours: %lu\n", frontend.cryostat.coldHeadHours);
    
    /* Start the configuration */

    #ifdef DEBUG_STARTUP
        printf(" Initializing Cryostat...\n");
    #endif
    
    /* Load the coefficient for the interpolation of the TVO temperature
       sensors. The PRT sensors are hardcoded in the software. The TVO
       coefficient are loaded from the configuration file. */
    /* Read the coefficients */
    for(sensor = 0; sensor < TVO_SENSORS_NUMBER; sensor++) {

        /* Configure the read array to get the TVO sensor number */
        dataIn.Name=TVO_NO_KEY;
        dataIn.VarType=Cfg_String;
        dataIn.DataPtr=sensorNo;

        /* Access configuration file, if error, skip the configuration. */
        if(myReadCfg(frontend.cryostat.configFile,
                     TVO_NO_SECTION(sensor),
                     &dataIn,
                     TVO_NO_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print sensor information */
        #ifdef DEBUG_STARTUP
            printf("  - Loading coefficients for TVO sensor: %d...\n     [%s]\n     TVO_NO: %s\n",
                   sensor, sensors[sensor], sensorNo);
        #endif

        /* Configure the read array to get the coefficient array */
        dataIn.Name=TVO_COEFFS_KEY;
        dataIn.VarType=Cfg_F_Array;
        dataIn.DataPtr=frontend.cryostat.cryostatTemp[sensor].coeff;

        /* Access configuration file, if error, skip the configuration. */
        if(myReadCfg(frontend.cryostat.configFile,
                     TVO_COEFFS_SECTION(sensor),
                     &dataIn,
                     TVO_COEFFS_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print sensor coefficients */
        #ifdef DEBUG_STARTUP
            for(frontend.cryostat.cryostatTemp[sensor].nextCoeff = 0;
                frontend.cryostat.cryostatTemp[sensor].nextCoeff < TVO_COEFFS_NUMBER;
                frontend.cryostat.cryostatTemp[sensor].nextCoeff++)
            {
                printf("      a%d = %f\n",
                       frontend.cryostat.cryostatTemp[sensor].nextCoeff, 
                       frontend.cryostat.cryostatTemp[sensor].coeff[frontend.cryostat.cryostatTemp[sensor].nextCoeff]);
            }
            printf("    done!\n"); // TVO coefficients
        #endif /* DEBUG_STARTUP */

        /* Initialize next coeff to read to zero */
        frontend.cryostat.cryostatTemp[sensor].nextCoeff = 0;
    }

    /* The vaccum controller power up state is ON. This allows to monitor the
       pressure of the dewar even if the monitor and control system is not
       activated. To keep track of the current hardware state, we have to
       initialize the monitor of the vacuum controller to ON in order to
       have the proper reading. This is because we don't have a real read-back
       of the current state, but just a register that holds the commanded
       state. */

    #ifdef DEBUG_STARTUP
        printf("  - Set the startup state of the vacuum controller to enabled.\n");
    #endif
    frontend.cryostat.vacuumController.enable=VACUUM_CONTROLLER_ENABLE;

    /* Set the default value for the temperature and pressure sensors to
       FLOAT_UNINIT to allow verification that they've been monitored. */
    #ifdef DEBUG_STARTUP
        printf("  - Set the startup value for the cryostat sensors.\n");
        printf("    - Pressure...\n");
    #endif
    for(sensor = 0;
        sensor < VACUUM_SENSORS_NUMBER;
        sensor++) 
    {
        frontend.cryostat.vacuumController.vacuumSensor[sensor].
            pressure=FLOAT_UNINIT;
    }
    
    #ifdef DEBUG_STARTUP
        printf("    - Temperature...\n");
    #endif        
    for(sensor = 0;
        sensor < CRYOSTAT_TEMP_SENSORS_NUMBER;
        sensor++)
    {
        frontend.cryostat.cryostatTemp[sensor].temp=FLOAT_UNINIT;
    }
    #ifdef DEBUG_STARTUP
        printf("    done!\n"); // Default sensor value
        printf(" done!\n\n"); // Cryostat
    #endif

    return NO_ERROR;
}

int cryostatSensorTablesReport(void) {
    unsigned char sensor, coeff;
    char sensors[TVO_SENSORS_NUMBER+1][TVO_SEC_NAME_SIZE] = MACRO_TVO_SENSOR_NAMES;
    printf("\nCryostat sensor tables report:\n");
    for(sensor = 0; sensor < TVO_SENSORS_NUMBER; sensor++) {
        printf("Sensor %d [%s]:\n", sensor, sensors[sensor]);
        printf("TVO_COEFFS=");
        for(coeff = 0; coeff < TVO_COEFFS_NUMBER; coeff++) {
            printf("%f", frontend.cryostat.cryostatTemp[sensor].coeff[coeff]);
            if (coeff < TVO_COEFFS_NUMBER - 1)
                printf(",");
        }
        printf("\n\n");
    }
    return NO_ERROR;
}

/* Supply Current 230V Handler */
/* This function deals with all the monitor requests diected to the 230V supply
   current. There are no control messages allowed for the 230V supply
   current. */
void supplyCurrent230VHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("  230V Supply current\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_CRYOSTAT, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_CRYOSTAT, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the 230V supply current */
    if(asyncSupplyCurrent230VError==ERROR) {
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
    }

    /* Store the last known value in the outgoing message */
    CONV_FLOAT=frontend.cryostat.supplyCurrent230V;

    /* If monitor on a monitor RCA */
    if (frontend.cryostat.backingPump.enable == BACKING_PUMP_DISABLE) 
    {
        // always return HARDW_BLKD when the backing pump is off
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* If the async monitoring is disabled, notify the monitored message */
    if (asyncState == ASYNC_OFF) {
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

// Handler to set/monitor cold head hours
void coldHeadHoursHandler(void) {
    #ifdef DEBUG_CRYOSTAT
        printf("   Cold head hours\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cryostat.lastColdHeadHours)

        /* Extract the unsigned int from the CAN message. */
        changeEndianInt(CONV_CHR_ADD, CAN_DATA_ADD);

        // Set the cold head hours:
        frontend.cryostat.coldHeadHours = CONV_UINT(0);
        // Set the dirty bit to indicate this needs to be written to NV memory:
        frontend.cryostat.coldHeadHoursDirty = 1;

        #ifdef DEBUG_CRYOSTAT_ASYNC
            printf("Cryostat -> coldHeadHoursResetHandler was set to %d\n", frontend.cryostat.coldHeadHours);
        #endif /* DEBUG_CRYOSTAT_ASYNC */

        /* If everything went fine, it's a control message, we're done. */
        return;
    }


    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cryostat.lastColdHeadHours)
        return;
    }
    
    // Convert the stored data:
    // We're storing it as a unsigned long since that's what the INI library supports
    //  so cast it to a unsigned int.
    CONV_UINT(0) = (unsigned int) frontend.cryostat.coldHeadHours;

    /* If the async monitoring is disabled, notify the monitored message */
    if (asyncState==ASYNC_OFF){
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndianInt(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_INT_SIZE;
}

/* Cryostat async */
/*! This function deals with the asynchronous monitor of the analog values in
    the cryostat.
    \return
        - \ref NO_ERROR     -> if no error occured
        - \ref ASYNC_DONE   -> once all the async operations are done
        - \ref ERROR        -> if something went wrong */
int cryostatAsync(void){
    /* A static enum to track the state of the async function */
    static enum {
        ASYNC_CRYO_GET_TEMP,
        ASYNC_CRYO_GET_PRES,
        ASYNC_CRYO_GET_230V,
        ASYNC_CRYO_LOG_HOURS
    } asyncCryoGetState = ASYNC_CRYO_GET_TEMP;

    // Don't do async if there is no cryostat:
    if (frontend.cryostat.available == UNAVAILABLE)
        return NO_ERROR;

    /* Address the cryostat */
    currentModule=CRYO_MODULE;

    /* Switch to the correct state */
    switch(asyncCryoGetState){
        /* Monitor all the temperatures one at the time asynchronously */
        case ASYNC_CRYO_GET_TEMP:

            /* Get cryostat temperatures */
            asyncCryoTempError[currentAsyncCryoTempModule]=getCryostatTemp();

            /* If done or error, go to next sensor */
            switch(asyncCryoTempError[currentAsyncCryoTempModule]){
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    #ifdef DEBUG_CRYOSTAT_ASYNC
                        printf("Async -> Cryostat -> ASYNC_CRYO_GET_TEMP(%d)\n", currentAsyncCryoTempModule);
                    #endif /* DEBUG_CRYOSTAT_ASYNC */
                    asyncCryoTempError[currentAsyncCryoTempModule]=NO_ERROR;
                    break;
                case ERROR:
                    break;
                default:
                    break;
            }

            /* Next sensor, if wrap around, then next monitor next thing */
            if(++currentAsyncCryoTempModule==CRYOSTAT_TEMP_SENSORS_NUMBER){
                currentAsyncCryoTempModule-=CRYOSTAT_TEMP_SENSORS_NUMBER;
                asyncCryoGetState = ASYNC_CRYO_GET_PRES;
            }

            break;

        /* Monitor all the pressures one at the time asynchronously */
        case ASYNC_CRYO_GET_PRES:

            /* Get cryostat pressures */
            asyncVacuumControllerError[currentAsyncVacuumControllerModule]=getVacuumSensor();

            /* If done or error, go to next sensor */
            switch(asyncVacuumControllerError[currentAsyncVacuumControllerModule]){
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    #ifdef DEBUG_CRYOSTAT_ASYNC
                        printf("Async -> Cryostat -> ASYNC_CRYO_GET_PRES(%d)\n", currentAsyncVacuumControllerModule);
                    #endif /* DEBUG_CRYOSTAT_ASYNC */
                    asyncVacuumControllerError[currentAsyncVacuumControllerModule]=NO_ERROR;
                    break;
                case ERROR:
                    break;
                default:
                    break;
            }

            /* Next sensor, if wrap around, then next monitor next thing */
            if(++currentAsyncVacuumControllerModule==VACUUM_SENSORS_NUMBER){
                currentAsyncVacuumControllerModule-=VACUUM_SENSORS_NUMBER;
                asyncCryoGetState = ASYNC_CRYO_GET_230V;
            }

            break;

        /* Monitor the 230V asynchronously */
        case ASYNC_CRYO_GET_230V:

            /* Get cryostat 230V supply */
            asyncSupplyCurrent230VError = getSupplyCurrent230V();

            /* If done or error, go to next sensor */
            switch(asyncSupplyCurrent230VError){
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    #ifdef DEBUG_CRYOSTAT_ASYNC
                        printf("Async -> Cryostat -> ASYNC_CRYO_GET_230V\n");
                    #endif /* DEBUG_CRYOSTAT_ASYNC */
                    asyncSupplyCurrent230VError=NO_ERROR;
                    break;
                case ERROR:
                    break;
                default:
                    break;
            }

            // Next async state:
            asyncCryoGetState = ASYNC_CRYO_LOG_HOURS;

            return ASYNC_DONE;
            break;

        case ASYNC_CRYO_LOG_HOURS:

            // perform cryostat cold head logging:
            asyncCryostaLogHoursError = cryostatAsyncLogHours();

            switch(asyncCryostaLogHoursError) {
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    asyncCryostaLogHoursError=NO_ERROR;
                    break;
                case ERROR:
                    break;
                default:
                    break;
            }
            
            // Wrap back to first async state:
            asyncCryoGetState = ASYNC_CRYO_GET_TEMP;
            return ASYNC_DONE;
            break;

        default:
            return ERROR;
            break;
    }

    return NO_ERROR;
}


int cryostatAsyncLogHours(void) {
    float temp4K, temp12K, temp90K;
    int timedOut, cnt;   

    // A static enum to track the state of logging hours:
    static enum {
        ASYNC_CRYO_LOG_HOURS_SET_TIMER,
        ASYNC_CRYO_LOG_HOURS_READ_TIMER,
        ASYNC_CRYO_LOG_HOURS_CHECK_TEMPS
    } asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_SET_TIMER;

    switch (asyncCryoLogHoursState) {
        case ASYNC_CRYO_LOG_HOURS_SET_TIMER:

            #ifdef DEBUG_CRYOSTAT_ASYNC
                printf("Async -> Cryostat -> ASYNC_CRYO_LOG_HOURS_SET_TIMER\n");
            #endif /* DEBUG_CRYOSTAT_ASYNC */

            // Setup timer to wait for next log hours interval:
            if (startAsyncTimer(TIMER_CRYO_LOG_HOURS,
                                TIMER_CRYO_LOG_HOURS_WAIT,
                                FALSE) == ERROR) {

                // Next state: start state
                asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_SET_TIMER;
                return ERROR;
            }

            // set next state
            asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_READ_TIMER;
            break;

        case ASYNC_CRYO_LOG_HOURS_READ_TIMER:

            // check the timer:
            timedOut = queryAsyncTimer(TIMER_CRYO_LOG_HOURS);

            if (timedOut == ERROR){
                // Next state: start state
                asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_SET_TIMER;
                return ERROR;
            }

            // Check if timer expired. No need to clear the timer because
            //  it is done by the queryAsyncTimer function if expired.
            if(timedOut==TIMER_EXPIRED) {
                // set next state
                asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_CHECK_TEMPS;
            } else {
                // not expired but we need to yield to other async tasks
                return ASYNC_DONE;
            }
            break;

        case ASYNC_CRYO_LOG_HOURS_CHECK_TEMPS:

            #ifdef DEBUG_CRYOSTAT_ASYNC
                printf("Async -> Cryostat -> ASYNC_CRYO_LOG_HOURS_CHECK_TEMPS\n");
            #endif /* DEBUG_CRYOSTAT_ASYNC */

            // Decide whether to log hours based on the cold stage temps
            temp4K=frontend.cryostat.cryostatTemp[CRYOCOOLER_4K].temp;
            temp12K=frontend.cryostat.cryostatTemp[CRYOCOOLER_12K].temp;
            temp90K=frontend.cryostat.cryostatTemp[CRYOCOOLER_90K].temp;

            cnt = 0;

            // Is the 4K cryostat sensor reading valid and below the threshold indicating cryocooling?
            if (CRYOSTAT_TEMP_BELOW_MAX(temp4K, CRYOSTAT_LOG_HOURS_THRESHOLD)) {
                cnt++;  // yes.  increase the count of sensors below the threshold.
            }

            // Is the 12K sensor below the threshold?
            if (CRYOSTAT_TEMP_BELOW_MAX(temp12K, CRYOSTAT_LOG_HOURS_THRESHOLD)) {
                cnt++;  // yes.
            }
            
            // Is the 90K sensor below the threshold?
            if (CRYOSTAT_TEMP_BELOW_MAX(temp90K, CRYOSTAT_LOG_HOURS_THRESHOLD)) {
                cnt++;  // yes.
            }

            // Increment the cold head hours if at least 2 of 3 are below the threshold:
            if (cnt >= 2) {
                frontend.cryostat.coldHeadHours++;
                frontend.cryostat.coldHeadHoursDirty = 1;
            }

            asyncCryoLogHoursState = ASYNC_CRYO_LOG_HOURS_SET_TIMER;
            return ASYNC_DONE;
            break;

        default:
            return ERROR;
            break;
    }
    return NO_ERROR;
}
