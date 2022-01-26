/*! \file   timer.h
    \brief  Timers header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the timers used by the software. */

#ifndef _TIMER_H
    #define _TIMER_H

    /* Defines */
    #define MAX_TIMERS_NUMBER           100       //!< Max number of timers

    /*** RSS ***/
    #define TIMER_RSS                   0        // Timer number
    #define TIMER_TO_RSS                3600000L // Timeout in milliseconds

    /*** Serial Mux Board ***/
    #define TIMER_SERIAL_MUX            10      // Timer number
    #define TIMER_TO_SERIAL_MUX         1000    // Timeout in milliseconds

    /*** Cartridge level timers ***/
    /* Initialization timer */
    #define TIMER_CARTRIDGE_INIT        15      // Timer number
    #define TIMER_TO_CARTRIDGE_INIT     10       // Timeout in milliseconds

    /*** Bias Module ***/
    /* ADC */
    #define TIMER_BIAS_ADC_RDY          20      // Timer number
    #define TIMER_BIAS_TO_ADC_RDY       100     // Timeout in milliseconds
    /* DAC1 */
    #define TIMER_BIAS_DAC1_RDY         21      // Timer number
    #define TIMER_BIAS_TO_DAC1_RDY      100     // Timeout in milliseconds
    /* BAND9 SIS Heater */
    #define TIMER_BIAS_B9_HEATER(pol)   (22+pol)// Timer number
    #define TIMER_BIAS_TO_B9_HEATER     10000   // Timeout in milliseconds

    /*** LO Module ***/
    /* ADC */
    #define TIMER_LO_ADC_RDY            30      // Timer number
    #define TIMER_LO_TO_ADC_RDY         100     // Timeout in milliseconds

    /*** Power distribution Module */
    /* ADC */
    #define TIMER_PD_ADC_RDY            40      // Timer number
    #define TIMER_PD_TO_ADC_RDY         100     // Timeout in milliseconds

    /*** IF Switch Module ***/
    /* ADC */
    #define TIMER_IF_ADC_RDY            50      // Timer number
    #define TIMER_IF_TO_ADC_RDY         100     // Timeout in milliseconds

    /*** Cryostat Module ***/
    /* ADC */
    #define TIMER_CRYO_ANALOG_WAIT      60      // Timer number
    #define TIMER_CRYO_TO_ANALOG_WAIT   100     // Timeout in milliseconds
    /* CRYOSTAT_LOG_HOURS */
    #define TIMER_CRYO_LOG_HOURS        61      // Timer number
    #define TIMER_CRYO_LOG_HOURS_WAIT   3600000L // Timeout in milliseconds

    /*** LPR Module ***/
    /* ADC */
    #define TIMER_LPR_ADC_RDY           70      // Timer number
    #define TIMER_LPR_TO_ADC_RDY        100     // Timeout in milliseconds
    /* SWITCH READY */
    #define TIMER_LPR_SWITCH_RDY        71      // Timer number
    #define TIMER_LPR_TO_SWITCH_RDY     5000    // Timeout in milliseconds

    /*** One Wire Bus Module ***/
    /* IRQ */
    #define TIMER_OWB_IRQ               80      // Timer number
    #define TIMER_TO_OWB_IRQ            1000    // Timeout in milliseconds
    /* RESET */
    #define TIMER_OWB_RESET             81      // Timer number
    #define TIMER_TO_OWB_RESET          10000   // Timeout in milliseconds

    /*** Parallel port communication ***/
    /* AMBSI1 Ready */
    #define TIMER_PP_AMBSI_RDY          90      // Timer number
    #define TIMER_PP_TO_AMBSI_RDY       1000    // Timeout in milliseconds

    /* Timer control */
    #define TIMER_ON                    1
    #define TIMER_OFF                   0

    /* Timer status */
    #define TIMER_RUNNING               0       //!< Signal for timer running
    #define TIMER_EXPIRED               1       //!< Signal for timer expired
    #define TIMER_NOT_RUNNING           (-2)    //!< Signal for timer not running
    #define TIMER_NO_OUT_OF_RANGE       (-3)    //!< Signal for timer number out of range

    /* Prototypes */
    /* Externs */
    extern void waitMilliseconds(unsigned int milliseconds);  //!< Wait a defined number of milliseconds
    extern int startAsyncTimer(unsigned char timerNo,
                               unsigned long mSeconds,
                               unsigned char reload); //!< Setup and start the asynchronous timer
    extern int queryAsyncTimer(unsigned char timerNo); //!< Query the state of the asynchronous timer
    extern int stopAsyncTimer(unsigned char timerNo); //!< Clear the state of the asynchronous timer
#endif /* _TIMER_H */
