/*! \file   cartridge.c
    \brief  Cartridge functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>

    \$Id: cartridge.c,v 1.45 2009/04/09 02:09:55 avaccari Exp $

    This files contains all the functions necessary to handle cartridge events.

    \todo What should be done if an error is encountered while reading the
          configuration file? */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "debug.h"
#include "database.h"
#include "biasSerialInterface.h"
#include "iniWrapper.h"

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
unsigned char currentCartridgeSubsystem=0;
unsigned char currentLoAndTempModule=0;
unsigned char currentBiasModule=0;
unsigned char currentCartridgeTempSubsystemModule=0;

/* Cartridge handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cartriges. */
void cartridgeHandler(void){
    #ifdef DEBUG
        printf(" Cartridge: %d (currentModule)\n",
               currentModule);
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the receiver is outfitted with the cartridge */
        if(frontend.
            cartridge[currentModule].
             available==UNAVAILABLE){
           storeError(ERR_CARTRIDGE,
                       0x01); // Error 0x01 -> Cartridge not installed

           CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
           return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the cartrdige is powered before allowing monitor and control.
       This check should always be performed because if the cartridge doesn't
       go through the initialization sequence, the CPLDs in the cartridge will
       think that they should operate at 5MHz instead of 10 and the returned
       data will be half of the real data. */
    if(frontend.
        cartridge[currentModule].
         state==CARTRIDGE_OFF){
        storeError(ERR_CARTRIDGE,
                   0x06); // Error 0x06 -> The cartrdige is not powered
        CAN_STATUS = HARDW_BLKD_ERR; // Notify incoming message
        return;
    }

    /* Check if the specified submodule is in range */
    currentCartridgeSubsystem=(CAN_ADDRESS&CARTRIDGE_SUBSYSTEM_RCA_MASK)>>CARTRIDGE_SUBSYSTEM_MASK_SHIFT;
    if(currentCartridgeSubsystem>=CARTRIDGE_SUBSYSTEMS_NUMBER){
        storeError(ERR_CARTRIDGE,
                   0x02); // Error 0x02 -> Cartridge subsystem out of range

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
        storeError(ERR_CARTRIDGE,
                   0x03); // Error 0x03 ->  Lo and cartridge temperature submodule out of range
        return;
    }

    /* Call the correct function */
    (loAndTempModulesHandler[currentLoAndTempModule])();
}

/* Cartrdige temperature sensor handler. */
static void cartridgeTempSubsystemHandler(void){
    #ifdef DEBUG
        printf("   Cartridge Temperature\n");
    #endif /* DEBUG */

    /* No need to check if it is outfitted since this is a fictuos subsystem
       used to redirect CAN messages */

    /* Check if the the specified submodule is in range */
    currentCartridgeTempSubsystemModule=(CAN_ADDRESS&CARTRIDGE_TEMP_SUBSYSTEM_MODULES_RCA_MASK)>>CARTRIDGE_TEMP_SUBSYSTEM_MODULES_MASK_SHIFT;
    if(currentCartridgeTempSubsystemModule>=CARTRIDGE_TEMP_SUBSYSTEM_MODULES_NUMBER){
        storeError(ERR_CARTRIDGE,
                   0x04); // Error 0x04 ->  Cartridge temperature submodule out of range
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
        storeError(ERR_CARTRIDGE,
                   0x05); // Error 0x05 ->  BIAS submodule out of range
        return;
    }


    /* Call the correct function */
    (biasModulesHandler[currentBiasModule])();
}


/* Cartrdige stop */
/*! This function performs the operations necessary to shut down a cartidge.
    \param cartrdige    This is the selected cartrdige to power off
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cartridgeStop(unsigned char cartridge){

    #ifdef DEBUG_INIT
        printf("- Shutting down cartridge %d...\n",
               cartridge);
    #endif  // DEBUG_INIT

    /* Change the state of the addressed cartridge to OFF */
    frontend.
     cartridge[cartridge].
      state = CARTRIDGE_OFF;

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
    unsigned char sensor, cnt;


    /* A variable to hold the section names of the cartridge configuration file
       where the temperature sensors offsets can be found. */
    char sensors[CARTRIDGE_TEMP_SENSORS_NUMBER+1][SENSOR_SEC_NAME_SIZE]={"4K_STAGE",
                                                                         "110K_STAGE",
                                                                         "POL0_MIXER",
                                                                         "SPARE",
                                                                         "15K_STAGE",
                                                                         "POL1_MIXER"};


    printf(" Cartridge %d configuration file: %s\n",
           currentModule+1,
           frontend.
            cartridge[currentModule].
             configFile);

    printf(" Initializing Cartridge %d ESN:",
           currentModule+1);

    /* Get the serial number from the configuration file */
    /* Configure the read array */
    dataIn.
     Name=CARTRIDGE_ESN_KEY;
    dataIn.
     VarType=Cfg_HB_Array;
    dataIn.
     DataPtr=frontend.
              cartridge[currentModule].
               serialNumber;

    /* Access configuration file, if error, then skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   configFile,
                 CARTRIDGE_ESN_SECTION,
                 &dataIn,
                 CARTRIDGE_ESN_EXPECTED)!=NO_ERROR){
        return NO_ERROR;
    }

    /* Print the serial number */
    for(cnt=0;
        cnt<SERIAL_NUMBER_SIZE;
        cnt++){
        printf(" %x",
               frontend.
                cartridge[currentModule].
                 serialNumber[cnt]);
    }
    printf("...\n"); // Serial number

    /* Load the hardware availability information for the selected cartridge. */
    printf("  - Hardware availability...\n");

    /* Polarization Level */
    for(currentBiasModule=0;
        currentBiasModule<POLARIZATIONS_NUMBER;
        currentBiasModule++){

        /* Polarization availability */
        /* Configure the read array for polarization availability info */
        dataIn.
         Name=POL_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     available;

        /* Access configuration file, if error, then skip the configuration. */
        if(myReadCfg(frontend.
                      cartridge[currentModule].
                       configFile,
                     POL_AVAIL_SECT(currentBiasModule),
                     &dataIn,
                     POL_AVAIL_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }
        #ifdef DEBUG_STARTUP
            /* Print the availability info */
            printf("    - Polarization %d available: %d\n",
                   currentBiasModule,
                   frontend.
                    cartridge[currentModule].
                     polarization[currentBiasModule].
                      available);
        #endif /* DEBUG_STARTUP */


        /* Sideband Level */
        for(currentPolarizationModule=0;
            currentPolarizationModule<SIDEBANDS_NUMBER;
            currentPolarizationModule++){

            /* Sideband availability */
            /* Configure the read array for sideband availability info */
            dataIn.
             Name=SB_AVAIL_KEY;
            dataIn.
             VarType=Cfg_Boolean;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        polarization[currentBiasModule].
                         sideband[currentPolarizationModule].
                          available;

            /* Access configuration file, if error, then skip the configuration. */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           configFile,
                         SB_AVAIL_SECT(currentBiasModule,
                                       currentPolarizationModule),
                         &dataIn,
                         SB_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            #ifdef DEBUG_STARTUP
                /* Print the availability info */
                printf("      - Sideband %d available: %d\n",
                       currentPolarizationModule,
                       frontend.
                        cartridge[currentModule].
                         polarization[currentBiasModule].
                          sideband[currentPolarizationModule].
                           available);
            #endif /* DEBUG_STARTUP */

            /* SIS availability */
            /* Configure the read array for SIS availability info */
            dataIn.
             Name=SIS_AVAIL_KEY;
            dataIn.
             VarType=Cfg_Boolean;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        polarization[currentBiasModule].
                         sideband[currentPolarizationModule].
                          sis.
                           available;

            /* Access configuration file, if error, then skip the configuration. */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           configFile,
                         SIS_AVAIL_SECT(currentBiasModule,
                                        currentPolarizationModule),
                         &dataIn,
                         SIS_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            #ifdef DEBUG_STARTUP
                /* Print the availability info */
                printf("        - SIS available: %d\n",
                       frontend.
                        cartridge[currentModule].
                         polarization[currentBiasModule].
                          sideband[currentPolarizationModule].
                           sis.
                            available);
            #endif /* DEBUG_STARTUP */

            /* SIS magnet availability */
            /* Configure the read array for SIS magnet availability info */
            dataIn.
             Name=SIS_MAG_AVAIL_KEY;
            dataIn.
             VarType=Cfg_Boolean;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        polarization[currentBiasModule].
                         sideband[currentPolarizationModule].
                          sisMagnet.
                           available;

            /* Access configuration file, if error, then skip the configuration. */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           configFile,
                         SIS_MAG_AVAIL_SECT(currentBiasModule,
                                            currentPolarizationModule),
                         &dataIn,
                         SIS_MAG_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            #ifdef DEBUG_STARTUP
                /* Print the availability info */
                printf("        - SIS magnet available: %d\n",
                       frontend.
                        cartridge[currentModule].
                         polarization[currentBiasModule].
                          sideband[currentPolarizationModule].
                           sisMagnet.
                            available);
            #endif /* DEBUG_STARTUP */


            /* LNA availability */
            /* Configure the read array for LNA availability info */
            dataIn.
             Name=LNA_AVAIL_KEY;
            dataIn.
             VarType=Cfg_Boolean;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        polarization[currentBiasModule].
                         sideband[currentPolarizationModule].
                          lna.
                           available;

            /* Access configuration file, if error, then skip the configuration. */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           configFile,
                         LNA_AVAIL_SECT(currentBiasModule,
                                        currentPolarizationModule),
                         &dataIn,
                         LNA_AVAIL_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            #ifdef DEBUG_STARTUP
                /* Print the availability info */
                printf("        - LNA available: %d\n",
                       frontend.
                        cartridge[currentModule].
                         polarization[currentBiasModule].
                          sideband[currentPolarizationModule].
                           lna.
                            available);
            #endif /* DEBUG_STARTUP */


            /* LNA STAGE Level */
            for(currentLnaModule=0;
                currentLnaModule<LNA_STAGES_NUMBER;
                currentLnaModule++){

                /* LNA STAGE availability */
                /* Configure the read array for LNA STAGE availability info */
                dataIn.
                 Name=LNA_STAGE_AVAIL_KEY;
                dataIn.
                 VarType=Cfg_Boolean;
                dataIn.
                 DataPtr=&frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sideband[currentPolarizationModule].
                              lna.
                               stage[currentLnaModule].
                                available;

                /* Access configuration file, if error, then skip the configuration. */
                if(myReadCfg(frontend.
                              cartridge[currentModule].
                               configFile,
                             LNA_STAGE_AVAIL_SECT(currentBiasModule,
                                                  currentPolarizationModule,
                                                  currentLnaModule),
                             &dataIn,
                             LNA_STAGE_AVAIL_EXPECTED)!=NO_ERROR){
                    return NO_ERROR;
                }

                #ifdef DEBUG_STARTUP
                    /* Print the availability info */
                    printf("        - LNA STAGE %d available: %d\n",
                           currentLnaModule,
                           frontend.
                            cartridge[currentModule].
                             polarization[currentBiasModule].
                              sideband[currentPolarizationModule].
                               lna.
                                stage[currentLnaModule].
                                 available);
                #endif /* DEBUG_STARTUP */
            }
        }


        /* LNA LED availability */
        /* Configure the read array for LNA LED availability info */
        dataIn.
         Name=LNA_LED_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     lnaLed.
                      available;

        /* Access configuration file, if error, then skip the configuration. */
        if(myReadCfg(frontend.
                      cartridge[currentModule].
                       configFile,
                     LNA_LED_AVAIL_SECT(currentBiasModule),
                     &dataIn,
                     LNA_LED_AVAIL_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        #ifdef DEBUG_STARTUP
            /* Print the availability info */
            printf("      - LNA LED available: %d\n",
                   frontend.
                    cartridge[currentModule].
                     polarization[currentBiasModule].
                      lnaLed.
                       available);
        #endif /* DEBUG_STARTUP */



        /* SIS heater availability */
        /* Configure the read array for SIS heater availability info */
        dataIn.
         Name=SIS_HEATER_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sisHeater.
                      available;

        /* Access configuration file, if error, then skip the configuration. */
        if(myReadCfg(frontend.
                      cartridge[currentModule].
                       configFile,
                     SIS_HEATER_AVAIL_SECT(currentBiasModule),
                     &dataIn,
                     SIS_HEATER_AVAIL_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        #ifdef DEBUG_STARTUP
            /* Print the availability info */
            printf("      - SIS heater available: %d\n",
                   frontend.
                    cartridge[currentModule].
                     polarization[currentBiasModule].
                      sisHeater.
                       available);
        #endif /* DEBUG_STARTUP */


        /* Schottky mixer availability */
        /* Configure the read array for Schottky mixer availability info */
        dataIn.
         Name=SCHOTTKY_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     schottkyMixer.
                      available;

        /* Access configuration file, if error, then skip the configuration. */
        if(myReadCfg(frontend.
                      cartridge[currentModule].
                       configFile,
                     SCHOTTKY_AVAIL_SECT(currentBiasModule),
                     &dataIn,
                     SCHOTTKY_AVAIL_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        #ifdef DEBUG_STARTUP
            /* Print the availability info */
            printf("      - Schottky mixer available: %d\n",
                   frontend.
                    cartridge[currentModule].
                     polarization[currentBiasModule].
                      schottkyMixer.
                       available);
        #endif /* DEBUG_STARTUP */
    }
    printf("    done!\n"); // Hardware availability


    /* Load the SIS current sense resistor value. This assumes that all the
       sidebands in all the polarizations have the same sense resistor. If this
       is not the case then the software and the configuration files will have
       to be update to allow for different values for different sidebands and
       polarizations.
       Also the availability of polarization and sideband should be checked
       before this step. The check is skipped because no harm is done if the
       value of the resistor is set for a not installed*/
    printf("  - Sense resistor...\n");

    /* Configure read array */
    dataIn.
     Name=RESISTOR_VALUE_KEY;
    dataIn.
     VarType=Cfg_Float;
    dataIn.
     DataPtr=&resistor;

    /* Access configuration file, if error, skip the configuration. */
    if(myReadCfg(frontend.
                  cartridge[currentModule].
                   configFile,
                 RESISTOR_VALUE_SECTION,
                 &dataIn,
                 RESISTOR_VALUE_EXPECTED)!=NO_ERROR){
        return NO_ERROR;
    }

    /* Store the value in all polarizations sidebands. */
    for(currentBiasModule=0;
        currentBiasModule<POLARIZATIONS_NUMBER;
        currentBiasModule++){
        for(currentPolarizationModule=0;
            currentPolarizationModule<SIDEBANDS_NUMBER;
            currentPolarizationModule++){

            /* Store in the frontend variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sideband[currentPolarizationModule].
                sis.
                 resistor=resistor;

            #ifdef DEBUG_STARTUP
                printf("    - Polarization %d Sideband %d (%f ohm)...done!\n",
                       currentBiasModule,
                       currentPolarizationModule,
                       frontend.
                        cartridge[currentModule].
                         polarization[currentBiasModule].
                          sideband[currentPolarizationModule].
                           sis.
                            resistor);
            #endif /* DEBUG_STARTUP */
        }
    }

    printf("    done!\n"); // Sense resistor

    /* Load temperature sensors offsets. */
    printf("  - Temperature sensors offsets...\n");

    for(sensor=0;
        sensor<CARTRIDGE_TEMP_SENSORS_NUMBER;
        sensor++){

        /* Load the configuration for the available sensors */
        dataIn.
         Name=SENSOR_AVAIL_KEY;
        dataIn.
         VarType=Cfg_Boolean;
        dataIn.
         DataPtr=&frontend.
                   cartridge[currentModule].
                    cartridgeTemp[sensor].
                     available;

        /* Access configuration file, if error, skip the configuration. */
        if(myReadCfg(frontend.
                      cartridge[currentModule].
                       configFile,
                     SENSOR_AVAIL_SECTION(sensor),
                     &dataIn,
                     SENSOR_AVAIL_EXPECTED)!=NO_ERROR){
            return NO_ERROR;
        }

        /* Check if the sensor is available */
        if(frontend.
            cartridge[currentModule].
             cartridgeTemp[sensor].
              available==AVAILABLE){

            /* If available, red the offset and store it in the frontend
               variable. */
            dataIn.
             Name=SENSOR_OFFSET_KEY;
            dataIn.
             VarType=Cfg_Float;
            dataIn.
             DataPtr=&frontend.
                       cartridge[currentModule].
                        cartridgeTemp[sensor].
                         offset;

            /* Access configuration file, if error, skip the configuration. */
            if(myReadCfg(frontend.
                          cartridge[currentModule].
                           configFile,
                         SENSOR_OFFSET_SECTION(sensor),
                         &dataIn,
                         SENSOR_OFFSET_EXPECTED)!=NO_ERROR){
                return NO_ERROR;
            }

            #ifdef DEBUG_STARTUP
                printf("    - Sensor %d [%s] (offset: %f)...done!\n",
                       sensor,
                       SENSOR_OFFSET_SECTION(sensor),
                       frontend.
                        cartridge[currentModule].
                         cartridgeTemp[sensor].
                          offset);
            #endif /* DEBUG_STARTUP */
        }
    }

    printf("    done!\n"); // Temperature sensor offsets








    printf(" done!\n\n"); // Cartridge

    return NO_ERROR;
}

/* Cartrige init */
/*! This function performs the operations necessary to initialize a cartrdige at
    runtime. These are executed everytime a cartridge is powered up.
    \param cartridge    This is the selected cartrdige to initialize
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int cartridgeInit(unsigned char cartridge){

    /* Set currentModule variable to reflect the selected cartrdige.
       This is necessary because currentModule is the global variable pointing
       to the selected cartridge. */
    currentModule = cartridge;

    /* Check if the receiver is outfitted with the selected cartridge */
    #ifdef DATABASE_HARDW
        /* Check if the receiver is outfitted with the cartridge */
        if(frontend.
            cartridge[currentModule].
             available==UNAVAILABLE){
            storeError(ERR_CARTRIDGE,
                       0x01); // Error 0x01 -> Cartridge not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
            return ERROR;
        }
    #endif /* DATABASE_HARDW */

    #ifdef DEBUG_INIT
        printf("Initializing cartrdige (%d)\n",
                currentModule+1);
    #endif // DEBUG_INIT

    /* Enable the 10MHz communication for the BIAS and the LO and update the
       relative state variable. Initialize both polarization even if there might
       be only one. This is a control serial message, there will be no timeout
       even if one of the polarization is not installed. Same for the LO, if it
       is not installed, it doesn't matter. */

    /* Polarizations */
    /* Set current cartrdige subsystem to bias */
    currentCartridgeSubsystem=CARTRIDGE_SUBSYSTEM_BIAS;
    /* Set the 10MHz */
    for(currentBiasModule=0;
        currentBiasModule<POLARIZATIONS_NUMBER;
        currentBiasModule++){

        /* Initialize the polarizations */
        if(polarizationInit()==ERROR){
            return ERROR;
        }
    }

    /* LO */
    /* Set current cartrdige subsystem to LO */
    currentCartridgeSubsystem=CARTRIDGE_SUBSYSTEM_LO;

    /* Initialize the LO */
    if(loInit()==ERROR){
        return ERROR;
    }


    /* Change the state of the addressed cartridge to ON */
    #ifdef DEBUG_INIT
        printf(" - Turning cartridge ON...");
    #endif // DEBUG_INIT

    frontend.
     cartridge[currentModule].
      state = CARTRIDGE_ON;

    #ifdef DEBUG_INIT
        printf("done!\n"); // Turning cartrdige on

        printf("done!\n\n"); // Cartridge
    #endif // DEBUG_INIT

    return NO_ERROR;

}

