/* based on Andrea's main.c

/*  $Id: feMCTest.cpp,v 1.1 2013/05/02 00:08:23 jpisano Exp $

//  */
//     #include <stdio.h>      /* printf */
// extern "C"
// {
//     #define DEBUG
//     #include "debug.h"
//     #include "version.h"
//     #include "globalOperations.h"
//     #include "globalDefinitions.h"
    #include "ppcomm.h"
    #include "main.h"













































    #include "error.h"
    #include "can.h"
    #include "pegasus.h"
    #include "console.h"
    #include "async.h"
    #include "timer.h"
}// extern "C"
// #define _ARCH_DIR i386
//#include <signal.h>
//#include <time.h>
//#include <timer.h>
#include <types.h>
//#include <c:/WATCOM/lh/sys/time.h> // itimer a POSIX-like timer
// these are declared in main.c,but not found
unsigned char restart = 0;
    unsigned char stop = 0;

//-----------------------------------------------------------------------
/* @(#) $Id: feMCTest.cpp,v 1.1 2013/05/02 00:08:23 jpisano Exp $
 *
 * Copyright (C) 2006
 * Associated Universities, Inc. Washington DC, USA.
 *
 * This library is free software; you can redistribute it and/or modify it
 * under the terms of the GNU Library General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or (at your
 * option) any later version.
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 * 
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307  USA
 * Correspondence concerning ALMA Software should be addressed as follows:
 * Internet email: alma-sw-admin@nrao.edu
 */

class FeMcTest
{
public:
    /// default constructor
    FeMcTest();

    /// copy constructor
    FeMcTest( const FeMcTest &copyOf)
	{ *this = copyOf; }

    /// assignment operator
    FeMcTest &operator =( const FeMcTest &rhs)
    { if ( this != &rhs)
	{} 
	return *this; 
    }

    /// destructor
    ~FeMcTest() {}
/*
private:
*/
};
/* This was copied from Andrea's fe_mc.c as a stable starting point
to use the fe_mc functions 2/19/2013
*/





// Source: http://fixunix.com/vxworks/49632-implementing-posix-timer-example.html
//-----------------------------------------------------------------------
void PosixTimerTest()
{
    /*
The timer_create() function creates a per-process timer using the specified clock source, clock_id, as the timing base. The only supported clock ID is CLOCK_REALTIME.

The returned timer ID is used in subsequent calls to timer_gettime(), timer_settime() and timer_delete().

The timer is created in the disabled state, and isn't enabled until timer_settime() is called.

The sigevent structure pointed to by evp contains at least the following member:

int sigev_signo
    If sigev_signo is positive, it's interpreted as a signal number to be sent to the calling process when the timer expires.

    If sigev_signo is negative then it's interpreted as a proxy to trigger when the timer expires. If evp is NULL, then a signal of SIGALRM is set when the timer expires.

    If sigev_signo is 0, the process sleeps until the timer expires.

Synopsis:
    */
/*
    clockid_t  clock_id = CLOCK_REALTIME;
    timer_t myTimer = timer_create(clock_id ); 
*/
}
//-----------------------------------------------------------------------
/*
void myTimerhandlerHandler(time_t timerid, int myarg )
{
}
*/
//-----------------------------------------------------------------------
/*
//void start_timer(void)
{

//time_t timerid;
//struct itimerval value;

value.it_value = 5;
value.it_value.tv_nsec = 0;

value.it_interval.tv_sec = 5;
value.it_interval.tv_nsec = 0;

timer_create (CLOCK_REALTIME, NULL, &timerid);

timer_connect (timerid, myTimerhandlerHandler,0);

timer_settime (timerid, 0, &value, NULL);
 }
*/



//-----------------------------------------------------------------------
/*
//int myTimerHandler()(timer_t time_t timerid,int myarg )
{
    printf("\nHello World\nmyarg: %d\n",myarg);
return 0;
}
*/
//-----------------------------------------------------------------------
int main(void)
{
    /* Print version information */
    //displayVersion();

    /* Initialize the frontend */
    if(initialization()==ERROR)
    {

	printf("Front End initialization FAILED AT at  %s:%d\n"__FILE__, __LINE__);
        return ERROR;
    }

    /* Enable connection with AMBSI1 */
    if(PPStart()==ERROR)
    {
	printf("ABSI1 initialization FAILED at %s:%d\n"__FILE__, __LINE__);
        return ERROR;
    }

    /* Main Super-loop 
1. Get a CAN msg
2. figure out if it's aMonitor request
2.1 if so, build a response
2.2 & return it
3. if not, process control command.
*/
    consoleEnable = 1;
  
//    while(!stop) // extra forever loop which seems to prevent cosole loop???
    {
        
/*	DEBUG_PRINT("Waiting for messages...\n\n");
        #ifdef DEBUG

        #endif // DEBUG 
*/
//        printf("Waiting for messages...\n%s:%d\n",__FILE__,__LINE__);
//        printf("Waiting for messages...\n\n");
        /* Do whatever is that you do when you don't have anything to do */
        while(!newCANMsg && !stop)
       {

            /* Call the console handling. Make sure this doesn't affect
               performances. */
            if(consoleEnable)
            {
                console();
            }

            /* Perform the required asynchronous operations, i.e. FEMC tasks while idle between
	       CAN msgs. */
            async();
	    //Query the RSS update timer. 
//            if(queryAsyncTimer(TIMER_RSS))
              {
//                   
		   // Restart the timer
		  startAsyncTimer(TIMER_RSS,TIMER_TO_RSS, FALSE);
              }
	 
       }

        /* If the software was stopped via console, don't handle the message */

        if(stop==TRUE)
        {
            return 0;// should be exit(0);
        }
/*
        #ifdef DEBUG
            printf("Message received!\n\n");
        #endif  DEBUG */
//	DEBUG_PRINT(("Message received!\n\n");
//        CANMessageHandler();

    }

    /* Shut down the frontend */
    if(shutDown()==ERROR)
    {// JAP 2/19/2013: looks like do nothing ???
    }

    /* If it was a restart request: restart the software. */
if(restart==TRUE)
    {
        printf("Rebooting system!\n");
        reboot();
    }

    return NO_ERROR;
}
