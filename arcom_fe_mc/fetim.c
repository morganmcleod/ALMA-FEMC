/*! \file   fetim.c
    \brief  FETIM functions

    <b> File informations: </b><br>
    Created: 2011/03/25 17:01:27 by avaccari

    <b> CVS informations: </b><br>
    \$Id: fetim.c,v 1.2 2011/11/09 00:40:30 avaccari Exp $

    This file contains all the functions necessary to handle FETIM events. */

/* Includes */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* exit */

#include "frontend.h"
#include "debug.h"
#include "error.h"
#include "fetimSerialInterface.h"
#include "async.h"
#include "globalOperations.h"



/* Globals */
/* Externs */
unsigned char currentFetimModule=0;
unsigned char currentAsyncFetimExtTempModule=0; /*! < This global keeps track of
                                                      the FETIM extarnal
                                                      temperature module
                                                      currently addressed by the
                                                      asynchronous routine. */
int asyncFetimExtTempError[COMP_TEMP_SENSORS_NUMBER]; /*!< A global to keep
                                                           track of the async
                                                           error while
                                                           monitoring fetim ext
                                                           temperatures */


/* Statics */
static HANDLER fetimModulesHandler[FETIM_MODULES_NUMBER]={interlockHandler,
                                                          compressorHandler,
                                                          dewarHandler};

