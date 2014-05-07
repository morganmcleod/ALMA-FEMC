/*! \file   globalOperations.c
    \brief  Global operations

    <b> File informations: </b><br>
    Created: 2006/10/24 11:52:13 by avaccari

    <b> CVS informations: </b><br>
    \$Id: globalOperations.c,v 1.16 2008/03/10 22:15:43 avaccari Exp $

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

/* Globals */
/* Externs */
unsigned char initializing = 3; /*!< This variable is a semaphore communicating
                                     the initialization state to the system. The
                                     value of 3 for the TRUE state is due to the
                                     trick used to minimize check time during
                                     the execution of the code when not in
                                     initialization time.
                                     During initialization only special messages
                                     are allowed. To minimize the time spend to
                                     check if the software is initializing, we
                                     use a trick where the function pointer is
                                     shifted by 3 during initialization. This
                                     saves time during normal execution. */
unsigned char initializeFrontend = FALSE;
unsigned char frontendInitializing = 0;
/* Initialization */
/*! This function takes care of initializing all the subsystem of the system.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int initialization(void){
    printf("Initializing...\n\n");

    /* Initialize the error library */
    if(errorInit()==ERROR){
        initializing = 0;
        return ERROR;
    }

    /* One wire bus initialization */
    #ifdef OWB
        if(owbInit()==ERROR){
            initializing = 0;
            return ERROR;
        }
    #endif /* OWB */

    /* Initialize the parallel port communication. This is done after the OWB
       initialization because there is no need for communication between ABMSI1
       and ARCOM before that.
       Up to this point, interrupts have been practically disabled. */
    if(PPOpen()==ERROR){
        initializing = 0;
        return ERROR;
    }

    /* At this point we gathered all the information about the ESNs and the
       communication is fully established with the AMBSI. Now we wait for the
       user to send a go-ahead command. During this wait, the configuration INI
       file can be uploaded via ethernet to the ARCOM drive. During the frontend
       initialization, they will be read. */

    /* Initialization loop */
    /* A timer to make sure that we are compatible with version of the software
       without this feature. This might be removed once the compatibility
       issue has been resolved. */
    startAsyncTimer(TIMER_INIT_WAIT_CMD,
                    TIMER_INIT_TO_WAIT_CMD);

    /* The initializeFrontend variable is a global variable. The state is
       changed in the can.c module by a control message. */
    do{
        /* Wait for messages */
        #ifdef DEBUG
            printf("Waiting for initialization control messages...\n\n");
        #endif /* DEBUG */

        /* Do whatever is that you do when you don't have anything to do */
        while(!newCANMsg&&!stop&&!initializeFrontend){
            /* Call the console handling. Make sure this doesn't affect
               performances. */
            console();
            if(queryAsyncTimer(TIMER_INIT_WAIT_CMD)==TIMER_EXPIRED){
                initializeFrontend=TRUE;
                break;
            }
        }

        /* If the software was stopped via console, shutdown */
        if(stop==TRUE){
            /* Shut down the frontend */
            if(shutDown()==ERROR){
            }

            /* If it was a restart request: restart the software. */
            if(restart==TRUE){
                printf("Rebooting system!\n");
                reboot();
            }

            return ERROR; // This return code from initialization will exit the software
        }

        #ifdef DEBUG
            printf("Message received!\n\n");
        #endif /* DEBUG */
        if(queryAsyncTimer(TIMER_INIT_WAIT_CMD)==TIMER_EXPIRED){
            break;
        }
        CANMessageHandler();
    } while (initializeFrontend==FALSE);


    /* Disable parallel port interrupt */
    PicPPIrqCtrl(DISABLE);
    PPIrqCtrl(DISABLE);
    PPClear();

    #ifdef DEBUG_INIT
        printf("Interrupts disabled!\n");
    #endif /* DEBUG_INIT */

    /* Initialize the frontend */
    if(frontendInit()==ERROR){
        initializing = 0;
        PPClear();
        PPIrqCtrl(ENABLE);
        PicPPIrqCtrl(ENABLE);
        return ERROR;
    }

    initializing = 0;
    #ifdef DEBUG_INIT
        printf("End initialization!\n");
    #endif /* DEBUG_INIT */

    /* Enable parallel port interrupt */
    PPClear();
    PPIrqCtrl(ENABLE);
    PicPPIrqCtrl(ENABLE);
    #ifdef DEBUG_INIT
        printf("Interrupts enabled!\n");
    #endif /* DEBUG_INIT */

    return NO_ERROR;
}

/*! This function takes care of shutting down all the subsystems.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int shutDown(void){
    printf("Shutting down...\n\n");

    /* Stop the timer for the RSS page update */
    stopAsyncTimer(TIMER_RSS);

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

    return NO_ERROR;
}


