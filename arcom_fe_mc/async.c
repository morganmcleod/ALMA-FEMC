/*! \file   async.c
    \brief  Async operations executed at idle time

    <b> File informations: </b><br>
    Created: 2009/03/25 18:17:13 by avaccari

    <b> CVS informations: </b><br>
    \$Id: async.c,v 1.5 2009/10/13 15:01:49 avaccari Exp $

    This file contains the functions that take care of executing operation while
    the cpu is idle between incoming CAN messages. */

/* Includes */
#include "async.h"
#include "frontend.h"
#include "error.h"

/* Globals */
ASYNC_STATE asyncState = ASYNC_CRYOSTAT; /*!< This variable contains the current status
                                              of the async process. */
/* Externs */


/* Statics */

/* Executes async operations. This could be in its own module. */
/*! This function handles the async operations. These are tasks the FEMC will
    execute while idle between can messages. */
void async(void){

    /* Switch to the correct subsystem */
    switch(asyncState){
        /* Run the cryostat async functions */
        case ASYNC_CRYOSTAT:
            /* If done or error, go to next subsystem */
            switch(cryostatAsync()){
                case NO_ERROR:
                    return;
                    break;
                case ASYNC_DONE:
                case ERROR:
                    break;
                default:
                    break;
            }

            break;
        case ASYNC_OFF:
            asyncState=ASYNC_OFF;
            break;
        case ASYNC_ON:
            asyncState=ASYNC_CRYOSTAT;
            break;

        default:
            break;
    }

    return;
}
