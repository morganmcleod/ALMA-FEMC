/*! \file   lo.c
    \brief  LO functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lo.c,v 1.16 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the functions necessary to handle LO events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "debug.h"
#include "database.h"
#include "serialInterface.h"
#include "loSerialInterface.h"
#include "iniWrapper.h"

/* Globals */
/* Externs */
unsigned char   currentLoModule=0;
/* Statics */
static HANDLER  loModulesHandler[LO_MODULES_NUMBER]={ytoHandler,
                                                     photomixerHandler,
                                                     pllHandler,
                                                     amcHandler,
                                                     paHandler};

/* LO init */
/*! This function performs the operations necessary to initialize a LO at
    runtime.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loInit(void){
    printf(" - Initializing LO...\n");

    /* Set the 10MHz */
    printf("   - 10MHz...\n");

    if(serialAccess(LO_10MHZ_MODE,
                    NULL,
                    LO_10MHZ_MODE_SIZE,
                    LO_10MHZ_MODE_SHIFT_SIZE,
                    LO_10MHZ_MODE_SHIFT_DIR,
                    SERIAL_WRITE)==ERROR){
        return ERROR;
    }
    frontend.
     cartridge[currentModule].
      lo.
       ssi10MHzEnable=ENABLE;

    printf("     done!\n"); // 10MHz

    /* Set correct loop bandwidth. If it is not defined return the undefined
       warning. */
    printf("   - PLL Loop Bandwidth...\n");
    if(frontend.
        cartridge[currentModule].
         lo.
          pll.
           loopBandwidthSelect[DEFAULT_VALUE]==PLL_LOOP_BANDWIDTH_UNDEFINED){
       storeError(ERR_LO,
                  0x03); // Error 0x03 -> Warning: The addressed hardware is not properly defined yet
        return NO_ERROR;
    }

    /* If the value is defined then set it to the default value. */
    setLoopBandwidthSelect(frontend.
                            cartridge[currentModule].
                             lo.
                              pll.
                               loopBandwidthSelect[DEFAULT_VALUE]);

    printf("     done!\n"); // PLL loop bandwidth
    printf("   done!\n"); // LO

    return NO_ERROR;
}


