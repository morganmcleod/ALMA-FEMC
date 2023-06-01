/*! \file   cartridge.c
    \brief  Cartridge functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle cartridge events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "biasSerialInterface.h"
#include "iniWrapper.h"
#include "async.h"
#include "pdSerialInterface.h"
#include "timer.h"
#include "serialMux.h"

/* Statics */
static HANDLER cartridgeSubsystemHandler[CARTRIDGE_SUBSYSTEMS_NUMBER]={biasSubsystemHandler,
                                                                       loAndTempSubsystemHandler};

static HANDLER biasModulesHandler[BIAS_MODULES_NUMBER]={polarizationHandler,
                                                        polarizationHandler};

static HANDLER loAndTempModulesHandler[LO_TEMP_MODULES_NUMBER]={loHandler,
                                                                cartridgeTempSubsystemHandler};

static HANDLER cartridgeTempSubsystemModulesHandler[CARTRIDGE_TEMP_SUBSYSTEM_MODULES_NUMBER]={cartridgeTempHandler,
                                                                                              cartridgeTempHandler,
                                                                                              cartridgeTempHandler,
                                                                                              cartridgeTempHandler,
                                                                                              cartridgeTempHandler,
                                                                                              cartridgeTempHandler};

/* Externs */
unsigned char currentCartridgeSubsystem=0; /*! This global keep track of the
                                               currently addressed cartridge
                                               subsystem (WCA or CCA) */
unsigned char currentLoAndTempModule=0; /*! This global keeps track of the
                                            currently addressed LO and cartridge
                                            temperature sensors */
unsigned char currentBiasModule=0; /*! This global keeps track of the currently
                                       addressed polarization */
unsigned char currentCartridgeTempSubsystemModule=0; /*! This global keeps track
                                                         of the currently
                                                         addressed temperature
                                                         sensor */

