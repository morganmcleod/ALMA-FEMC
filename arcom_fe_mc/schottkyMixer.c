/*! \file   schottkyMixer.c
    \brief  Schottky Mixer attenuator functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: schottkyMixer.c,v 1.12 2006/10/20 20:03:04 avaccari Exp $

    This files contains all the functions necessary to handle Schottky mixer
    events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentSchottkyMixerModule=0;
/* Statics */
static HANDLER schottkyMixerModulesHandler[SCHOTTKY_MIXER_MODULES_NUMBER]={dummyHandler};

/* Schottky mixer handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the Schottky mixer attenuator. */
void schottkyMixerHandler(void){

    /* The value of currentSchottkyMixerModule is not changed since there is
       only one submodule in the SchottkyMixer attenuator module.
       This structure is preserved only for consistency.
       If the timing should be an issue, it can be removed and the functionality
       can be placed directly in the schottkyMixerHandler function. */

    #ifdef DEBUG
        printf("     Schottky Mixer\n");
    #endif /* DEBUG */

    #ifdef DATABASE_HARDW
        /* Check if the selected polarization is outfitted with the desired Schottky
           mixer */
        if(frontend.
            cartridge[currentModule].
             polarization[currentBiasModule].
              schottkyMixer.
               available==UNAVAILABLE){
            storeError(ERR_SCHOTTKY_MIXER,
                       0x01); // Error 0x01 -> Schottky Mixer not installed

            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
            return;
        }
    #endif /* DATABASE_HARDW */

    /* Call the correct handler */
    (schottkyMixerModulesHandler[currentSchottkyMixerModule])();
}

/* Attenuation Value Handler */
static void dummyHandler(void){

    #ifdef DEBUG
        printf("      Dummy\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  polarization[currentBiasModule].
                   schottkyMixer.
                    lastDummy,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          polarization[currentBiasModule].
           schottkyMixer.
            lastDummy.
             status=NO_ERROR;

/********* Do whatever is necessary to control **************/

        return;
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
                   schottkyMixer.
                    lastDummy,
               CAN_LAST_CONTROL_MESSAGE_SIZE);
        return;
    }

    /* If monitor on a monitor RCA */
/********* Do whatever is necessary to monitor *************/
    /* For now return a dummy */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               polarization[currentBiasModule].
                schottkyMixer.
                 dummy;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

