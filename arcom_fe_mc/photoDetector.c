/*! \file   photoDetector.c
    \brief  EDFA photo detector functions

    <b> File Informations: </b><br>
    Created: 2007/06/02 11:38:40 by avaccari

    This file contains all the functions necessary to handler the EDFA photo
    detector. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "lprSerialInterface.h"

/* Globals */
/* Externs */
unsigned char   currentPhotoDetectorModule=0;
static HANDLER  photoDetectorModulesHandler[PHOTO_DETECTOR_MODULES_NUMBER]={currentHandler,
                                                                            conversionCoeffHandler,
                                                                            powerHandler};
static LPR *currentLPR;

/* Photo Detector handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the EDFA photo detector. */
void photoDetectorHandler(void){
    #ifdef DEBUG_LPR
        printf("   Photo Detector\n");
    #endif /* DEBUG_LPR */

    /* Since the EDFA is always outfitted with the photo detector, no hardware
       check is performed. */

    /* Check if the specified submodule is in range */
    currentPhotoDetectorModule = (CAN_ADDRESS & PHOTO_DETECTOR_MODULES_RCA_MASK);
    if (currentPhotoDetectorModule >= PHOTO_DETECTOR_MODULES_NUMBER) {
        storeError(ERR_PHOTO_DETECTOR, ERC_MODULE_RANGE); //EDFA photo detector submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* select which LPR data structure to use */
    currentLPR = (currentModule == LPR2_MODULE) ? &frontend.lpr2 : &frontend.lpr;
    
    /* Call the correct handler */
    (photoDetectorModulesHandler[currentPhotoDetectorModule])();
}

/* EDFA photo detector current handler */
static void currentHandler(void){

    #ifdef DEBUG_LPR
        printf("    Current\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PHOTO_DETECTOR, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTO_DETECTOR, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the Photo Detector current */
    if(getPhotoDetectorCurrent()==ERROR){
        /* If error during monitoring, store th ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=currentLPR->edfa.photoDetector.current;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=currentLPR->edfa.photoDetector.current;
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
    return;
}

/* EDFA photo detector conversion coefficient handler */
void conversionCoeffHandler(void) {
    #ifdef DEBUG_LPR
        printf("    Conversion Coeff\n");
    #endif /* DEBUG_LPR */

    /* If control (size !=0) */
    if(CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(currentLPR->edfa.photoDetector.lastCoeff)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD, CAN_DATA_ADD);

        // Save the new coefficient and return:
        currentLPR->edfa.photoDetector.coeff = CONV_FLOAT;
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(currentLPR->edfa.photoDetector.lastCoeff)
        return;
    }
    
    /* If monitor on a monitor RCA */
    CONV_FLOAT = currentLPR->edfa.photoDetector.coeff;
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* EDFA photo detector power handler */
static void powerHandler(void){
    #ifdef DEBUG_LPR
        printf("    Power\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PHOTO_DETECTOR, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTO_DETECTOR, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the Photo Detector current */
    if(getPhotoDetectorPower()==ERROR){
        /* If error during monitoring, store th ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=currentLPR->edfa.photoDetector.power;
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=currentLPR->edfa.photoDetector.power;
    }
    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD, CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
    return;
}
