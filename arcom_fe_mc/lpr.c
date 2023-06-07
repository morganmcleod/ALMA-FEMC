/*! \file   lpr.c
    \brief  LPR functions

    <b> File information: </b><br>
    Created: 2007/06/02 17:01:27 by avaccari

    This file contains all the functions necessary to handle LPR events. */

/* Includes */
#include <stdio.h>      /* printf */
#include <string.h>     /* strcpy */

#include "debug.h"
#include "error.h"
#include "frontend.h"
#include "timer.h"
#include "lprSerialInterface.h"
#include "serialInterface.h"
#include "can.h"
#include "iniWrapper.h"


/* Globals */
/* Externs */
unsigned char   currentLprModule=0;
/* Statics */
static HANDLER  lprModulesHandler[LPR_MODULES_NUMBER]={lprTempHandler,
                                                       lprTempHandler,
                                                       opticalSwitchHandler,
                                                       edfaHandler};
static int lprCloseShutter();

/* LPR handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the LPR. */
void lprHandler(void){
    #ifdef DEBUG_LPR
        printf(" LPR\n");
    #endif /* DEBUG_LPR */

    /* Since the receiver is always outfitted with a LPR, ho hadrware check is
       performed. */

    /* Check if the submodule is in range */
    currentLprModule=(CAN_ADDRESS&LPR_MODULES_RCA_MASK)>>LPR_MODULES_MASK_SHIFT;
    if(currentLprModule>=LPR_MODULES_NUMBER){
        storeError(ERR_LPR, ERC_MODULE_RANGE); //LPR submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Call the correct handler */
    (lprModulesHandler[currentLprModule])();
}

/* LPR Startup */
/*! This function performs the operations necessary to initialize the LPR. This
    are performed only once at startup.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int lprStartup(void) {

    /* Parse the FRONTEND.INI file to extract the name of the configuration
       file for the LPR. */
    #ifdef DEBUG_STARTUP
        printf(" LPR configuration file: ");
    #endif

    strcpy(frontend.lpr.configFile, "LPR.INI");
    strcpy(frontend.lpr2.configFile, "LPR.INI");

    #ifdef DEBUG_STARTUP
        /* Start the configuration */
        printf(" Initializing LPR...\n");

        /* Load the calibrated coefficient for the EDFA photodetector power
           redout. */
        printf("  - Loading EDFA photo detector power coefficient...\n");
    #endif
    
    // No longer loading from INI file:
    frontend.lpr.edfa.photoDetector.coeff = LPR_ADC_EDFA_PD_POWER_COEFF_DFLT;
    frontend.lpr2.edfa.photoDetector.coeff = LPR_ADC_EDFA_PD_POWER_COEFF_DFLT;

    #ifdef DEBUG_STARTUP
        /* Print coefficient */
        printf("     a = %f\n",
               frontend.lpr.edfa.photoDetector.coeff);
        printf("    done!\n"); // EDFA photo detector coefficient
    #endif

    /* Set the currentModule variable to reflect the fact that the LPR is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=LPR_MODULE;

    #ifdef DEBUG_STARTUP
        /* Setup 10 MHz communication */
        printf("  - Setting up 10MHz communication...\n");
    #endif

    if(serialAccess(LPR_10MHZ_MODE,
                NULL,
                LPR_10MHZ_MODE_SIZE,
                LPR_10MHZ_MODE_SHIFT_SIZE,
                LPR_10MHZ_MODE_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
        return ERROR;
    }
    frontend.lpr.ssi10MHzEnable=ENABLE;

    if (frontend.enableLpr2) {
        currentModule += 1;

        if(serialAccess(LPR_10MHZ_MODE,
            NULL,
            LPR_10MHZ_MODE_SIZE,
            LPR_10MHZ_MODE_SHIFT_SIZE,
            LPR_10MHZ_MODE_SHIFT_DIR,
            SERIAL_WRITE)==ERROR){
            return ERROR;
        }
        frontend.lpr2.ssi10MHzEnable=ENABLE;

        currentModule -= 1;
    }

    #ifdef DEBUG_STARTUP
        printf("    done!\n"); // 10MHz communication

        /* Initialize the optical switch. */
        printf("  - Initializing the optical switch...\n");

        /* Set optical modulation input to 0V. This will ensure that only minimum
           optical radiation escaped the LPR should the switch not be shuttered. */
        printf("    - Setting modulation input value to 0.0V...\n");
    #endif

    /* Load the CAN float to 0.0V */
    CONV_FLOAT=0.0;
    /* Call the setModulationInputValue() function to set the value in
       hardware. If error, return error and abort initialization. */
    if(setModulationInputValue()==ERROR) {
        return ERROR;
    }

    if (frontend.enableLpr2) {
        currentModule += 1;
        if(setModulationInputValue()==ERROR) {
            return ERROR;
        }
        currentModule -= 1;
    }

    #ifdef DEBUG_STARTUP
        printf("      done!\n"); // Modulation input to 0.0V

        /* Shutter the optical switch */
        printf("    - Set optical switch in shutter mode...\n");
    #endif

    if (lprCloseShutter() == ERROR)
        return ERROR;
     
     if (frontend.enableLpr2) {
        currentModule += 1;
        if (lprCloseShutter() == ERROR)
            return ERROR;
        currentModule -= 1;
    }

    /* If everyting went fine, update the optical switch port to reflect
       the fact that the shutter is enabled. */
    frontend.lpr.opticalSwitch.port=PORT_SHUTTERED;
    frontend.lpr2.opticalSwitch.port=PORT_SHUTTERED;

    #ifdef DEBUG_STARTUP
        printf("      done!\n"); // Set shutter
        printf("    done!\n"); // Optical Switch
        printf(" done!\n\n"); // Initialization
    #endif
    return NO_ERROR;
}

static int lprCloseShutter() {
    /* A variable to keep track of the timer */
    int timedOut;

    /* Setup for 5 seconds and start the asynchornous timer */
    if(startAsyncTimer(TIMER_LPR_SWITCH_RDY,
                       TIMER_LPR_TO_SWITCH_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    /* Try standard shutter for 5 sec. The standard shutter waits for the
       optical switch to be ready. */
    do {
        #ifdef DEBUG_STARTUP
            printf("      - Trying standard shutter...\n");
        #endif
        timedOut=queryAsyncTimer(TIMER_LPR_SWITCH_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((setOpticalSwitchShutter(STANDARD)==ERROR)&&(timedOut==TIMER_RUNNING));

    #ifdef DEBUG_STARTUP
        printf("        done!\n"); // Optical switch ready
    #endif

    /* If the timer has expired, signal the error and force the shutter */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_OPTICAL_SWITCH, ERC_HARDWARE_TIMEOUT); //Time out while waiting for ready state during initialization

        /* Force the shutter mode. If error, return error and abort
           initialization. */
        printf(" LPR  - Set optical switch: Forcing shutter mode...\n");
        if(setOpticalSwitchShutter(FORCED)==ERROR){
            return ERROR;
        }
        #ifdef DEBUG_STARTUP
            printf("        done!\n"); // Force shutter
        #endif
    } else {
        /* Stop the timer */
        if(stopAsyncTimer(TIMER_LPR_SWITCH_RDY)==ERROR){
            return ERROR;
        }
    }
    return NO_ERROR;
}


/* LPR Stop */
/*! This function performs the operations necessary to shutdown the LPR. This
    are performed only once at shutdown.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int lprStop(void){

    /* Set the currentModule variable to reflect the fact that the LPR is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=LPR_MODULE;

    #ifdef DEBUG_STARTUP
        printf(" Powering down LPR...\n");

    /* Set optical modulation input to 0V. This will ensure that only minimum
       optical radiation escaped the LPR should the switch not be shuttered. */
    printf("  - Setting modulation input value to 0.0V...\n");
    #endif

    /* Load the CAN float to 0.0V */
    CONV_FLOAT=0.0;
    /* Call the setModulationInputValue() function to set the value in
       hardware. If error, return error and abort initialization. */
    if(setModulationInputValue()==ERROR){
        return ERROR;
    }

    if (frontend.enableLpr2) {
        currentModule += 1;
        if(setModulationInputValue()==ERROR){
            return ERROR;
        }
        currentModule -= 1;
    }

    #ifdef DEBUG_STARTUP
        printf("    done!\n"); // Modulation input to 0.0V

        /* Shutter the optical switch */
        printf("  - Set optical switch in shutter mode...\n");
    #endif

    if (lprCloseShutter() == ERROR)
        return ERROR;
     
    if (frontend.enableLpr2) {
        currentModule += 1;
        if (lprCloseShutter() == ERROR)
            return ERROR;
        currentModule -= 1;
    }

    /* If everyting went fine, update the optical switch port to reflect
       the fact that the shutter is enabled. */
    frontend.lpr.opticalSwitch.port=PORT_SHUTTERED;
    frontend.lpr2.opticalSwitch.port=PORT_SHUTTERED;


    #ifdef DEBUG_STARTUP
        printf("    done!\n"); // Set shutter
        printf(" done!\n\n"); // Optical Switch
    #endif

    return NO_ERROR;
}
