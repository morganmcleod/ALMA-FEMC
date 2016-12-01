/*! \file   photoDetector.c
    \brief  EDFA photo detector functions

    <b> File Informations: </b><br>
    Created: 2007/06/02 11:38:40 by avaccari

    <b> CVS informations: </b><br>
    \$Id: photoDetector.c,v 1.4 2009/08/25 21:39:39 avaccari Exp $

    This files contains all the functions necessary to handler the EDFA photo
    detector. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "lprSerialInterface.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentPhotoDetectorModule=0;
static HANDLER  photoDetectorModulesHandler[PHOTO_DETECTOR_MODULES_NUMBER]={currentHandler,
                                                                            conversionCoeffHandler,
                                                                            powerHandler};

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
        storeError(ERR_PHOTO_DETECTOR,
                   0x01); // Error 0x01 -> EDFA photo detector submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

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
        storeError(ERR_PHOTO_DETECTOR,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTO_DETECTOR,
                   0x03); // Error 0x03 -> Monitor message out of range
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
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     photoDetector.
                      current[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     photoDetector.
                      current[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           lpr.
                            edfa.
                             photoDetector.
                              current[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           lpr.
                            edfa.
                             photoDetector.
                              current[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               lpr.
                                edfa.
                                 photoDetector.
                                  current[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               lpr.
                                edfa.
                                 photoDetector.
                                  current[HI_ERROR_RANGE])){
                    storeError(ERR_PHOTO_DETECTOR,
                               0x04); // Error 0x04 -> Error: EDFA photodetector current in error range
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PHOTO_DETECTOR,
                               0x05); // Error 0x05 -> Warning: EDFA photodetector current in warning range
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
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
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   lpr.
                                    edfa.
                                     photoDetector.
                                      lastCoeff)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        // Save the new coefficient and return:
        frontend.
         lpr.
          edfa.
           photoDetector.coeff = CONV_FLOAT;

        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                   lpr.
                                    edfa.
                                     photoDetector.
                                      lastCoeff)
        return;
    }
    
    /* If monitor on a monitor RCA */
    storeError(ERR_LPR,
               0x02); // Error 0x02: Monitor message out of range
    
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;
}

/* EDFA photo detector power handler */
static void powerHandler(void){
    #ifdef DEBUG_LPR
        printf("    Power\n");
    #endif /* DEBUG_LPR */

    /* If control (size!=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PHOTO_DETECTOR,
                   0x02); // Error 0x02 -> Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTO_DETECTOR,
                   0x03); // Error 0x03 -> Monitor message out of range
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
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     photoDetector.
                      power[CURRENT_VALUE];
    } else {
        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   lpr.
                    edfa.
                     photoDetector.
                      power[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           lpr.
                            edfa.
                             photoDetector.
                              power[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           lpr.
                            edfa.
                             photoDetector.
                              power[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               lpr.
                                edfa.
                                 photoDetector.
                                  power[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               lpr.
                                edfa.
                                 photoDetector.
                                  power[HI_ERROR_RANGE])){
                    storeError(ERR_PHOTO_DETECTOR,
                               0x06); // Error 0x06 -> Error: EDFA photodetector power in error range
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PHOTO_DETECTOR,
                               0x07); // Error 0x07 -> Warning: EDFA photodetector power in warning range
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size.
       The value has to be converted from little endian (Intel) to big endian
       (CAN). */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

    return;
}
