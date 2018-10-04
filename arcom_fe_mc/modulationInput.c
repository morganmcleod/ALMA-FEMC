/*! \file   modulationInput.c
    \brief  EDFA modulation input functions

    <b> File informations: </b><br>
    Created:  2007/06/02 10:29:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: modulationInput.c,v 1.6 2009/08/25 21:39:39 avaccari Exp $

    This file contains all the functions necessary to handle the EDFA modulation
    input port. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "lprSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
unsigned char   currentModulationInputModule=0;
/* Statics */
static HANDLER  modulationInputModulesHandler[MODULATION_INPUT_MODULES_NUMBER]={valueHandler,
                                                                                miSpecialMsgsHandler};

/* EDFA modulation input handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the EDFA modulation input. */
void modulationInputHandler(void){

    #ifdef DEBUG_LPR
        printf("   Modulation Input\n");
    #endif /* DEBUG_LPR */

    /* Since the LPR is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentModulationInputModule=(CAN_ADDRESS&MODULATION_INPUT_MODULES_RCA_MASK)>>MODULATION_INPUT_MODULES_MASK_SHIFT;
    if(currentModulationInputModule>=MODULATION_INPUT_MODULES_NUMBER){
        storeError(ERR_MODULATION_INPUT,
                   0x02); // Error 0x02 -> Modulation Input submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error

        return;
    }

    /* Call the correct handler */
    (modulationInputModulesHandler[currentModulationInputModule])();
}


/* EDFA modulation input value handler */
static void valueHandler(void){
    #ifdef DEBUG
        printf("    Value\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   lpr.
                                    edfa.
                                     modulationInput.
                                      lastValue)

        /* Extract the float from the can message. */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        /* Set the lna stage. If an error occurs then store the state and
           then return. */
        if(setModulationInputValue()==ERROR){
            /* Store the ERROR state in the last control message varibale */
            frontend.
             lpr.
              edfa.
               modulationInput.
                lastValue.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     lpr.
                                      edfa.
                                       modulationInput.
                                        lastValue)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware read-back value but just
       the current status that is stored in memory. The memory status is
       updated when a new modulation input value is sent with a control
       message. */
    /* Extract the float from the last CAN message data. */
    changeEndian(CONV_CHR_ADD,
                 frontend.
                  lpr.
                   edfa.
                    modulationInput.
                     lastValue.
                      data);
    /* Copy the last issued message to the current value */
    frontend.
     lpr.
      edfa.
       modulationInput.
        value[CURRENT_VALUE]=CONV_FLOAT;

    CONV_FLOAT=frontend.
               lpr.
                edfa.
                 modulationInput.
                  value[CURRENT_VALUE];

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

