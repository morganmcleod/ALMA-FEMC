/*! \file   cartridgeTemp.c
    \brief  Cartridge temperature sensors functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle cartridge
    temperature sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "debug.h"
#include "globalDefinitions.h"
#include "biasSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentCartridgeTempModule=0;

/* Statics */
static HANDLER cartridgeTempModulesHandler[CARTRIDGE_TEMP_MODULES_NUMBER] = {tempHandler,
                                                                             tempOffsetHandler};

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
void cartridgeTempHandler(void) {
    #ifdef DEBUG
        printf("    Cartridge Temperature Sensors: %d\n",
               currentCartridgeTempSubsystemModule);
    #endif /* DEBUG */

    /* Check if the specified submodule is in range */
    currentCartridgeTempModule = (CAN_ADDRESS & CARTRIDGE_TEMP_MODULES_RCA_MASK) 
        >> CARTRIDGE_TEMP_MODULES_MASK_SHIFT;
    if(currentCartridgeTempModule >= CARTRIDGE_TEMP_MODULES_NUMBER){
        storeError(ERR_CARTRIDGE_TEMP, ERC_MODULE_RANGE); //Cartridge subsystem out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (cartridgeTempModulesHandler[currentCartridgeTempModule])();
}

/* Temperature Offset Handler */
static void tempOffsetHandler(void) {
    #ifdef DEBUG
        printf("      Temperature Offset\n");
    #endif /* DEBUG */

    /* If control message (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].
            cartridgeTemp[currentCartridgeTempSubsystemModule].lastOffset)

        /* Extract the floating data from the CAN message */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);

        /* Save the new value */
        frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].
            offset = CONV_FLOAT;
        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass == CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].
            cartridgeTemp[currentCartridgeTempSubsystemModule].lastOffset)
        return;
    }

    /* If monitor on monitor RCA */
    /* Return the value from memory */
    CONV_FLOAT=frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].offset;

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). It is done directly instead of using a function to save some time. */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}


/* Temperature Value Handler */
static void tempHandler(void) {

    #ifdef DEBUG
        printf("      Temperature Value\n");
    #endif /* DEBUG */

    /* If control (size!=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_CARTRIDGE_TEMP, ERC_RCA_RANGE);  //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass == CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_CARTRIDGE_TEMP, ERC_RCA_RANGE);  //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    #ifdef DEBUG
        printf("       Sensor %d in Band %d mapped to Polarization %d, Sensor %d\n",
               currentCartridgeTempSubsystemModule,
               currentModule,
               temperatureSensor[currentCartridgeTempSubsystemModule]
                    [currentModule].polarization,
               temperatureSensor[currentCartridgeTempSubsystemModule]
                    [currentModule].sensorNumber);
    #endif /* DEBUG */
    
    /* Monitor the temperature sensor */
    getTemp(temperatureSensor[currentCartridgeTempSubsystemModule]
                [currentModule].polarization,
            temperatureSensor[currentCartridgeTempSubsystemModule]
                [currentModule].sensorNumber);

    /* If error during monitoring, the error state was stored in the outgoing CAN message during by getTemp().
       Whether or not an error occurred, store the last read value in the outgoing message: */
    CONV_FLOAT = frontend.cartridge[currentModule].cartridgeTemp[currentCartridgeTempSubsystemModule].temp;
    
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