/* Cartridge handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the cartridges. */
void cartridgeHandler(void){
    #ifdef DEBUG
        printf(" Cartridge: %d (currentModule)\n",
               currentModule);
    #endif /* DEBUG */

    if(frontend.cartridge[currentModule].available==UNAVAILABLE) {
        storeError(ERR_CARTRIDGE, ERC_MODULE_ABSENT);  //Cartridge not installed
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }
    
    /* Check the state of the cartridge */
    switch(frontend.cartridge[currentModule].state) {

        /* Check if the cartridge is in error state. If this is the case, then
           no action is allowed until the error is cleared. See ICD. */
        case CARTRIDGE_ERROR:
            storeError(ERR_CARTRIDGE, ERC_HARDWARE_ERROR);  //The cartridge is in error state
            CAN_STATUS = HARDW_ERROR; // Notify incoming message
            return;
            break;

        /* Check if the cartridge is powered before allowing monitor and control.
           This check should always be performed because if the cartridge doesn't
           go through the initialization sequence, the CPLDs in the cartridge will
           think that they should operate at 5MHz instead of 10 and the returned
           data will be half of the real data. */
        case CARTRIDGE_OFF:
            storeError(ERR_CARTRIDGE, ERC_MODULE_POWER); //The cartridge is not powered
            CAN_STATUS = HARDW_BLKD_ERR; // Notify incoming message
            return;
            break;

        /* Check if the cartridge is initializing. If it is, return the status
           but no error necessary. */
        case CARTRIDGE_INITING:
            CAN_STATUS = HARDW_BLKD_ERR;
            return;
            break;

        /* Check if the cartridge is transitioning to STANDBY2.
           If it is, return the status but no error necessary. */
        case CARTRIDGE_GO_STANDBY2:
            CAN_STATUS = HARDW_BLKD_ERR;
            return;
            break;

        default:
            break;
    }

    /* Check if the specified submodule is in range */
    currentCartridgeSubsystem=(CAN_ADDRESS&CARTRIDGE_SUBSYSTEM_RCA_MASK)>>CARTRIDGE_SUBSYSTEM_MASK_SHIFT;
    if(currentCartridgeSubsystem>=CARTRIDGE_SUBSYSTEMS_NUMBER){
        storeError(ERR_CARTRIDGE, ERC_MODULE_RANGE); //Cartridge subsystem out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct function */
    (cartridgeSubsystemHandler[currentCartridgeSubsystem])();
}

/* LO and Cartridge temperature sensors handler. */
static void loAndTempSubsystemHandler(void){
    #ifdef DEBUG
        printf("  LO or Temperature\n");
    #endif /* DEBUG */

    /* No need to check if it is outfitted since this is a fictuos subsystem
       used to redirect CAN messages */

    /* Check if the specified submodule is in range */
    currentLoAndTempModule=(CAN_ADDRESS&LO_TEMP_MODULES_RCA_MASK)>>LO_TEMP_MODULES_MASK_SHIFT;
    if(currentLoAndTempModule>=LO_TEMP_MODULES_NUMBER){
        storeError(ERR_CARTRIDGE, ERC_MODULE_RANGE); //Lo and cartridge temperature submodule out of range
        return;
    }

    /* Call the correct function */
    (loAndTempModulesHandler[currentLoAndTempModule])();
}

/* Cartridge temperature sensor handler. */
static void cartridgeTempSubsystemHandler(void){
    #ifdef DEBUG
        printf("   Cartridge Temperature\n");
    #endif /* DEBUG */

    /* No need to check if it is outfitted since this is a fictuos subsystem
       used to redirect CAN messages */

    /* Check if the the specified submodule is in range */
    currentCartridgeTempSubsystemModule=(CAN_ADDRESS&CARTRIDGE_TEMP_SUBSYSTEM_MODULES_RCA_MASK)>>CARTRIDGE_TEMP_SUBSYSTEM_MODULES_MASK_SHIFT;
    if(currentCartridgeTempSubsystemModule>=CARTRIDGE_TEMP_SUBSYSTEM_MODULES_NUMBER){
        storeError(ERR_CARTRIDGE, ERC_MODULE_RANGE); //Cartridge temperature submodule out of range
        return;
    }

    /* Since the sensor are actually controlled by the bias module while the
       RCAs are listed under the LO and Temp submodule, we have to manually
       change the subsystem to address the correct hardware. */
    currentCartridgeSubsystem=CARTRIDGE_SUBSYSTEM_BIAS;

    /* Call the correct function */
    (cartridgeTempSubsystemModulesHandler[currentCartridgeTempSubsystemModule])();
}

/* BIAS handler. */
static void biasSubsystemHandler(void){

    #ifdef DEBUG
        printf("  BIAS\n");
    #endif /* DEBUG */

    /* No need to check if it is outfitted since this is a fictuos subsystem
       used to redirect CAN messages */

    /* Check if the specified submodule is in range */
    currentBiasModule=(CAN_ADDRESS&BIAS_MODULES_RCA_MASK)>>BIAS_MODULES_MASK_SHIFT;
    if(currentBiasModule>=BIAS_MODULES_NUMBER){
        storeError(ERR_CARTRIDGE, ERC_MODULE_RANGE); //BIAS submodule out of range
        return;
    }


    /* Call the correct function */
    (biasModulesHandler[currentBiasModule])();
}


/* Cartridge stop */
/*! This function performs the operations necessary to shut down a cartidge.
    \param cartridge    This is the selected cartridge to power off
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cartridgeStop(unsigned char cartridge){

    #ifdef DEBUG_INIT
        printf("- Shutting down cartridge %d...\n",
               cartridge);
    #endif  // DEBUG_INIT

    /* Change the state of the addressed cartridge to OFF */
    frontend.cartridge[cartridge].state = CARTRIDGE_OFF;

    /* Force clear STANDBY2 mode */
    frontend.cartridge[cartridge].standby2 = FALSE;

    #ifdef DEBUG_INIT
        printf("  done!\n\n");
    #endif  // DEBUG_INIT

    return NO_ERROR;
}


