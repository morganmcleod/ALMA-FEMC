/*! \file   timer.c
    \brief  Timers functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle time events.
    There are two different timers available:
        - \ref waitSeconds  This is a synchronous timer meaning that once it is
          called, the execution of the program stops until the timer expires.
        - \ref startAsyncTimer, \ref queryAsyncTimer, \ref stopAsyncTimer
          These function handle one of the \ref MAX_TIMERS_NUMBER provided
          asynchronous timer. In this case the timer is started and the status
          can be queried to figure out if the timer is expired or not. This is
          useful to implement timeouts which do not have stringent requirements
          on the precision of the timer. */

/* Includes */
#include <time.h>   /* clock */
#include <i86.h>    /* delay */

#include "timer.h"
#include "error.h"
#include "globalDefinitions.h"

/* Globals */
static clock_t asyncStartTime[MAX_TIMERS_NUMBER]; // A global for the async timer start time
static unsigned char asyncRunning[MAX_TIMERS_NUMBER]; // A global for the async timer current state
static unsigned long asyncMSeconds[MAX_TIMERS_NUMBER]; // A global for the async timer wait time

/*! This function will wait \p milliseconds seconds before returning.
    \param  milliSeconds     The amount of milliseconds to wait */
void waitMilliseconds(unsigned int milliseconds){
    delay(milliseconds);
}

/*! This function will initialize and start the asynchronous timer. The timer
    will wait the ammount specified in the parameter.
    \param timerNo  The timer to activate. The maximum number of timers is
                    defined by \ref MAX_TIMERS_NUMBER
    \param mSeconds The number of milliseconds to wait
    \param reload   If \ref TRUE then reload the timer with the new value

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int startAsyncTimer(unsigned char timerNo,
                    unsigned long mSeconds,
                    unsigned char reload){
    /* If there is an attempt to initialize a timer that is not available, don't
       initialize and return an error */
    if(timerNo>MAX_TIMERS_NUMBER){
        storeError(ERR_TIMER, ERC_MODULE_RANGE); //Required async timer out of range
        return ERROR;
    }

    /* If the timer is already running and this is not a reload, don't
       initialize and return an error */
    if(asyncRunning[timerNo]){
        if(reload==FALSE){
            storeError(ERR_TIMER, ERC_HARDWARE_WAIT); //Async timer already running
            return ERROR;
        }
    }

    asyncStartTime[timerNo]=clock(); // Store the current time
    asyncMSeconds[timerNo]=mSeconds; // Store the wait time
    asyncRunning[timerNo]=TIMER_ON; // The timer is now running

    return NO_ERROR;
}

/*! This function queries the state of the selected asynchronous timer. Every
    time is called after while the timer has been started with
    \ref startAsyncTimer it will return the status of the timer.
    The async timer can be stopped at any time and cleared by calling the
    function \ref stopAsyncTimer
    \param timerNo an unsigned char
    \return
        - \ref ERROR                    -> something went wrong
        - \ref TIMER_RUNNING            -> not expired
        - \ref TIMER_EXPIRED            -> expired
        - \ref TIMER_NOT_RUNNING        -> timer not running
        - \ref TIMER_NO_OUT_OF_RANGE    -> timer number outside range */
int queryAsyncTimer(unsigned char timerNo){
    /* If there is an attempt to query a timer that is not available, don't
       query and return an error */
    if(timerNo>MAX_TIMERS_NUMBER){
        storeError(ERR_TIMER, ERC_MODULE_RANGE); //Required async timer out of range
        return TIMER_NO_OUT_OF_RANGE;
    }

    /* Check if the async timer is running or not */
    if(!asyncRunning[timerNo]){
        return TIMER_NOT_RUNNING;
    }

    /* Check if the async timer is running */
    if((clock()-asyncStartTime[timerNo])<asyncMSeconds[timerNo]){
        return TIMER_RUNNING;
    }

    /* Timer expired: stop the timer */
    if(stopAsyncTimer(timerNo)==ERROR){
        return ERROR;
    }

    return TIMER_EXPIRED;

}

/*! This function will clear the state of the selected asynchronous timer when
    called.
    \param timerNo an unsigned char

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int stopAsyncTimer(unsigned char timerNo){
    /* If there is an attempt to stop a timer that is not available, don't stop
       and return an error */
    if(timerNo>MAX_TIMERS_NUMBER){
        storeError(ERR_TIMER, ERC_MODULE_RANGE); //Required async timer out of range
        return ERROR;
    }
    asyncRunning[timerNo]=TIMER_OFF;
    return NO_ERROR;
}
