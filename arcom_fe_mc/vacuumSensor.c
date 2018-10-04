/*! \file   vacuumSensor.c
    \brief  Cryostat vacuum sensors functions

    <b> File informations: </b><br>
    Created: 2007/04/06 14:41:08 by avaccari

    <b> CVS informations: </b><br>
    \$Id: vacuumSensor.c,v 1.11 2009/10/13 15:01:49 avaccari Exp $

    This files contains all the functions necessary to handle cryostat
    vacuum sensors events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "globalDefinitions.h"
#include "error.h"
#include "database.h"
#include "cryostatSerialInterface.h"
#include "can.h"
#include "async.h"

/* Globals */
/* Externs */
unsigned char   currentVacuumSensorModule=0;
/* Statics */
static HANDLER vacuumSensorModulesHandler[VACUUM_SENSOR_MODULES_NUMBER]={pressureHandler};

/* Cryostat vacuum sensors handler */
/*! This function will be called by the CAN message handling subrutine when the
    receivd message is pertinent to the cryostat vacuum sensors. */
void vacuumSensorHandler(void){

    #ifdef DEBUG_CRYOSTAT
    printf("   Vacuum Sensor: %d\n",
           currentVacuumControllerModule);
    #endif /* DEBUG_CRYOSTAT */

    /* Since the cryostat is always outfitted with all the vacuum sensors, no
       hardware check is performed. */

    /* Since there is only one submodulein the cryostat temperature sensor, the
       check to see if the desired submodule is in range is not needed and we
       can directly call the correct handler. */
    /* Call the correct handler */
    (vacuumSensorModulesHandler[currentVacuumSensorModule])();
}

/* Pressure Handler */
static void pressureHandler(void){

    #ifdef DEBUG_CRYOSTAT
        printf("    Pressure Value\n");
    #endif /* DEBUG_CRYOSTAT */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_VACUUM_SENSOR, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_VACUUM_SENSOR, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the vacum sensor pressure */
    if(asyncVacuumControllerError[currentVacuumControllerModule]==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;

        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cryostat.
                    vacuumController.
                     vacuumSensor[currentVacuumControllerModule].
                      pressure[CURRENT_VALUE];
        /* If the error was a conversion error, store the status in the CAN
           message. */
        if(CONV_FLOAT==CRYOSTAT_PRESS_CONV_ERR){
            CAN_STATUS = HARDW_CON_ERR;
        }

    } else {
        /* if no error during monitor process, gather the stored data */
        CONV_FLOAT=frontend.
                   cryostat.
                    vacuumController.
                     vacuumSensor[currentVacuumControllerModule].
                      pressure[CURRENT_VALUE];
    }
    /* If the async monitoring is disabled, notify the monitored message */
    if(asyncState==ASYNC_OFF){
        CAN_STATUS = HARDW_BLKD_ERR;
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
