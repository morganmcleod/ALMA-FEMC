/*! \file   polDac.c
    \brief  Polarization DAC functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: polDac.c,v 1.12 2007/06/22 22:03:00 avaccari Exp $

    This files contains all the functions necessary to handle the polarization
    DAC events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "biasSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentPolDacModule=0;
/* Statics */
static HANDLER  polDacModulesHandler[POL_DAC_MODULES_NUMBER]={resetStrobeHandler,
                                                              clearStrobeHandler};

/* Polarization DAC handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the polarization DAC. */
void polDacHandler(void){

    #ifdef DEBUG
        printf("      Polarization DAC: %d\n",
               currentPolSpecialMsgsModule);
    #endif /* DEBUG */

    /* No check is necessary on the existance of the hardware since, if the
       check on the polarization passed, the DACs are automatically installed. */


    /* Check if the submodule is in range */
    currentPolDacModule=(CAN_ADDRESS&POL_DAC_MODULES_RCA_MASK)>>POL_DAC_MODULES_MASK_SHIFT;
    if(currentPolDacModule>=POL_DAC_MODULES_NUMBER){
        storeError(ERR_POL_DAC,
                   0x01); // Error 0x01 -> Polarization DAC submodule out of range

        CAN_STATUS = HARDW_RNG_ERR;

        return;
    }
    /* Call the correct handler */
    (polDacModulesHandler[currentPolDacModule])();
}

/* Reset Strobe Handler */
static void resetStrobeHandler(void){

    #ifdef DEBUG
        printf("       Reset Strobe\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   polSpecialMsgs.
                    polDac[currentPolSpecialMsgsModule].
                     lastResetStrobe,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           polSpecialMsgs.
            polDac[currentPolSpecialMsgsModule].
             lastResetStrobe.
              status=NO_ERROR;

        /* Send the strobe */
        if(setBiasDacStrobe()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               polSpecialMsgs.
                polDac[currentPolSpecialMsgsModule].
                 lastResetStrobe.
                  status=ERROR;

            return;
        }

    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   polSpecialMsgs.
                    polDac[currentPolSpecialMsgsModule].
                     lastResetStrobe,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;

    }

    /* If monitor on monitor RCA: this should never happen because there are
       no monitor available for this particular device. In case though, return
       a CAN range error to avoid timeouts. */
    storeError(ERR_POL_DAC,
               0x03); // Error 0x03 -> Monitor RCA out of range
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;

}

/* Clear Strobe Handler */
static void clearStrobeHandler(void){

    #ifdef DEBUG
        printf("       Clear Strobe\n");
    #endif /* DEBUG */

    /* Check if the submodule is in range: Only DAC1 is able to receive a clear
       strobe. */
    if(currentPolSpecialMsgsModule!=POL_DAC_ALLOW_CLEAR_STROBE){
        storeError(ERR_POL_DAC,
                   0x02); // Error 0x02 -> Polarization DAC doesn't accept clear strobe

        CAN_STATUS = HARDW_RNG_ERR;

        return;
    }

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   polSpecialMsgs.
                    polDac[currentPolSpecialMsgsModule].
                     lastClearStrobe,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);
        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           polSpecialMsgs.
            polDac[currentPolSpecialMsgsModule].
             lastClearStrobe.
              status=NO_ERROR;


        /* Send the strobe */
        if(setBiasDacStrobe()==ERROR){
            /* Store the ERROR state in the last control message variable. */
            frontend.
             cartridge[currentModule].
              polarization[currentBiasModule].
               polSpecialMsgs.
                polDac[currentPolSpecialMsgsModule].
                 lastClearStrobe.
                  status=ERROR;

            return;
        }

    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   polSpecialMsgs.
                    polDac[currentPolSpecialMsgsModule].
                     lastClearStrobe,
               CAN_LAST_CONTROL_MESSAGE_SIZE);
    }

    /* If monitor on monitor RCA: this should never happen because there are
       no monitor available for this particular device. In case though, return
       a CAN range error to avoid timeouts. */
    storeError(ERR_POL_DAC,
               0x03); // Error 0x03 -> Monitor RCA out of range
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;
}
