/*! \file   cartridgeTemp.c
    \brief  Cartridge temperature sensors functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cartridgeTemp.c,v 1.18 2008/02/28 22:15:05 avaccari Exp $

    This files contains all the functions necessary to handle cartridge
    temperature sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "debug.h"
#include "database.h"
#include "globalDefinitions.h"
#include "biasSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentCartridgeTempModule=0;
/* Statics */
static HANDLER cartridgeTempModulesHandler[CARTRIDGE_TEMP_MODULES_NUMBER]={tempHandler};
/* A static variable to assign the sensors to the proper hardware */
static TEMP_SENSOR temperatureSensor[CARTRIDGE_TEMP_SENSORS_NUMBER]
                                    [CARTRIDGES_NUMBER]=
//    BAND1   BAND2   BAND3   BAND4   BAND5   BAND6   BAND7   BAND8   BAND9   BAND10
    {{{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0},{P0,S0}},     // 4K Stage
     {{P0,S1},{P0,S1},{P1,S0},{P0,S1},{P0,S1},{P0,S1},{P0,S1},{P0,S1},{P0,S1},{P0,S1}},     // 110K Stage
     {{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2},{P0,S2}},     // Mixer Pol0
     {{P1,S0},{P1,S0},{P0,S1},{P1,S0},{P1,S0},{P1,S0},{P1,S0},{P1,S0},{P1,S0},{P1,S0}},     // Spare
     {{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1},{P1,S1}},     // 15K Stage
     {{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2},{P1,S2}}};    // Mixer Pol1

/* Cartridge Temperature sensors handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the cartriges temperature sensors. */
void cartridgeTempHandler(void){

    /* The value of currentCartridgeTempModule is not changed since there is
       only one submodule in the cartridge temp module.
       This structure is preserved only for consistency.
       If the timing should be an issue, it can be removed and the functionality
       can be placed directly in the cartridgeTempHandler function. */

    #ifdef DEBUG
        printf("    Cartridge Temperature Sensors: %d\n",
               currentCartridgeTempSubsystemModule);
    #endif /* DEBUG */


    #ifdef DATABASE_HARDW
        /* Check if the receiver is outfitted with the sensor */
        if(frontend.
            cartridge[currentModule].
             cartridgeTemp[currentCartridgeTempSubsystemModule].
              available==UNAVAILABLE){
            storeError(ERR_CARTRIDGE_TEMP,
                       0x01); // Error 0x01 -> Temperature sensor not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Since there is only one submodule in the cartrdige temperature sensor,
       the check to see if the desired submodule is in range, is not needed and
       we can directly call the correct handler. */
    /* Call the correct handler */
    (cartridgeTempModulesHandler[currentCartridgeTempModule])();
}

/* Temperature Value Handler */
static void tempHandler(void){

    #ifdef DEBUG
        printf("      Temperature Value\n");
    #endif /* DEBUG */

    /* If control (size!=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_CARTRIDGE_TEMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_CARTRIDGE_TEMP,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the temperature sensor */
    #ifdef DEBUG
        printf("       Sensor %d in Band %d mapped to Sensor %d in Polarization %d\n",
               currentCartridgeTempSubsystemModule,
               currentModule,
               temperatureSensor[currentCartridgeTempSubsystemModule]
                                [currentModule].
                sensorNumber,
               temperatureSensor[currentCartridgeTempSubsystemModule]
                                [currentModule].
                polarization);
    #endif /* DEBUG */
    if(getTemp(temperatureSensor[currentCartridgeTempSubsystemModule]
                                [currentModule].
                polarization,
               temperatureSensor[currentCartridgeTempSubsystemModule]
                                [currentModule].
                sensorNumber)==ERROR){
        /* If error during monitoring, the error state was stored in the
           outgoing CAN message state during the previous statement. This
           different format is used because getTemp might return
           two different error state depending on error conditions. */
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    cartridgeTemp[currentCartridgeTempSubsystemModule].
                     temp[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    cartridgeTemp[currentCartridgeTempSubsystemModule].
                     temp[CURRENT_VALUE];

        /* Check the results against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            cartridgeTemp[currentCartridgeTempSubsystemModule].
                             temp[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            cartridgeTemp[currentCartridgeTempSubsystemModule].
                             temp[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                cartridgeTemp[currentCartridgeTempSubsystemModule].
                                 temp[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                cartridgeTemp[currentCartridgeTempSubsystemModule].
                                 temp[HI_ERROR_RANGE])){
                    storeError(ERR_CARTRIDGE_TEMP,
                               0x04); // Error 0x04 -> Error: Cartrdige temperature in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_CARTRIDGE_TEMP,
                               0x05); // Error 0x05 -> Warning: Cartrdige temperature in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}


