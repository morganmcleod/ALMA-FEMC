/*! \file   sisHeater.c
    \brief  SIS heater functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sisHeater.c,v 1.16 2009/04/09 02:09:55 avaccari Exp $

    This files contains all the functions necessary to handle SIS heater
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "biasSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentSisHeaterModule=0;
/* Statics */
static HANDLER  sisHeaterModulesHandler[SIS_HEATER_MODULES_NUMBER]={enableHandler,
                                                                    currentHandler};

/* SIS Heater handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the SIS heater. */
void sisHeaterHandler(void){

    #ifdef DEBUG
        printf("     SIS Heater\n");
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected polarization is outfitted with the desired SIS heater */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              sisHeater.
               available==UNAVAILABLE){
            storeError(ERR_SIS_HEATER,
                       0x01); // Error 0x01 -> SIS heater not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Check if the submodule is in range */
    currentSisHeaterModule=(CAN_ADDRESS&SIS_HEATER_MODULES_RCA_MASK)>>SIS_HEATER_MODULES_MASK_SHIFT;
    if(currentSisHeaterModule>=SIS_HEATER_MODULES_NUMBER){
        storeError(ERR_SIS_HEATER,
                   0x02); // Error 0x02 -> SIS heater submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }

    /* Call the correct handler */
    (sisHeaterModulesHandler[currentSisHeaterModule])();
}

/* Heater enable handler */
static void enableHandler(void){

    #ifdef DEBUG
        printf("      Heater Enable\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sisHeater.
                    lastEnable,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           sisHeater.
            lastEnable.
             status=NO_ERROR;

        /* Change the status of the LNA led according to the content of the CAN
           message. */
        if(setSisHeaterEnable(CAN_BYTE?SIS_HEATER_ENABLE:
                                       SIS_HEATER_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               sisHeater.
                lastEnable.
                 status=ERROR;

            return;
        }

        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   sisHeater.
                    lastEnable,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               polarization[currentBiasModule].
                sisHeater.
                 enable[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

/* Heater current handler */
/* This function deals with all the monitor requests directed to the sis heater
   current. There are no control messages allowed for the heater current. */
static void currentHandler(void){

    #ifdef DEBUG
        printf("      Heater Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control messages are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_SIS_HEATER,
                   0x05); // Error 0x05: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on the RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        storeError(ERR_SIS_MAGNET,
                   0x06); // Error 0x09: Monitor message out of range
       /* Store the state in the outgoing CAN message */
       CAN_STATUS = MON_CAN_RNG;

       return;
    }

    /* Monitor the SIS heater current */
    if(getSisHeater()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sisHeater.
                      current[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    polarization[currentBiasModule].
                     sisHeater.
                      current[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sisHeater.
                              current[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            polarization[currentBiasModule].
                             sisHeater.
                              current[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sisHeater.
                                  current[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                polarization[currentBiasModule].
                                 sisHeater.
                                  current[HI_ERROR_RANGE])){
                    storeError(ERR_SIS_HEATER,
                               0x03); // Error 0x03: Error: SIS heater current in error range.
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_SIS_HEATER,
                               0x04); // Error 0x04: Warning: SIS heater current in warning range.
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
