/*! \file   frontend.c
    \brief  Frontend functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: frontend.c,v 1.23 2011/11/28 22:10:49 avaccari Exp $

    This file contains the functions and the informations necessary to deal with
    the frontend system. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "iniWrapper.h"
#include "database.h"


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
/* At some point calculate the FRONTEND.INI CRC and store it in the frontend
   variable. */

/* Deal with the ESNs and the exchange of data with configuration database.
   We assume that all of this happened and the configuration files are stored
   on the drive. */

/* During the frontend init an exchange of info should happen with the
   configuration database resulting in the file frontend.ini being stored in
   the drive together with a bunch of other ini files. The frontend.ini file
   will contain information on available hardware and it will be used to proceed
   with the initialization once a "go" command is received by the control
   software.... yeah right! */

/* Somehow the initialization should stop at this point waiting to a "go" from
   the control software. */

/* At this point everything in the frontend init went fine and we have an
   unpdated set of configuration files on the drive and we have received the
   "go" and we can proceed with the initialization. (The frontend.ini file
   has been parsed during the frontend initialization) */


    #ifdef DATABASE_HARDW
        /* Few variables to help load the data from the configuration file */
        CFG_STRUCT dataIn;

        printf("Initializing frontend...\n\n");

        /* Search for available cartridges and initialize them */
        for(currentModule=0;
            currentModule<CARTRIDGES_NUMBER;
            currentModule++){

            /* Load the configuration for the available cartridges */
            dataIn.
             Name=BAND_AVAIL_KEY;
            dataIn.
             VarType=Cfg_Boolean;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        available;

            /* Access configuration file, if error, return skip the configuration. */
            if(myReadCfg(FRONTEND_CONF_FILE,
                         BAND_AVAIL_SECTION(currentModule),
                         &dataIn,
                         BAND_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            /* Check if the cartridge is available */
            if(frontend.
                cartridge[currentModule].
                 available==AVAILABLE){

                /* If available, read the name of the configuration file for the
                   cartridge store it in the frontend variable. */
                dataIn.
                 Name=CART_FILE_KEY;
                dataIn.
                 VarType=Cfg_String;
                dataIn.
                 DataPtr=frontend.
                          cartridge[currentModule].
                           configFile;

                /* Access configuration file, if error, return skip the
                   configuration. */
                if(myReadCfg(FRONTEND_CONF_FILE,
                             CART_FILE_SECTION(currentModule),
                             &dataIn,
                             CART_FILE_EXPECTED)!=NO_ERROR){
                    return NO_ERROR;
                }

                /* If available, read the name of the configuration file for the
                   cartridge store it in the frontend variable. */
                dataIn.
                 Name=WCA_FILE_KEY;
                dataIn.
                 VarType=Cfg_String;
                dataIn.
                 DataPtr=frontend.
                          cartridge[currentModule].
                           lo.
                            configFile;

                /* Access configuration file, if error, return skip the
                   configuration. */
                if(myReadCfg(FRONTEND_CONF_FILE,
                             WCA_FILE_SECTION(currentModule),
                             &dataIn,
                             WCA_FILE_EXPECTED)!=NO_ERROR){
                    return NO_ERROR;
                }

                /* Perform cartrdige startup configuration */
                if(cartridgeStartup()==ERROR){
                    return ERROR;
                }

                /* Perform LO startup configuration */
                if(loStartup()==ERROR){
                    return ERROR;
                }
            }
        }
    #else // If the hardware configuration database is not available
        printf("Initializing frontend...\n");
    #endif /* DATABASE_HARDW */

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

    /* Switch to operational mode */
    frontend.
     mode[CURRENT_VALUE] = OPERATIONAL_MODE;

    printf("done!\n\n");

    return NO_ERROR;
}

