/*! \file   photomixer.c
    \brief  1st LO Photomixer functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle the 1st LO
    photomixer events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "loSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentPhotomixerModule=0;
/* Statics */
static HANDLER  photomixerModulesHandler[PHOTOMIXER_MODULES_NUMBER]={enableHandler,
                                                                     voltageHandler,
                                                                     currentHandler};

/* 1st LO photomixer handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the 1st LO photomixer. */
void photomixerHandler(void){

    #ifdef DEBUG
        printf("    Photomixer\n");
    #endif /* DEBUG */

    /* Since the LO is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentPhotomixerModule=(CAN_ADDRESS&PHOTOMIXER_MODULES_RCA_MASK)>>PHOTOMIXER_MODULES_MASK_SHIFT;
    if(currentPhotomixerModule>=PHOTOMIXER_MODULES_NUMBER){
        storeError(ERR_PHOTOMIXER, ERC_MODULE_RANGE); //Photomixer submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error

        return;
    }
    /* Call the correct handler */
    (photomixerModulesHandler[currentPhotomixerModule])();
}

/* Enable Handler */
static void enableHandler(void){

    #ifdef DEBUG
        printf("     Enable\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    lo.
                                     photomixer.
                                      lastEnable)

        /* Change the status of the photomixer according to the content of the
           CAN message. */
        if(setPhotomixerEnable(CAN_BYTE?PHOTOMIXER_ENABLE:
                                        PHOTOMIXER_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               photomixer.
                lastEnable.
                 status=ERROR;

            return;
        }

        /* If everyting went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      lo.
                                       photomixer.
                                        lastEnable)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                photomixer.
                 enable;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}


/* Voltage Handler */
/* This function deals with all the monitor requests diected to the photomixer
   voltage. There are no control messages allowed for the photomixer voltage. */
static void voltageHandler(void){

    #ifdef DEBUG
        printf("     Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PHOTOMIXER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTOMIXER, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the photomixer voltage */
    if(getPhotomixer(PHOTOMIXER_BIAS_V)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     photomixer.
                      voltage;
    } else {
        /* If no error during the monitor process, gather the stored data. */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     photomixer.
                      voltage;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). This affect the first 4 char of the message, the status
       byte is left unaffected. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Current Handler */
/* This function deals with all the monitor requests directed to the photomixer
   current. There are no control messages allowed for the photomixer current. */
static void currentHandler(void){

    #ifdef DEBUG
        printf("     Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PHOTOMIXER, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_PHOTOMIXER, ERC_RCA_RANGE); //Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;
        return;
    }

    /* Monitor the photomixer current */
    /** This is a fix to enable a stable reading of the photomixer current.
        once (if ever) the hardware is going to be able to return a stable
        reading within the allocated time, this should be removed. For this
        fix, the first readout will not check for errors. **/
    getPhotomixer(PHOTOMIXER_BIAS_C);
    /** End Fix **/

    if(getPhotomixer(PHOTOMIXER_BIAS_C)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     photomixer.
                      current;
    } else {
        /* If no error during the monitor process, gather the stored data. */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     photomixer.
                      current;
    }
    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). This affect the first 4 char of the message, the status
       byte is left unaffected. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}