/* Cartridge startup init */
/*! This function performs the operations necessary to initialize a cartridge
    during startup. These operations are performed only once during the startup
    sequence.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cartridgeStartup(void){

    /* Few variables to help load the data from the configuration file */
    CFG_STRUCT dataIn;
    float resistor=0.0;
    unsigned char sensor;

    /* A variable to hold the section names of the cartridge configuration file
       where the temperature sensors offsets can be found. */
    char sensors[CARTRIDGE_TEMP_SENSORS_NUMBER+1][SENSOR_SEC_NAME_SIZE]={"4K_STAGE",
                                                                         "110K_STAGE",
                                                                         "POL0_MIXER",
                                                                         "SPARE",
                                                                         "15K_STAGE",
                                                                         "POL1_MIXER"};

    #ifdef DEBUG_STARTUP
        printf(" Cartridge %d configuration file: %s\n",
               currentModule + 1,
               frontend.cartridge[currentModule].configFile);

        printf(" Initializing Cartridge %d...\n", currentModule+1);                                                                         
    #endif

    /* Assign current sense resistor value */
    switch(currentModule + 1) {
        case 3:
            resistor = 5.0;
            break;
        case 4:
            resistor = 5.0;
            break;
        case 5:
            resistor = 5.1;
            break;
        case 6:
            resistor = 5.0;
            break;
        case 7:
            resistor = 50.0;
            break;
        case 8:
            resistor = 5.0;
            break;
        case 9:
            resistor = 10.0;
            break;
        case 10:
            resistor = 10.0;
            break;
        default:
            resistor = 1.0;
            break;
    }

    /* Polarization Level */
    for(currentBiasModule = 0; 
        currentBiasModule < POLARIZATIONS_NUMBER; 
        currentBiasModule++) 
    {
        /* Sideband Level */
        for(currentPolarizationModule = 0;
            currentPolarizationModule < SIDEBANDS_NUMBER;
            currentPolarizationModule++)
        {
            /* SIS availability for bands 3-10: */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].sis.available
                = ((currentModule + 1) >= 3);

            /* SIS current sense resistor */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].
                sis.resistor = resistor;

            /* SIS magnet availability for bands 5-10: */
            /* Configure the read array for SIS magnet availability info */
            frontend.cartridge[currentModule].polarization[currentBiasModule].
                sideband[currentPolarizationModule].sisMagnet.available
                = ((currentModule + 1) >= 5);
        }
        /* SIS heater availability for bands 3-10: */
        frontend.cartridge[currentModule].polarization[currentBiasModule].
            sisHeater.available = ((currentModule + 1) >= 3);;
    }

    #ifdef DEBUG_STARTUP    
        printf("  - Temperature sensor offsets!\n"); 
    #endif // DEBUG_STARTUP

    /* Cartridge temp sensors offsets */
    for(sensor = 0;
        sensor < CARTRIDGE_TEMP_SENSORS_NUMBER;
        sensor++)
    {
        dataIn.Name=SENSOR_OFFSET_KEY;
        dataIn.VarType=Cfg_Float;
        dataIn.DataPtr=&frontend.cartridge[currentModule].cartridgeTemp[sensor].offset;

        /* Access configuration file, if error, skip the configuration. */
        if(myReadCfg(frontend.cartridge[currentModule].configFile,
                     SENSOR_OFFSET_SECTION(sensor),
                     &dataIn,
                     SENSOR_OFFSET_EXPECTED) != NO_ERROR)
        {
            printf("Error reading cartridge:%d sensor:%d\n", currentModule, sensor);
        }
        #ifdef DEBUG_STARTUP
            printf("    - Sensor %d [%s] offset=%f\n",
                    sensor,
                    SENSOR_OFFSET_SECTION(sensor),
                    frontend.cartridge[currentModule].cartridgeTemp[sensor].offset);
        #endif /* DEBUG_STARTUP */
    }

    #ifdef DEBUG_STARTUP    
        printf("    done!\n"); // Temperature sensor offsets
        printf(" done!\n\n"); // Cartridge
    #endif // DEBUG_STARTUP

    return NO_ERROR;
}


/* Cartrige init */
/*! This function performs the operations necessary to initialize a cartridge at
    runtime. These are executed everytime a cartridge is powered up.
    \param cartridge    This is the selected cartridge to initialize
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cartridgeInit(unsigned char cartridge){

    /* Set currentModule variable to reflect the selected cartridge.
       This is necessary because currentModule is the global variable pointing
       to the selected cartridge. */
    currentModule = cartridge;

    /* Check if the receiver is outfitted with the cartridge */
    if(frontend.cartridge[currentModule].available==UNAVAILABLE) {
        storeError(ERR_CARTRIDGE, ERC_MODULE_ABSENT); //Cartridge not installed
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return ERROR;
    }
    
    #ifdef DEBUG_INIT
        printf("Initializing cartridge (%d)\n", currentModule+1);
    #endif // DEBUG_INIT

    /* Enable the 10MHz communication for the BIAS and the LO and update the
       relative state variable. Initialize both polarization even if there might
       be only one. This is a control serial message, there will be no timeout
       even if one of the polarization is not installed. Same for the LO, if it
       is not installed, it doesn't matter. */

    /* Polarizations */
    /* Set current cartridge subsystem to bias */
    currentCartridgeSubsystem=CARTRIDGE_SUBSYSTEM_BIAS;
    /* Set the 10MHz */
    for(currentBiasModule = 0;
        currentBiasModule < POLARIZATIONS_NUMBER;
        currentBiasModule++)
    {
        /* Initialize the polarizations */
        if(polarizationInit()==ERROR) {
            return ERROR;
        }
    }

    /* LO */
    /* Set current cartridge subsystem to LO */
    currentCartridgeSubsystem=CARTRIDGE_SUBSYSTEM_LO;

    /* Initialize the LO */
    if(loInit()==ERROR) {
        return ERROR;
    }

    /* Change the state of the addressed cartridge to CARTRDIGE_READY (powered
       and initialized) */
    #ifdef DEBUG_INIT
        printf(" - Turning cartridge ON...");
    #endif // DEBUG_INIT

    /* Actual turn-on handled below in cartridgeAsync() */

    #ifdef DEBUG_INIT
        printf(" done!\n"); // Turning cartridge on
        printf("done!\n\n"); // Cartridge
    #endif // DEBUG_INIT

    return NO_ERROR;
}


