/*! \file   laser.c
    \brief  EDFA laser

    <b> File information: </b><br>
    Created: 2007/06/02 12:15:18 by avaccari

    This file contains all the function necessary to handler the EDFA laser
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "can.h"
#include "frontend.h"
#include "lprSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentLaserModule=0;
/* Statics */
static HANDLER  laserModulesHandler[LASER_MODULES_NUMBER]={pumpTempHandler,
                                                           driveCurrentHandler,
                                                           photoDetectCurrentHandler};

/* EDFA Laser handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the EDFA laser. */
void laserHandler(void){
    #ifdef DEBUG_LPR
        printf("   Laser\n");
    #endif /* DEBUG_LPR */

    /* Since the EDFA is always outfitted with the laser, no hardware check is
       performed. */

    /* Check if the specified submodule is in range */
    currentLaserModule=(CAN_ADDRESS&LASER_MODULES_RCA_MASK);
    if(currentLaserModule>=LASER_MODULES_NUMBER){
        storeError(ERR_LASER, ERC_MODULE_RANGE); //EDFA laser submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (laserModulesHandler[currentLaserModule])();
}

/* EDFA laser pump temperature */
static void pumpTempHandler(void){
    #ifdef DEBUG_LPR
        printf("    Pump Temperature\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_LASER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LASER, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the EDFA laser pump temperature */
    if((CAN_STATUS=getLaserPumpTemperature())!=NO_ERROR){
        /* If error during monitoring, the error state was stored in the
           outgoing CAN message state during the previous statement. This
           different format is used because getCryostatTemp might return
           two different error state depending on error conditions. */
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     laser.
                      pumpTemp;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     laser.
                      pumpTemp;
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
    return;
}


/* EDFA laser drive current */
static void driveCurrentHandler(void){

    #ifdef DEBUG_LPR
        printf("    Drive current\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_LASER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LASER, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */\
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the EDFA laser drive current */
    if(getLaserDriveCurrent()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT = frontend.
                     lpr.
                      edfa.
                       laser.
                        driveCurrent;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     laser.
                      driveCurrent;
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}

/* EDFA laser photo detector current */
static void photoDetectCurrentHandler(void){
    #ifdef DEBUG_LPR
        printf("    Photo Detector Current\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_LASER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_LASER, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the EDFA laser photo detector current */
    if(getLaserPhotoDetectCurrent()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     laser.
                      photoDetectCurrent;
    } else {
        /* If no error during monitor process gather the staored data */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     laser.
                      photoDetectCurrent;
    }

    /* Load the CAN message payload with th returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}
