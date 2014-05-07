/*! \file   lpr.c
    \brief  LPR functions

    <b> File informations: </b><br>
    Created: 2007/06/02 17:01:27 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lpr.c,v 1.15 2009/08/25 21:39:39 avaccari Exp $

    This file contains all the functions necessary to handle LPR events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "error.h"
#include "frontend.h"
#include "timer.h"
#include "lprSerialInterface.h"
#include "serialInterface.h"
#include "can.h"
#include "iniWrapper.h"
#include "database.h"


/* Globals */
/* Externs */
unsigned char   currentLprModule=0;
/* Statics */
static HANDLER  lprModulesHandler[LPR_MODULES_NUMBER]={lprTempHandler,
                                                       lprTempHandler,
                                                       opticalSwitchHandler,
                                                       edfaHandler};

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
        storeError(ERR_LPR,
                   0x01); // Error 0x01 -> LPR submodule out of range

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
int lprStartup(void){

    /* Few variables to help load the coefficient in the frontend table */
    int timedOut;
    #ifdef DATABASE_HARDW
        unsigned char cnt;

        /* Few variables to help load the coefficient in the frontend table */
        CFG_STRUCT  dataIn;

        /* Parse the FRONTEND.INI file to extract the name of the configuration
           file for the LPR. */
        printf(" LPR configuration file: ");

        /* Configure the read array */
        dataIn.
         Name=LPR_CONF_FILE_KEY;
        dataIn.
         VarType=Cfg_String;
        dataIn.
         DataPtr=frontend.
                  lpr.
                   configFile;

        /* Access configuration file, if error, return skip the configuration. */
        if(myReadCfg(FRONTEND_CONF_FILE,
                     LPR_CONF_FILE_SECTION,
                     &dataIn,
                     LPR_CONF_FILE_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print config file */
        printf("%s\n",
               frontend.
                lpr.
                 configFile);


        /* Start the configuration */
        printf(" Initializing LPR ESN:");

        /* Get the serial number from the configuration file */
        /* Configure the read array */
        dataIn.
         Name=LPR_ESN_KEY;
        dataIn.
         VarType=Cfg_HB_Array;
        dataIn.
         DataPtr=frontend.
                  lpr.
                   serialNumber;

        /* Access configuration file, if error, return skip the configuration. */
        if(myReadCfg(frontend.
                      lpr.
                       configFile,
                     LPR_ESN_SECTION,
                     &dataIn,
                     LPR_ESN_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print serial number */
        for(cnt=0;
            cnt<SERIAL_NUMBER_SIZE;
            cnt++){
            printf(" %x",
                   frontend.
                    lpr.
                     serialNumber[cnt]);
        }
        printf("...\n");

        /* Load the calibrated coefficient for the EDFA photodetector power
           redout. */
        printf("  - Loading EDFA photo detector power coefficient...\n");
        /* Configure the read array */
        dataIn.
         Name=POWER_COEFF_KEY;
        dataIn.
         VarType=Cfg_Float;
        dataIn.
         DataPtr=&frontend.
                   lpr.
                    edfa.
                     photoDetector.
                      coeff;

        /* Access configuration file, if error, skip the configuration. */
        if(myReadCfg(frontend.
                      lpr.
                       configFile,
                     POWER_COEFF_SECTION,
                     &dataIn,
                     POWER_COEFF_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Print coefficient */
        printf("     a = %f\n",
               frontend.
                lpr.
                 edfa.
                  photoDetector.
                   coeff);

        printf("    done!\n"); // EDFA photo detector coefficient
    #else // If the hardware configuration database is not available
        printf(" Initializing LPR...\n");
        printf("  - Loading EDFA photo detector power default coefficient...\n");
        frontend.
         lpr.
          edfa.
           photoDetector.
            coeff=POWER_COEFF_DEFAULT;
        printf("     a = %f\n",
               frontend.
                lpr.
                 edfa.
                  photoDetector.
                   coeff);
        printf("    done!\n"); // EDFA photo detector coefficient

    #endif /* DATABASE_HARDW */

    /* Set the currentModule variable to reflect the fact that the LPR is
       selected. This is necessary because currentModule is the global variable
       used to select the communication channel. This is only necessary if
       serial communication have to be implemented. */
    currentModule=LPR_MODULE;

    /* Setup 10 MHz communication */
    printf("  - Setting up 10MHz communication...\n");

    if(serialAccess(LPR_10MHZ_MODE,
                NULL,
                LPR_10MHZ_MODE_SIZE,
                LPR_10MHZ_MODE_SHIFT_SIZE,
                LPR_10MHZ_MODE_SHIFT_DIR,
                SERIAL_WRITE)==ERROR){
        return ERROR;
    }
    frontend.
     lpr.
      ssi10MHzEnable=ENABLE;

    printf("    done!\n"); // 10MHz communication

    /* Initialize the optical switch. */
    printf("  - Initializing the optical switch...\n");


    /* Set optical modulation input to 0V. This will ensure that only minimum
       optical radiation escaped the LPR should the switch not be shuttered. */
    printf("    - Setting modulation input value to 0.0V...\n");
    /* Load the CAN float to 0.0V */
    CONV_FLOAT=0.0;
    /* Call the setModulationInputValue() function to set the value in
       hardware. If error, return error and abort initialization. */
    if(setModulationInputValue()==ERROR){
        return ERROR;
    }
    printf("      done!\n"); // Modulation input to 0.0V


    /* Shutter the optical switch */
    printf("    - Set optical switch in shutter mode...\n");
    /* Setup for 5 seconds and start the asynchornous timer */
    if(startAsyncTimer(TIMER_LPR_SWITCH_RDY,
                       TIMER_LPR_TO_SWITCH_RDY,
                       FALSE)==ERROR){
        return ERROR;
    }

    /* Try standard shutter for 5 sec. The standard shutter waits for the
       optical switch to be ready. */
    do {
        printf("      - Trying standard shutter...\n");
        timedOut=queryAsyncTimer(TIMER_LPR_SWITCH_RDY);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while ((setOpticalSwitchShutter(STANDARD)==ERROR)&&(timedOut==TIMER_RUNNING));

    printf("        done!\n"); // Optical switch ready

    /* If the timer has expired, signal the error and force the shutter */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_OPTICAL_SWITCH,
                   0x09); // Error 0x09 -> Warning: Time out while waiting for ready state during initialization

        /* Force the shutter mode. If error, return error and abort
           initialization. */
        printf("      - Forcing shutter mode...\n");
        if(setOpticalSwitchShutter(FORCED)==ERROR){
            return ERROR;
        }
        printf("        done!\n"); // Force shutter
    }

    /* If everyting went fine, update the optical switch port to reflect
       the fact that the shutter is enabled. */
    frontend.
     lpr.
      opticalSwitch.
       port[CURRENT_VALUE]=PORT_SHUTTERED;

    printf("      done!\n"); // Set shutter
    printf("    done!\n"); // Optical Switch
    printf(" done!\n\n"); // Initialization

    return NO_ERROR;
}

