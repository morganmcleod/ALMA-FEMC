/*! \file   miDac.c
    \brief  Modulation Input DAC functions

    <b> File informations: </b><br>
    Created: 2007/06/21 17:01:45 by avaccari

    <b> CVS informations: </b><br>
    \$Id: miDac.c,v 1.1 2007/06/22 22:03:00 avaccari Exp $

    This files contains all the functions necessary to handle the modulation
    input DAC events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "lprSerialInterface.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentMiDacModule=0;
/* Statics */
static HANDLER  miDacModulesHandler[MI_DAC_MODULES_NUMBER]={resetStrobeHandler};

/* Modulation input DAC handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the modulation input DAC. */
void miDacHandler(void){

    #ifdef DEBUG
        printf("      Modulation Input DAC: %d\n",
               currentMiSpecialMsgsModule);
    #endif /* DEBUG */

    /* No check is necessary on the existance of the hardware since, if the
       check on the modulation input passed, the DAC is automatically
       installed. */


    /* Since the is only one submodule in the modulation input DAC special
       messages, the check to see if the desired submodule is in range, is not
       needed and we can directly call the correct handler. */
    (miDacModulesHandler[currentMiDacModule])();
}

/* Reset Strobe Handler */
static void resetStrobeHandler(void){

    #ifdef DEBUG
        printf("       Reset Strobe\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   lpr.
                                    edfa.
                                     modulationInput.
                                      miSpecialMsgs.
                                       miDac.
                                        lastResetStrobe)

        /* Send the strobe */
        if(setLprDacStrobe()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             lpr.
              edfa.
               modulationInput.
                miSpecialMsgs.
                 miDac.
                  lastResetStrobe.
                   status=ERROR;

            return;
        }

    }

    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     lpr.
                                      edfa.
                                       modulationInput.
                                        miSpecialMsgs.
                                         miDac.
                                          lastResetStrobe)
        return;

    }

    /* If monitor on monitor RCA: this should never happen because there are
       no monitor available for this particular device. In case though, return
       a CAN range error to avoid timeouts. */
    storeError(ERR_MI_DAC,
               0x01); // Error 0x01 -> Monitor RCA out of range
    /* Store the state in the outgoing CAN message */
    CAN_STATUS = MON_CAN_RNG;

}