/* Cartrdige async */
/*! This function deals with the asynchronous operations related to a cartridge
    \return
        - \ref NO_ERROR     -> if no error occured
        - \ref ASYNC_DONE   -> once all the async operations are done
        - \ref ERROR        -> if something went wrong */
int cartridgeAsync(void){

    /* A static to keep track of the currently addressed cartridge */
    static unsigned char currentAsyncCartridge = 0;

    /* A static to keep track of the current async task for each cartridge */
    static enum {
        ASYNC_CARTRIDGE_IDLE,
        ASYNC_CARTRIDGE_INIT,
        ASYNC_CARTRIDGE_GO_STANDBY2
    } asyncCartridgeTask = ASYNC_CARTRIDGE_IDLE;
    /* Address the current async cartridge */
    currentModule=currentAsyncCartridge;

    /* Switch depending on the cartridge task */
    switch(asyncCartridgeTask){
        case ASYNC_CARTRIDGE_IDLE:
            // Check if the cartridge was turned on
            if(frontend.cartridge[currentAsyncCartridge].state==CARTRIDGE_ON) {

                // If CARTRIDGE_ON, then next task is initialization
                asyncCartridgeTask=ASYNC_CARTRIDGE_INIT;

                // Stay with this cartridge
                return NO_ERROR;
            }
            // Check if the cartridge was put from CARTRIDGE_READY into STANDBY2 mode:
            if(frontend.cartridge[currentAsyncCartridge].state==CARTRIDGE_GO_STANDBY2) {

                // If CARTRIDGE_ON, then next task is entering STANDBY2 mode
                asyncCartridgeTask=ASYNC_CARTRIDGE_GO_STANDBY2;

                // Stay with this cartridge
                return NO_ERROR;
            }
            break;

        case ASYNC_CARTRIDGE_INIT:
            /* Initialize cartridge and switch on result */
            switch(asyncCartridgeInit()) {
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    asyncCartridgeTask=ASYNC_CARTRIDGE_IDLE;
                    break;
                case ERROR:
                    /* If there was an error in the initialization, attempt to
                       turn off the cartridge. */

                    /* Turn off the power to the cartridge. */
                    if(setPdModuleEnable(PD_MODULE_DISABLE)==ERROR){
                        /* If we end up in here, it means that something very major
                           has happened and the communication within the FEMC
                           module is compromised. At this point all the bets on
                           the state of the system are off. This would require
                           a HALT but we are just going to notify the cartridge
                           module that there was an urecoverable error with the
                           initialization and allow for a restart of the cartridge. */
                        /* Store the Error state in the last control message variable */
                        frontend.powerDistribution.pdModule[currentAsyncCartridge].
                            lastEnable.status=ERROR;

                        /* Set the state of the cartridge to 'error' */
                        frontend.cartridge[currentAsyncCartridge].
                            state=CARTRIDGE_ERROR;

                        /* Next state: IDLE */
                        asyncCartridgeTask=ASYNC_CARTRIDGE_IDLE;
                        break; //TODO:  this break is confusing.  I think it exits the case stmt.
                               //       so none of the next steps execute.
                    }

                    /*  If it worked. Mark the catridge as off. */
                    if(cartridgeStop(currentAsyncCartridge)==ERROR){
                        /* Store the Error state in the last control message variable */
                        frontend.powerDistribution.pdModule[currentAsyncCartridge].
                            lastEnable.status=ERROR;
                    }

                    /* Decrease the number of currently turned on cartridges. */
                    frontend.powerDistribution.poweredModules--;

                    #ifdef DEBUG_POWERDIS
                        printPoweredModuleCounts();
                    #endif /* DEBUG_POWERDIS */

                    asyncCartridgeTask = ASYNC_CARTRIDGE_IDLE;
                    break;

                default:
                    break;
            }
            break;

        case ASYNC_CARTRIDGE_GO_STANDBY2:
            switch(asyncCartridgeGoStandby2()) {
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    asyncCartridgeTask=ASYNC_CARTRIDGE_IDLE;
                    break;
                case ERROR:
                    asyncCartridgeTask=ASYNC_CARTRIDGE_IDLE;
                    return ERROR;
                    break;
            }
            break;

        default:
            break;
    }

    /* Next cartridge, if wrap around, then we are done with cartridges. */
    if(++currentAsyncCartridge == CARTRIDGES_NUMBER) {
        currentAsyncCartridge = 0;

        /* If all the cartrdiges have been handled then we are done */
        return ASYNC_DONE;
    }
    return NO_ERROR;
}


