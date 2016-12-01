/*! \file   dewar.c
    \brief  FETIM dewar functions

    <b> File informations: </b><br>
    Created: 2011/03/28 17:13:45 by avaccari

    <b> CVS informations: </b><br>
    \$Id: dewar.c,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the functions necessary to handle FETIM dewar
    events. */

/* Includes */
#include <stdio.h>      /* printf */
#include <string.h>     /* memcpy */

#include "debug.h"
#include "frontend.h"
#include "fetimSerialInterface.h"
#include "error.h"

/* Globals */
/* Externs */
unsigned char   currentDewarModule=0;
/* Statics */
static HANDLER dewarModulesHandler[DEWAR_MODULES_NUMBER]={n2FillHandler};


/* FETIM dewar handler */
/*! This function will be called by the CAN message handling subrutine when the
    received message is pertinent to the FETIM dewar. */
void dewarHandler(void){

    #ifdef DEBUG_FETIM
        printf("  Dewar\n");
    #endif /* DEBUG_FERIM */

    /* Since there is only one submodule in the FETIM dewar, the check to see if
       the desired submodule is in range, is not needed and we can directly call
       the correct handler. */
    /* Call the correct handler */
    (dewarModulesHandler[currentDewarModule])();

    return;
}






/* N2 fill handler */
/* This function deals with the messages directed to the N2 fill. */
static void n2FillHandler(void){

    #ifdef DEBUG_FETIM
        printf("   N2 Fill\n");
    #endif /* DEBUG_FETIM */


    /* If control (size !=0) */
    if(CAN_SIZE){
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   fetim.
                                    dewar.
                                     lastN2Fill)

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         fetim.
          dewar.
           lastN2Fill.
            status=NO_ERROR;

        /* Change the status of the backing pump according to the content of the
           CAN message. */
        if(setN2FillEnable(CAN_BYTE?N2_FILL_ENABLE:
                                    N2_FILL_DISABLE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             fetim.
              dewar.
               lastN2Fill.
                status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     fetim.
                                      dewar.
                                       lastN2Fill)
        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the N2 fill is changed by a control command. */
    CAN_BYTE=frontend.
              fetim.
               dewar.
                n2Fill[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;




}

