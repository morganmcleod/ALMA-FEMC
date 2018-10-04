/*! \file   cryostat.c
    \brief  Cryostat functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>

    \$Id: cryostat.c,v 1.30 2011/11/09 00:40:30 avaccari Exp $

    This files contains all the functions necessary to handle cryostat events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "error.h"
#include "frontend.h"
#include "cryostatSerialInterface.h"
#include "iniWrapper.h"
#include "database.h"
#include "async.h"
#include "timer.h"

/* Globals */
/* Externs */
unsigned char currentCryostatModule = 0;
unsigned char currentColdHeadModule = 0;
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
                                                                 coldHeadHandler};


static HANDLER  coldHeadModulesHandler[CRYO_HOURS_MODULES_NUMBER]={coldHeadHoursHandler,
                                                                   coldHeadHoursResetHandler};

/* Cryostat handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the cryostat. */
void cryostatHandler(void){
    #ifdef DEBUG_CRYOSTAT
        printf(" Cryostat\n");
    #endif /* DEBUG_CRYOSTAT */

    #ifdef DATABASE_HARDW
        // Check if the receiver is outfitted with the cryostat
        if(frontend.cryostat.available == UNAVAILABLE) {
            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    currentCryostatModule=(CAN_ADDRESS&CRYOSTAT_MODULES_RCA_MASK)>>CRYOSTAT_MODULES_MASK_SHIFT;
    if(currentCryostatModule>=CRYOSTAT_MODULES_NUMBER){
        storeError(ERR_CRYOSTAT,
                   0x01); // Error 0x01 -> Cryostat submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (cryostatModulesHandler[currentCryostatModule])();
}

/* Cryostat initialization */
/*! This function performs all the necessary initialization for the cryostat.
    These are executed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cryostatStartup(void){

    #ifdef DATABASE_HARDW
        #ifdef DEBUG_STARTUP
            unsigned char coeff;
        #endif /* DEBUG_STARTUP */

        CFG_STRUCT  dataIn;
        unsigned char sensor, sensorNo[32];
        /* A variable to hold the section names of the cryostat configuration file
           where the TVO coefficients can be found. */
        char sensors[TVO_SENSORS_NUMBER+1][TVO_SEC_NAME_SIZE]={"CRYOCOOLER_4K",
                                                               "PLATE_4K_LINK_1",
                                                               "PLATE_4K_LINK_2",
                                                               "PLATE_4K_FAR_1",
                                                               "PLATE_4K_FAR_2",
                                                               "CRYOCOOLER_12K",
                                                               "PLATE_12K_LINK",
                                                               "PLATE_12K_FAR",
                                                               "PLATE_12K_SHIELD"};

        // get the [CRYO] available key:
        dataIn.
         Name=CRYO_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.cryostat.available;

        if (ReadCfg(FRONTEND_CONF_FILE,
                    CRYO_CONF_FILE_SECTION,
                    &dataIn) != CRYO_AVAIL_EXPECTED) 
        {
            // not found.  Assume available for backward compat:
            frontend.cryostat.available = AVAILABLE;
        }

        /* Set the currentModule variable to reflect the fact that the cryostat
           is selected. This is necessary because currentModule is the global
           variable used to select the communication channel. This is only
           necessary if the serial communication is not initiated by a CAN
           message. */
        currentModule=CRYO_MODULE;

        printf(" Initializing Cryostat Module...\n\n");
        printf("  - Reading Cryostat M&C module hardware revision level...\n");

        /* Call the getCryoHardwRevision() function to read the hardware
           revision level. If error, return error and abort initialization. */
        if(getCryoHardwRevision()==ERROR){
            return ERROR;
        }

        printf("     Revision level: %d\n",
               frontend.
                cryostat.
                 hardwRevision);

        printf("    done!\n\n"); // Hardware Revision Level

        /* Parse the FRONTEND.INI file to extract the name of the configuration
           file for the cryostat. */
        printf(" Cryostat configuration file: ");

        /* Configure the read array */
        dataIn.
         Name=CRYO_CONF_FILE_KEY;
        dataIn.
         VarType=Cfg_String;
        dataIn.
         DataPtr=frontend.
                  cryostat.
                   configFile;

        /* Access configuration file, if error, return skip the configuration. */
        if(myReadCfg(FRONTEND_CONF_FILE,
                     CRYO_CONF_FILE_SECTION,
                     &dataIn,
                     CRYO_CONF_FILE_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print config file */
        printf("%s\n",
               frontend.
                cryostat.
                 configFile);

// Rolled back cryostatAsyncLogHours so this is not needed for 2.8.2:
        // // Parse the FRONTEND.INI file to extract the name cryostat cold head hours file:
        // printf(" Cryostat cold head hours file:");

        // /* Configure the read array */
        // dataIn.
        //  Name=CRYO_HOURS_FILE_KEY;
        // dataIn.
        //  VarType=Cfg_String;
        // dataIn.
        //  DataPtr=frontend.
        //           cryostat.
        //            coldHeadHoursFile;

        // /* Access configuration file, if error, return skip the configuration. */
        // if(myReadCfg(FRONTEND_CONF_FILE,
        //              CRYO_HOURS_FILE_SECTION,
        //              &dataIn,
        //              CRYO_HOURS_FILE_EXPECTED)!=NO_ERROR){
        //     return NO_ERROR;
        // }

        // /* Print config file */
        // printf(" %s",
        //        frontend.
        //         cryostat.
        //          coldHeadHoursFile);

        // // Read the cold head hours:
        // /* Configure the read array */
        // dataIn.
        //  Name = CRYO_HOURS_KEY;
        // dataIn.
        //  VarType = Cfg_Ulong;
        // dataIn.
        //  DataPtr = &frontend.
        //              cryostat.
        //               coldHeadHours;

        // // Read the previous hours from the config file:
        // //  if error, assume 0 hours.
        // if (myReadCfg(frontend.
        //                cryostat.
        //                 coldHeadHoursFile,
        //              CRYO_HOURS_FILE_SECTION,
        //              &dataIn,
        //              CRYO_HOURS_FILE_EXPECTED) != NO_ERROR) 
        // {
        //     frontend.
        //      cryostat.
        //       coldHeadHours = 0;
        // }

        // printf(" hours: %lu\n", 
        //         frontend.
        //          cryostat.
        //           coldHeadHours);
        
        /* Start the configuration */

        printf(" Initializing Cryostat...\n");
        
        /* Load the coefficient for the interpolation of the TVO temperature
           sensors. The PRT sensors are hardcoded in the software. The TVO
           coefficient are loaded from the configuration file. */
        /* Read the coefficients */
        for(sensor=0;
            sensor<TVO_SENSORS_NUMBER;
            sensor++){

            /* Configure the read array to get the TVO sensor number */
            dataIn.
             Name=TVO_NO_KEY;
            dataIn.
             VarType=Cfg_String;
            dataIn.
             DataPtr=sensorNo;

            /* Access configuration file, if error, skip the configuration. */
            if(myReadCfg(frontend.
                          cryostat.
                           configFile,
                         TVO_NO_SECTION(sensor),
                         &dataIn,
                         TVO_NO_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            /* Print sensor information */
            printf("  - Loading coefficients for TVO sensor: %d...\n     [%s]\n     TVO_NO: %s\n",
                   sensor,
                   sensors[sensor],
                   sensorNo);

            /* Configure the read array to get the coefficient array */
            dataIn.
             Name=TVO_COEFFS_KEY;
            dataIn.
             VarType=Cfg_F_Array;
            dataIn.
             DataPtr=frontend.
                      cryostat.
                       cryostatTemp[sensor].
                        coeff;

            /* Access configuration file, if error, skip the configuration. */
            if(myReadCfg(frontend.
                          cryostat.
                           configFile,
                         TVO_COEFFS_SECTION(sensor),
                         &dataIn,
                         TVO_COEFFS_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            /* Print sensor coefficients */
            #ifdef DEBUG_STARTUP
                for(coeff=0;
                    coeff<TVO_COEFFS_NUMBER;
                    coeff++){
                    printf("      a%d = %f\n",
                           coeff,
                           frontend.
                            cryostat.
                             cryostatTemp[sensor].
                              coeff[coeff]);
                    }
            #endif /* DEBUG_STARTUP */

            printf("    done!\n"); // TVO coefficients
        }

    #else // If the hardware configuration database is not available
        printf(" Initializing Cryostat...\n");
    #endif /* DATABASE_HARDW */

    /* The vaccum controller power up state is ON. This allows to monitor the
       pressure of the dewar even if the monitor and control system is not
       activated. To keep track of the current hardware state, we have to
       initialize the monitor of the vacuum controller to ON in order to
       have the proper reading. This is because we don't have a real read-back
       of the current state, but just a register that holds the commanded
       state. */
    printf("  - Set the startup state of the vacuum controller to enable...");
    frontend.
     cryostat.
      vacuumController.
       enable[CURRENT_VALUE]=VACUUM_CONTROLLER_ENABLE;
    printf("done!\n"); // Vacuum controller startup state

    /* Set the default value for the temperature and pressure sensors to
       FLOAT_UNINIT to allow verification that they've been monitored. */
    printf("  - Set the startup value for the cryostat sensors...\n");
    printf("    - Pressure...");
    for(sensor=0;
        sensor<VACUUM_SENSORS_NUMBER;
        sensor++){
        frontend.
         cryostat.
          vacuumController.
           vacuumSensor[sensor].
            pressure[CURRENT_VALUE]=FLOAT_UNINIT;
    }
    printf("done!\n"); // Pressure

    printf("    - Temperature...");
    for(sensor=0;
        sensor<CRYOSTAT_TEMP_SENSORS_NUMBER;
        sensor++){
        frontend.
         cryostat.
          cryostatTemp[sensor].
           temp[CURRENT_VALUE]=FLOAT_UNINIT;
    }
    printf("done!\n"); // Temperature

    printf("    done!\n"); // Default sensor value

    printf(" done!\n\n"); // Cryostat

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
        storeError(ERR_CRYOSTAT,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_CRYOSTAT,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the 230V supply current */
    if(asyncSupplyCurrent230VError==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;

        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cryostat.
                    supplyCurrent230V[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process, gather the stored data */
        CONV_FLOAT=frontend.
                   cryostat.
                    supplyCurrent230V[CURRENT_VALUE];
    }

    /* If monitor on a monitor RCA */
    if (frontend.
         cryostat.
          backingPump.
           enable[CURRENT_VALUE] == BACKING_PUMP_DISABLE) 
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

// Handler for cold head hours retrieval and reset
void coldHeadHandler(void) {
    #ifdef DEBUG_CRYOSTAT
        printf("  Cold Head\n");
    #endif /* DEBUG_CRYOSTAT */

    /* Check if the submodule is in range */
    currentColdHeadModule = (CAN_ADDRESS & CRYO_HOURS_MODULES_RCA_MASK);

    if (currentColdHeadModule >= CRYO_HOURS_MODULES_NUMBER){
        storeError(ERR_CRYOSTAT,
                   0x01); // Error 0x01 -> Cryostat submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct handler */
    (coldHeadModulesHandler[currentColdHeadModule])();
}

// Handler to monitor cold head hours
void coldHeadHoursHandler(void) {
    #ifdef DEBUG_CRYOSTAT
        printf("   Cold head hours\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_CRYOSTAT,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_CRYOSTAT,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }
    
    // Convert the stored data:
    // We're storing it as a unsigned long since that's what the INI library supports
    //  so cast it to a unsigned int.
    CONV_UINT(0) = (unsigned int) frontend.
                                   cryostat.
                                    coldHeadHours;

    /* If the async monitoring is disabled, notify the monitored message */
    if (asyncState==ASYNC_OFF){
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndianInt(CAN_DATA_ADD,
                    CONV_CHR_ADD);
    CAN_SIZE=CAN_INT_SIZE;
}

// Handler to reset cold head hours
void coldHeadHoursResetHandler(void) {
    char buf[20];

    #ifdef DEBUG
        printf("   Reset cold head hours\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if (CAN_SIZE) {

        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cryostat.
                                    lastResetColdHeadHours)

        // If the payload is 1, reset the cold head hours:
        if (CAN_BYTE == 1) {
            // Reset the cold head hours:
            frontend.
             cryostat.
              coldHeadHours = 0;

            sprintf(buf, "%lu", frontend.
                                 cryostat.
                                  coldHeadHours);

            #ifdef DEBUG_CRYOSTAT_ASYNC
                printf("Cryostat -> coldHeadHoursResetHandler writing %s\n", buf);
            #endif /* DEBUG_CRYOSTAT_ASYNC */

            // // Write the current number of hours back to the cryostat hours log file:
            // UpdateCfg(frontend.
            //            cryostat.
            //             coldHeadHoursFile,
            //      CRYO_HOURS_FILE_SECTION,
            //      CRYO_HOURS_KEY,
            //      buf);
        }
        return;
    }

    /* If monitor on a control RCA */
    if (currentClass==CONTROL_CLASS) {
        // Return the last control message and status:
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cryostat.
                                      lastResetColdHeadHours)
        return;
    }

    /* If monitor on a monitor RCA */
    storeError(ERR_CRYOSTAT,
               0x03); // Error 0x06: Monitor message out of range
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;
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
            asyncCryoGetState = ASYNC_CRYO_GET_TEMP;

            return ASYNC_DONE;

            break;

        default:
            return ERROR;
            break;
    }

    return NO_ERROR;
}
