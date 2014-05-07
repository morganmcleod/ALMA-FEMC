/*! \file   timer.h
    \brief  Timers header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: timer.h,v 1.23 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the timers used by the software. */

#ifndef _TIMER_H
    #define _TIMER_H

    /* Defines */
    #define MAX_TIMERS_NUMBER       20      //!< Max number of timers

    /*** RSS ***/
    #define TIMER_RSS               0       // Timer number
    #define TIMER_TO_RSS            3600000L // Timeout in milliseconds

    /*** Serial Mux Board ***/
    #define TIMER_SERIAL_MUX        1       // Timer number
    #define TIMER_TO_SERIAL_MUX     100     // Timeout in milliseconds

    /*** Bias Module ***/
    /* ADC */
    #define TIMER_BIAS_ADC_RDY      2       // Timer number
    #define TIMER_BIAS_TO_ADC_RDY   100     // Timeout in milliseconds
    /* DAC1 */
    #define TIMER_BIAS_DAC1_RDY     3       // Timer number
    #define TIMER_BIAS_TO_DAC1_RDY  100     // Timeout in milliseconds

    /*** LO Module ***/
    /* ADC */
    #define TIMER_LO_ADC_RDY        4       // Timer number
    #define TIMER_LO_TO_ADC_RDY     100     // Timeout in milliseconds

    /*** Power distribution Module */
    /* ADC */
    #define TIMER_PD_ADC_RDY        5       // Timer number
    #define TIMER_PD_TO_ADC_RDY     100     // Timeout in milliseconds

    /*** IF Switch Module ***/
    /* ADC */
    #define TIMER_IF_ADC_RDY        6       // Timer number
    #define TIMER_IF_TO_ADC_RDY     100     // Timeout in milliseconds

    /*** Cryostat Module ***/
    /* ADC */
    #define TIMER_CRYO_ADC_RDY      7       // Timer number
    #define TIMER_CRYO_TO_ADC_RDY   100     // Timeout in milliseconds

    /*** LPR Module ***/
    /* ADC */
    #define TIMER_LPR_ADC_RDY       8       // Timer number
    #define TIMER_LPR_TO_ADC_RDY    100     // Timeout in milliseconds
    /* SWITCH READY */
    #define TIMER_LPR_SWITCH_RDY    9       // Timer number
    #define TIMER_LPR_TO_SWITCH_RDY 5000    // Timeout in milliseconds

    /*** One Wire Bus Module ***/
    /* IRQ */
    #define TIMER_OWB_IRQ           10      // Timer number
    #define TIMER_TO_OWB_IRQ        1000    // Timeout in milliseconds
    /* RESET */
    #define TIMER_OWB_RESET         11      // Timer number
    #define TIMER_TO_OWB_RESET      10000   // Timeout in milliseconds

    /*** Initialization ***/
    /* Initialize command */
    #define TIMER_INIT_WAIT_CMD     12      // Timer number
    #define TIMER_INIT_TO_WAIT_CMD  20000   // Timeout in milliseconds

    /* Timer control */
    #define TIMER_ON                1
    #define TIMER_OFF               0

    /* Timer status */
    #define TIMER_RUNNING           0       //!< Signal for timer running
    #define TIMER_EXPIRED           1       //!< Signal for timer expired
    #define TIMER_NOT_RUNNING       (-2)    //!< Signal for timer not running
    #define TIMER_NO_OUT_OF_RANGE   (-3)    //!< Signal for timer number out of range

    /* Prototypes */
    /* Externs */
    extern void waitSeconds(unsigned int seconds);  //!< Wait a defined number of seconds
    extern int startAsyncTimer(unsigned char timerNo,
                               unsigned long mSeconds); //!< Setup and start the asynchronous timer
    extern int queryAsyncTimer(unsigned char timerNo); //!< Query the state of the asynchronous timer
    extern int stopAsyncTimer(unsigned char timerNo); //!< Clear the state of the asynchronous timer
#endif /* _TIMER_H */