/* Asynchronously initialize a cartridge */
int asyncCartridgeInit(void){

    /* A static enum to track the state of the async init function */
    static enum {
        ASYNC_CARTRIDGE_INIT_SET_WAIT,
        ASYNC_CARTRIDGE_INIT_WAIT,
        ASYNC_CARTRIDGE_INIT_INIT
    } asyncCartridgeInitState = ASYNC_CARTRIDGE_INIT_SET_WAIT;

    /* Check if the cartridge was turned off in the meantime */
    if(frontend.cartridge[currentModule].state==CARTRIDGE_OFF) {

        /* If CARTRIDGE_OFF, then next task is idle */
        asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_SET_WAIT;

        /* Clear the timer */
        if(stopAsyncTimer(TIMER_CARTRIDGE_INIT)==ERROR){
            return ERROR;
        }

        /* Cartridge was turned off so nothing else to do */
        return ASYNC_DONE;

    }

    /* Switch depening on the current initialization state */
    switch(asyncCartridgeInitState){
        case ASYNC_CARTRIDGE_INIT_SET_WAIT:
            /* Set the state of the cartridge to 'initializing' */
            frontend.cartridge[currentModule].state=CARTRIDGE_INITING;

            /* Setup timer to wait before initializing the cartridge */
            if(startAsyncTimer(TIMER_CARTRIDGE_INIT,
                               TIMER_TO_CARTRIDGE_INIT,
                               FALSE)==ERROR){

                /* Next state: start state */
                asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_SET_WAIT;

                return ERROR;
            }

            /* Set next state */
            asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_WAIT;

            break;

        case ASYNC_CARTRIDGE_INIT_WAIT:
            {
                /* A temporaty variable to deal with the timer */
                int timedOut;

                /* Query the async timer */
                timedOut=queryAsyncTimer(TIMER_CARTRIDGE_INIT);

                if(timedOut==ERROR){
                    /* Next state: start state */
                    asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_SET_WAIT;

                    return ERROR;
                }

                /* Wait until timer expires. No need to clear the timer because
                   it is done by the queryAsyncTimer function if expired. */
                if(timedOut==TIMER_EXPIRED){
                    /* Set next state */
                    asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_INIT;
                }

                break;
            }
        case ASYNC_CARTRIDGE_INIT_INIT:
            /* Perform the actual initialization */
            if(cartridgeInit(currentModule)==ERROR){
                /* Set next state */
                asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_INIT;

                return ERROR;
            }

            /* Set the state of the cartridge to 'ready' */
            frontend.cartridge[currentModule].state=CARTRIDGE_READY;

            /* Next state: start state */
            asyncCartridgeInitState=ASYNC_CARTRIDGE_INIT_SET_WAIT;

            return ASYNC_DONE;
            break;

        default:
            return ERROR;
            break;
    }

    return NO_ERROR;
}

// Asynchronously set a cartridge to STANDBY2 mode:
int asyncCartridgeGoStandby2(void) {

    // Check if the cartridge was turned off in the meantime
    if(frontend.cartridge[currentModule].state == CARTRIDGE_OFF) {

        // Cartridge was turned off so nothing else to do
        return ASYNC_DONE;
    }

    // select the bias subsystem for all the following calls:
    currentCartridgeSubsystem = CARTRIDGE_SUBSYSTEM_BIAS;

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND0;
    lnaGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND1;
    lnaGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND0;
    lnaGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND1;
    lnaGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND0;
    sisGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND1;
    sisGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND0;
    sisGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND1;
    sisGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND0;
    sisMagnetGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    currentPolarizationModule = SIDEBAND1;
    sisMagnetGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND0;
    sisMagnetGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    currentPolarizationModule = SIDEBAND1;
    sisMagnetGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION0;
    lnaLedGoStandby2();

    LATCH_DEBUG_SERIAL_WRITE = 1;

    currentBiasModule = POLARIZATION1;
    lnaLedGoStandby2();

    // Set the state of the cartridge to READY:
    frontend.cartridge[currentModule].state=CARTRIDGE_READY;
            
    return ASYNC_DONE;
}
