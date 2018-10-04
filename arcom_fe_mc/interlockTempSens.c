/*! \file   interlockTempSens.c
    \brief  FETIM Interlock Temperature Sensors

    <b> File informations: </b><br>
    Created: 2011/03/29 17:34:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockTempSens.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This file contains all the functions necessary to handle FETIM interlock
    temperature sensor events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "debug.h"
#include "fetimSerialInterface.h"
#include "error.h"

/* Globals */
unsigned char   currentInterlockTempSensModule=0;
/* Statics */
static HANDLER interlockTempSensModulesHandler[INTERLOCK_TEMP_SENS_MODULES_NUMBER]={tempHandler};

/* Interlock Temperature Sensor Handler */
/*! This function will be called by the CAN message handler when the received
    message is in the address range of the interlock temperature sensor */
void interlockTempSensHandler(void){
    #ifdef DEBUG_FETIM
    printf("     Temperature Sensor: %d\n",
           currentInterlockTempModule);
    #endif /* DEBUG_FETIM */

    /* Since there is only one submodule in the interlock temperature sensor
       module, the check to see if the desired submodule is in range, is not
       needed and we can directly call the correct handler. */
    /* Call the correct handler */
    (interlockTempSensModulesHandler[currentInterlockTempSensModule])();

    return;

}

/* Tmperature sensor handler */
void tempHandler(void){
    #ifdef DEBUG_FETIM
        printf("      Temperature\n");
    #endif /* DEBUG_FETIM */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_INTRLK_TEMP,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control messages
       allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_INTRLK_TEMP,
                   0x03); // Error 0x03 -> Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }


    /* Monitor Interlock temperature */
    if(getInterlockTemp()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      sensors.
                       temperature.
                        intrlkTempSens[currentInterlockTempModule].
                         temp[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now this
           function is only printing out a warning/error message depending on
           the result but no actions are taken. */
    } else {
        /* If no error during monitor process, gather the stored data/ */
        CONV_FLOAT=frontend.
                    fetim.
                     interlock.
                      sensors.
                       temperature.
                        intrlkTempSens[currentInterlockTempModule].
                         temp[CURRENT_VALUE];
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big enadian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
