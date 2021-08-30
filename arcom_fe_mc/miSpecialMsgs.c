/*! \file   miSpecialMsgs.c
    \brief  Modulation input special messages functions

    <b> File information: </b><br>
    Created: 2007/06/22 16:48:20 by avaccari

    This file contains all the functions necessary to handle modulation input
    special messages events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "miSpecialMsgs.h"
#include "error.h"
#include "globalDefinitions.h"
#include "debug.h"

/* Globals */
/* Externs */
unsigned char   currentMiSpecialMsgsModule=0;
/* Statics */
static HANDLER  miSpecialMsgsModulesHandler[MI_SPECIAL_MSGS_MODULES_NUMBER]={miDacHandler};

/* Modulation Input special messages handler */
/*! This function will be called by the CAN message handling subroutine when the
    received message is pertinent to the modulation input special messages. */
void miSpecialMsgsHandler(void){
    /* The value of currentMiSpecialMsgsModule is not changed since there is
       only one submodule in the IF temperature servo module.
       Ths structure is preserved only for consistency.
       If the timing should be an issue, it can be removed and the functionality
       can be placed directly in the ifTempServoHandler function. */

    #ifdef DEBUG
        printf("    Modulation Input Special Message:\n");
    #endif /* DEBUG */

    /* There is no need to check for the existance of the hardware because if
       the addressed modulation input exists then also the DAC is installed. */

    /* Since the is only one submodule in the modulation input special messages,
       the check to see if the desired submodule is in range, is not needed and
       we can directly call the correct handler. */
    (miSpecialMsgsModulesHandler[currentMiSpecialMsgsModule])();
}

