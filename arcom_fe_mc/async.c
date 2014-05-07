/*! \file   async.c
    \brief  Async operations executed at idle time

    <b> File informations: </b><br>
    Created: 2009/03/25 18:17:13 by avaccari

    <b> CVS informations: </b><br>
    \$Id: async.c,v 1.1 2009/04/09 02:09:55 avaccari Exp $

    This file contains the functions that take care of executing operation while
    the cpu is idle between incoming CAN messages. */

/* Includes */
#include <stdio.h>  /* printf */

#include "async.h"
#include "timer.h"
#include "globalDefinitions.h"
#include "frontend.h"

/* Globals */
unsigned char asyncRequired=FALSE;
/* Externs */
/* Statics */

/* Executes async operations. This could be in its own module. */
/*! This function handles the async operations. These are tasks the FEMC will
    execute while idle between can messages. */
void async(void){

    return;
}