/* FETIM Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the FETIM. */
void fetimHandler(void){

    #ifdef DEBUG_FETIM
    printf(" FETIM: %d (currentModule)\n",
           currentModule);
    #endif /* DEBUG_FETIM */

    /* Check if the receiver is outfitted with the FETIM system */
    if(frontend.
        fetim.
         available==UNAVAILABLE){
        storeError(ERR_FETIM,
                   0x02); // Error 0x02 -> FETIM not installed
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Check if the specified submodule is in range */
    currentFetimModule=(CAN_ADDRESS&FETIM_MODULES_RCA_MASK)>>FETIM_MODULES_MASK_SHIFT;
    if(currentFetimModule>=FETIM_MODULES_NUMBER){
        storeError(ERR_FETIM,
                   0x01); // Error 0x01 -> Submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct function */
    (fetimModulesHandler[currentFetimModule])();

    return;
}






/* FETIM initialization */
/*! This function performs all the necessary initialization for the FETIM
    subsystem. These are executed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something went wrong */
int fetimStartup(void){

    /* Set the currentModule variable to reflect the fact that the FETIM is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=FETIM_MODULE;

    printf(" Initializing FETIM Module...\n");
    printf("  - Reading FETIM module hardware revision level...\n");

    /* Call the getIfSwitchHadrwRevision() function to read the hardware
       revision level and depening on the returned value evaluates the presence
       of the FETIM hardware. If error, return error and abort
       initialization. */
    if(getFetimHardwRevision()==ERROR){
        return ERROR;
    }

    /* If the FETIM is not installed notify the user */
    if(frontend.
        fetim.
         available==UNAVAILABLE){
        printf("     FETIM module not installed!\n");
    } else {
        printf("     Revision level: %d\n",
               frontend.
                fetim.
                 hardwRevision);
    }

    printf("    done!\n"); // Hardware Revision Level

    /* Set warning and error ranges for known sensors. */
    printf("  - Setting warning and error ranges of known sensors...\n");
    /* External sensor 2 connected monitors turbo pump temperature */
    frontend.
     fetim.
      compressor.
       temp[COMP_TEMP_SENSOR_TURBO].
        temp[LOW_ERROR_RANGE]=TURBO_PUMP_MIN_TEMPERATURE;
    frontend.
     fetim.
      compressor.
       temp[COMP_TEMP_SENSOR_TURBO].
        temp[LOW_WARNING_RANGE]=TURBO_PUMP_MIN_WARN_TEMP;
    frontend.
     fetim.
      compressor.
       temp[COMP_TEMP_SENSOR_TURBO].
        temp[HI_WARNING_RANGE]=TURBO_PUMP_MAX_WARN_TEMP;
    frontend.
     fetim.
      compressor.
       temp[COMP_TEMP_SENSOR_TURBO].
        temp[HI_ERROR_RANGE]=TURBO_PUMP_MAX_TEMPERATURE;
    printf("    done!\n"); // Warning and error ranges

    printf(" done!\n\n"); // Initialization

    return NO_ERROR;
}

/* FETIM async */
/*! This function deals with the asynchronous operation in the FETIM:
        - Monitor the external temperature sensors (this are going to be used
          by the turbo pump code to allow/disallow operation of the hardware)
        - Gracefully shutdown the Front End if the ultimate shutdown sequence
          has been started
        - Generate and communicate to the FETIM the FE state bit
        - ...
    \return
        - \ref NO_ERROR     -> if no error occured
        - \ref ASYNC_DONE   -> once all the async operations are done
        - \ref ERROR        -> if something went wrong */
int fetimAsync(void){

    /* A static enum to track the state of the async function */
    static enum {
        ASYNC_FETIM_GET_EXT_TEMP,
        ASYNC_FETIM_SET_FE_STATUS,
        ASYNC_FETIM_SHUTDOWN_FE
    } asyncFetimState = ASYNC_FETIM_GET_EXT_TEMP;

    /* If the FETIM is not installed, return */
    if(frontend.
        fetim.
         available==UNAVAILABLE){
        return ASYNC_DONE;
    }

    /* Address the FETIM */
    currentModule=FETIM_MODULE;

    /* Switch to the correct state */
    switch(asyncFetimState){
        /* Monitor the external temperature asynchronously */
        case ASYNC_FETIM_GET_EXT_TEMP:
            #ifdef DEBUG_FETIM_ASYNC
                printf("Async -> FETIM -> Ext Temp%d\n",currentAsyncFetimExtTempModule);
            #endif /* DEBUG_FETIM_ASYNC */

            /* Get the external temperatures */
            asyncFetimExtTempError[currentAsyncFetimExtTempModule]=getCompressorTemp();

            /* If done or error, go next sensor */
            switch(asyncFetimExtTempError[currentAsyncFetimExtTempModule]){
                case NO_ERROR:
                    return NO_ERROR;
                    break;
                case ASYNC_DONE:
                    asyncFetimExtTempError[currentAsyncFetimExtTempModule]=NO_ERROR;
                    break;
                case ERROR:
                    break;
                default:
                    break;
            }

            /* Next sensor, if wrap around, then next monitor next thing */
            if(++currentAsyncFetimExtTempModule==COMP_TEMP_SENSORS_NUMBER){
                currentAsyncFetimExtTempModule-=COMP_TEMP_SENSORS_NUMBER;
                asyncFetimState = ASYNC_FETIM_SET_FE_STATUS;
            }

            break;

        /* Set the FE status bit and send to the FETIM */
        case ASYNC_FETIM_SET_FE_STATUS:
            {
                float tempFloat;
                unsigned char newState;
                static unsigned char currentState=FE_STATUS_UNSAFE;

                #ifdef DEBUG_FETIM_ASYNC
                    printf("Async -> FETIM -> FE Status\n");
                #endif /* DEBUG_FETIM_ASYNC */

                /* Check current conditions */
                tempFloat=frontend.
                           cryostat.
                            vacuumController.
                             vacuumSensor[CRYOSTAT_PRESSURE].
                              pressure[CURRENT_VALUE];

                /* Set state accoding to current condition */
                if((tempFloat==FLOAT_ERROR)||(tempFloat==FLOAT_UNINIT)||(tempFloat>MAX_CRYO_COOLING_PRESSURE)){
                    newState=FE_STATUS_UNSAFE;
                } else {
                    newState=FE_STATUS_SAFE;
                }

                /* If the state has not changed, then skip. */
                if(newState==currentState){
                    asyncFetimState=ASYNC_FETIM_SHUTDOWN_FE;
                    break;
                }

                /* If it has changed then set FETIM FE status bit accordingly */
                setFeSafeStatus(newState);

                /* Update current state */
                currentState=newState;

                /* Set next state */
                asyncFetimState=ASYNC_FETIM_SHUTDOWN_FE;

                break;
            }

        /* Check if ultimate shutdown sequence has been triggered and if so,
           gracefully shut down the Front End */
        case ASYNC_FETIM_SHUTDOWN_FE:

            #ifdef DEBUG_FETIM_ASYNC
                printf("Async -> FETIM -> FE Shutdown\n");
            #endif /* DEBUG_FETIM_ASYNC */

            if(frontend.
                fetim.
                 interlock.
                  state.
                   shutdownTrig[CURRENT_VALUE]==TRUE){

                /* Try to shutdown the FE if error brutally exit */
                if(shutDown()==ERROR){
                    printf("Front End firmware exiting now due to FETIM shutdown\n");
                    exit(NO_ERROR);
                }
            }

           /* Set next state */
           asyncFetimState=ASYNC_FETIM_GET_EXT_TEMP;

           return ASYNC_DONE;

           break;

        default:
            return ERROR;
            break;
    }

    return NO_ERROR;

}