/* LOe startup init */
/*! This function performs the operations necessary to initialize a LO during
    startup. These operations are performed only once during the startup
    sequence.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int loStartup(void){

    /* Few variables to help load the data from the configuration file */
    CFG_STRUCT dataIn;
    unsigned char cnt;

    printf(" LO %d configuration file: %s\n",
           currentModule,
           frontend.
            cartridge[currentModule].
             lo.
              configFile);

    printf(" Initializing LO %d ESN:",
           currentModule);

    /* Get the serial number from the configuration file */
    /* Configure the read array */
    dataIn.
     Name=LO_ESN_KEY;
    dataIn.
     VarType=Cfg_HB_Array;
    dataIn.
     DataPtr=frontend.
              cartridge[currentModule].
               lo.
                serialNumber;

    /* Access configuration file, if error, then skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   lo.
                    configFile,
                 LO_ESN_SECTION,
                 &dataIn,
                 LO_ESN_EXPECTED)==ERROR){
        return NO_ERROR;
    }

    /* Print the serial number */
    for(cnt=0;
        cnt<SERIAL_NUMBER_SIZE;
        cnt++){
        printf(" %x",
               frontend.
                cartridge[currentModule].
                 lo.
                  serialNumber[cnt]);
    }
    printf("...\n"); // Serial number


    /* Load the PLL Loop Bandwidth value in the frontend variable default
       value. */
    printf("  - PLL Loop Bandwidth...\n");

    /* Configure read array */
    dataIn.
     Name=PLL_LOOP_BW_KEY;
    dataIn.
     VarType=Cfg_Byte;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 pll.
                  loopBandwidthSelect[DEFAULT_VALUE];

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   lo.
                    configFile,
                 PLL_LOOP_BW_SECTION,
                 &dataIn,
                 PLL_LOOP_BW_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - PLL loop bandwidth default value %d...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   loopBandwidthSelect[DEFAULT_VALUE]);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL loop bandwidth



    /* Load the PLL lock detect voltage scale factor the frontend variable
       value. */
    printf("  - PLL Lock detect voltage scaling factor...\n");

    /* Configure read array */
    dataIn.
     Name=PLL_LOCK_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 pll.
                  lockDetectVoltageScale;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   lo.
                    configFile,
                 PLL_LOCK_SECTION,
                 &dataIn,
                 PLL_LOCK_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - PLL lock detect voltage scaling factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   lockDetectVoltageScale);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL lock detect voltage scaling factor



    /* Load the PLL correction voltage scale factor the frontend variable
       value. */
    printf("  - PLL Correction voltage scaling factor...\n");

    /* Configure read array */
    dataIn.
     Name=PLL_CORR_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 pll.
                  correctionVoltageScale;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   lo.
                    configFile,
                 PLL_CORR_SECTION,
                 &dataIn,
                 PLL_CORR_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - PLL correction voltage scaling factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   correctionVoltageScale);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL correction voltage scaling factor



    /* Load the PLL YIG current scale factor the frontend variable value. */
    printf("  - PLL YIG Current scaling factor...\n");

    /* Configure read array */
    dataIn.
     Name=PLL_YIG_C_SCALE_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 pll.
                  YIGHeaterCurrentScale;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   lo.
                    configFile,
                 PLL_YIG_C_SCALE_SECTION,
                 &dataIn,
                 PLL_YIG_C_SCALE_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - PLL YIG heater current scaling factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   YIGHeaterCurrentScale);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL YIG heater current scaling factor



    /* Load the PLL YIG current offset factor the frontend variable value. */
    printf("  - PLL YIG Current offset factor...\n");

    /* Configure read array */
    dataIn.
     Name=PLL_YIG_C_OFFSET_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 pll.
                  YIGHeaterCurrentOffset;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
               lo.
                configFile,
             PLL_YIG_C_OFFSET_SECTION,
             &dataIn,
             PLL_YIG_C_OFFSET_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - PLL YIG heater current offset factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  pll.
                   YIGHeaterCurrentOffset);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // PLL YIG heater current offset factor




    /* Load the LO supply voltages scaling factor the frontend variable value. */
    printf("  - LO supply voltages scaling factor...\n");

    /* Configure read array */
    dataIn.
     Name=LO_SUPPLY_V_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 supplyVoltagesScale;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
               lo.
                configFile,
             LO_SUPPLY_V_SECTION,
             &dataIn,
             LO_SUPPLY_V_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - LO supply voltages scaling factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  supplyVoltagesScale);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // LO supply voltages scaling factor



    /* Load the LO multiplier currents scaling factor the frontend variable value. */
    printf("  - LO multiplier currents scaling factor...\n");

    /* Configure read array */
    dataIn.
     Name=LO_MULTIPLIER_C_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&frontend.
               cartridge[currentModule].
                lo.
                 multiplierCurrentsScale;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
               lo.
                configFile,
             LO_MULTIPLIER_C_SECTION,
             &dataIn,
             LO_MULTIPLIER_C_EXPECTED)==ERROR){
        return NO_ERROR;
    }


    #ifdef DEBUG_STARTUP
        printf("    - LO multiplier currents scaling factor %f...done!\n",
               frontend.
                cartridge[currentModule].
                 lo.
                  multiplierCurrentsScale);
    #endif /* DEBUG_STARTUP */

    printf("    done!\n"); // LO multiplier currents scaling factor


    printf(" done!\n\n"); // LO


    return NO_ERROR;

}















/* LO handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the LO. */
void loHandler(void){
    #ifdef DEBUG
        printf("   LO\n");
    #endif /* DEBUG */

    /* It is assumed that if the cartridge is available, then the WCA is
       installed as well. */

    /* Check if the submodule is in range */
    currentLoModule=(CAN_ADDRESS&LO_MODULES_RCA_MASK)>>LO_MODULES_MASK_SHIFT;
    if(currentLoModule>=LO_MODULES_NUMBER){
        storeError(ERR_LO,
                   0x02); // Error 0x02 -> LO submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (loModulesHandler[currentLoModule])();
}

