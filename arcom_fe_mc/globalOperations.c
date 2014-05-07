/*! \file   globalOperations.c
    \brief  Global operations

    <b> File informations: </b><br>
    Created: 2006/10/24 11:52:13 by avaccari

    <b> CVS informations: </b><br>
    \$Id: globalOperations.c,v 1.23 2011/11/28 22:10:49 avaccari Exp $

    This files contains all the functions necessary to handle global frontend
    operations. */


/* Includes */
#include <i86.h>        /* _enable, _disable */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "globalDefinitions.h"
#include "owb.h"
#include "timer.h"
#include "main.h"
#include "console.h"
#include "pegasus.h"
#include "debug.h"
#include "ppComm.h"
#include "serialMux.h"

/* Initialization */
/*! This function takes care of initializing all the subsystem of the system.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int initialization(void){
    printf("Initializing...\n\n");

    /* Initialize the error library */
    if(errorInit()==ERROR){
        return ERROR;
    }

    /* Initialize the Serial Mux board */
    if(serialMuxInit()==ERROR){
        return ERROR;
    }

    /* One wire bus initialization */
    #ifdef OWB
        if(owbInit()==ERROR){
            return ERROR;
        }

        if(owbGetEsn()==ERROR){
            return ERROR;
        }
    #endif /* OWB */

    /* Switch to maintenance mode before allowing interrupt. The idea is that at
       this point the FE will be in a state where it knows about the installed
       hardware and the data is available for the control software to analyze.
       While in maintenance mode, all the housekeeping operation should be
       performed if necessary:
       - update configuration files
       - ...
       The FE will wait for a message to switch to operational mode. (Right now
       the switch is automatic. But the framework is put in place to allow this
       control of states during startup). */
    /* Switch to operational mode */
    frontend.
     mode[CURRENT_VALUE] = MAINTENANCE_MODE;

    /* Initialize the parallel port communication. This is done after the OWB
       initialization because there is no need for communication between ABMSI1
       and ARCOM before that.
       Up to this point, interrupts have been practically disabled. */
    if(PPOpen()==ERROR){
        return ERROR;
    }


    /* At this point we gathered all the information about the ESNs and the
       communication is fully established with the AMBSI. Now we wait for the
       user to send a go-ahead command. During this wait, the configuration INI
       file can be uploaded via ethernet to the ARCOM drive. During the frontend
       initialization, they will be read. At the end of the initialization, the
       frontend module switched the mode to operational. */


    /* Initialize the frontend */
    if(frontendInit()==ERROR){
        return ERROR;
    }

    printf("End initialization!\n\n");

    return NO_ERROR;
}

/*! This function takes care of shutting down all the subsystems.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int shutDown(void){
    printf("Shutting down...\n\n");

    /* Stop the timer for the RSS page update */
//    stopAsyncTimer(TIMER_RSS);

    /* Shut down the frontend */
    if(frontendStop()==ERROR){
/**************************************************** Do something? ******/
    }

    /* Shut down the parallel port communication */
    if(PPClose()==ERROR){
/**************************************************** Do something? ******/
    }

    /* Shut down the error handling */
    if(errorStop()==ERROR){
/**************************************************** Do something? ******/
    }

    printf("Shut down complete! Exiting...\n\n");

    return NO_ERROR;
}


