/*! \file   frontend.c
    \brief  Frontend functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    This file contains the functions and the information necessary to deal with
    the frontend system. */

/* Includes */
#include <stdio.h>      /* printf */
#include <string.h>     /* memset */

#include "frontend.h"
#include "error.h"
#include "iniWrapper.h"
#include "debug.h"

#include "sockets/include/compiler.h"
#include "sockets/include/capi.h"

/* Globals */
/* Externs */
FRONTEND    frontend;   /*!< This variable contains the current status of
                             the entire frontend system. */


/* Stop the frontend */
/*! This function takes care of shutting down the frontend.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int frontendStop(void){

    /* Shut down power distribution system */
    if(powerDistributionStop()==ERROR){
        return ERROR;
    }

    /* Shut down the LO subsystems */
    if(loShutdown()==ERROR){
        return ERROR;
    }

    /* Shut down the LPR */
    if(lprStop()==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}


/* Frontend Init */
/*! This function performs the operations necessry to initialize the frontend.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int frontendInit(void){

    #ifdef CHECK_HW_AVAIL
        CFG_STRUCT dataIn;
    #endif

    #ifdef DEBUG_INIT
        printf("Initializing frontend...\n\n");
    #endif

    // load the ipaddress from SOCKETS:
    frontendInitIPAddress();

    #ifndef CHECK_HW_AVAIL

        /* Cartridge availability and INI filenames */
        for(currentModule = 0;
            currentModule < CARTRIDGES_NUMBER;
            currentModule++)
        {   
            frontend.cartridge[currentModule].available = TRUE;
            sprintf(frontend.cartridge[currentModule].configFile, "CART%d.INI", currentModule + 1);
            sprintf(frontend.cartridge[currentModule].lo.configFile, "WCA%d.INI", currentModule + 1);
        }

    #else

        /* Search for available cartridges and initialize them */
        for(currentModule = 0;
            currentModule < CARTRIDGES_NUMBER;
            currentModule++)
        {

            /* Load the configuration for the available cartridges */
            dataIn.Name=BAND_AVAIL_KEY;
            dataIn.VarType=Cfg_Boolean;
            dataIn.DataPtr=&frontend.cartridge[currentModule].available;

            /* Access configuration file, if error, return skip the configuration. */
            if(myReadCfg(FRONTEND_CONF_FILE,
                         BAND_AVAIL_SECTION(currentModule),
                         &dataIn,
                         BAND_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            if (frontend.cartridge[currentModule].available) {
                /* If available, read the name of the CCA.INI file. */
                dataIn.Name=CART_FILE_KEY;
                dataIn.VarType=Cfg_String;
                dataIn.DataPtr=frontend.cartridge[currentModule].configFile;

                /* Access configuration file, if error, return skip the
                   configuration. */
                if(myReadCfg(FRONTEND_CONF_FILE,
                             CART_FILE_SECTION(currentModule),
                             &dataIn,
                             CART_FILE_EXPECTED)!=NO_ERROR){
                    return NO_ERROR;
                }

                /* If available, read the name of the WCA.INI file. */
                dataIn.Name=WCA_FILE_KEY;
                dataIn.VarType=Cfg_String;
                dataIn.DataPtr=frontend.cartridge[currentModule].lo.configFile;

                /* Access configuration file, if error, return skip the
                   configuration. */
                if(myReadCfg(FRONTEND_CONF_FILE,
                             WCA_FILE_SECTION(currentModule),
                             &dataIn,
                             WCA_FILE_EXPECTED)!=NO_ERROR){
                    return NO_ERROR;
                }
            }
        }

    #endif // CHECK_HW_AVAIL

    /* Perform CCA and LO startup */
    for(currentModule = 0;
        currentModule < CARTRIDGES_NUMBER;
        currentModule++)
    {
        if(frontend.cartridge[currentModule].available) {

            /* Perform cartridge startup configuration */
            if(cartridgeStartup()==ERROR){
                return ERROR;
            }

            /* Perform LO startup configuration */
            if(loStartup()==ERROR){
                return ERROR;
            }
        }
    }

    /* Initialize the LPR */
    if(lprStartup()==ERROR){
        return ERROR;
    }

    /* Initialize the cryostat system */
    if(cryostatStartup()==ERROR){
        return ERROR;
    }

    /* Initialize the power distribution system */
    if(powerDistributionStartup()==ERROR){
        return ERROR;
    }

    /* Initialize the IF switch system */
    if(ifSwitchStartup()==ERROR){
        return ERROR;
    }

    /* Initialize the FETIM system */
    if(fetimStartup()==ERROR){
        return ERROR;
    }

    #ifdef DEBUG_INIT
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}

int frontendInitIPAddress(void) {
    int ret;
    unsigned uSize;
    uSize = 4;
    // set the IP address to all zero:
    memset(frontend.ipaddress, 0, uSize);

    // get the IP addres from the SOCKET API
    ret = GetKernelInformation(0, K_INF_IP_ADDR, 0, frontend.ipaddress, &uSize);

    printf("IP Address: %d.%d.%d.%d\n", frontend.ipaddress[0], frontend.ipaddress[1], frontend.ipaddress[2], frontend.ipaddress[3]);
    return NO_ERROR;
}

int frontendWriteNVMemory(void) {
    char buf[20];
    #ifdef DEBUG_CRYOSTAT_ASYNC
        printf("frontend -> frontendWriteNVMemory\n");
    #endif /* DEBUG_CRYOSTAT_ASYNC */

    if (frontend.cryostat.coldHeadHoursDirty != 0) {
        sprintf(buf, "%lu", frontend.cryostat.coldHeadHours);
        // Write the current number of hours back to the cryostat hours log file:
        UpdateCfg(frontend.cryostat.coldHeadHoursFile,
             CRYO_HOURS_FILE_SECTION,
             CRYO_HOURS_KEY,
             buf);
        frontend.cryostat.coldHeadHoursDirty = 0;
        #ifdef DEBUG_CRYOSTAT_ASYNC
            printf("frontend -> frontendWriteNVMemory wrote %s hours\n", buf);
        #endif /* DEBUG_CRYOSTAT_ASYNC */
    }
    return NO_ERROR;
}

int feAndCartridgesConfigurationReport(void) {
    unsigned char band;
    printf("FE and cartridges configuration report:\n");
    printf("FE Mode:%d\n", frontend.mode);
    printf("IP Address:%d.%d.%d.%d\n", 
            frontend.ipaddress[0], frontend.ipaddress[1], frontend.ipaddress[2], frontend.ipaddress[3]);
    printf("Cryostat: available:%d hardwRevision:%d\n",
            frontend.cryostat.available,
            frontend.cryostat.hardwRevision);
    printf("IF Switch: hardwRevision:%d bandSelect:%d\n", 
            frontend.ifSwitch.hardwRevision,
            frontend.ifSwitch.bandSelect);
    printf("LPR: bandSelect:%d\n",
            frontend.lpr.opticalSwitch.port);
    printf("FETIM: available:%d hardwRevision:%d\n",
            frontend.fetim.available,
            frontend.fetim.hardwRevision);
    printf("Bands powered:%d Standby2:%d\n", 
            frontend.powerDistribution.poweredModules,
            frontend.powerDistribution.standby2Modules);

    for(band = 0; band < CARTRIDGES_NUMBER; band++) {   
        printf("Band%d: available:%d files:%s %s resistor:%.1f powered:%d\n", 
            band + 1, 
            frontend.cartridge[band].available,
            frontend.cartridge[band].configFile,
            frontend.cartridge[band].lo.configFile,
            frontend.cartridge[band].polarization[0].sideband[0].sis.resistor,
            frontend.powerDistribution.pdModule[band].enable);

        if (frontend.cartridge[band].available) {
            printf(" Pol0 sis1:%d sis2:%d sisMag1:%d sisMag2:%d sisHeat:%d\n",
                frontend.cartridge[band].polarization[0].sideband[0].sis.available,
                frontend.cartridge[band].polarization[0].sideband[1].sis.available,
                frontend.cartridge[band].polarization[0].sideband[0].sisMagnet.available,
                frontend.cartridge[band].polarization[0].sideband[1].sisMagnet.available,
                frontend.cartridge[band].polarization[0].sisHeater.available);
            printf(" Pol1 sis1:%d sis2:%d sisMag1:%d sisMag2:%d sisHeat:%d\n",
                frontend.cartridge[band].polarization[1].sideband[0].sis.available,
                frontend.cartridge[band].polarization[1].sideband[1].sis.available,
                frontend.cartridge[band].polarization[1].sideband[0].sisMagnet.available,
                frontend.cartridge[band].polarization[1].sideband[1].sisMagnet.available,
                frontend.cartridge[band].polarization[1].sisHeater.available);
            printf(" Temp offsets: %.3f %.3f %.3f %.3f %.3f %.3f\n",
                frontend.cartridge[band].cartridgeTemp[0].offset,
                frontend.cartridge[band].cartridgeTemp[1].offset,
                frontend.cartridge[band].cartridgeTemp[2].offset,
                frontend.cartridge[band].cartridgeTemp[3].offset,
                frontend.cartridge[band].cartridgeTemp[4].offset,
                frontend.cartridge[band].cartridgeTemp[5].offset);
        }
    }
    printf("\n");
    return NO_ERROR;
}

int loPaLimitsTablesReport(void) {
    unsigned char band;
    printf("\nLO PA_LIMITS tables report:\n");   
    for(band = 0; band < CARTRIDGES_NUMBER; band++) {   
        printPaLimitsTable(band);
    }
    return NO_ERROR;
}
