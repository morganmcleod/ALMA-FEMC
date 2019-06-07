/*! \file       lnaLed.h
    \ingroup    polarization
    \brief      LNA led header file

    <b> File information: </b><br>
    Created: 2004/08/24 14:46:33 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the LNA led. */

/*! \defgroup   lnaLed  LNA led
    \ingroup    polarization
    \brief      LNA led module
    \note       The \ref lnaLed module doesn't include any submodule.

    For more information on this module see \ref lnaLed.h */

#ifndef _LNALED_H
    #define _LNALED_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

        /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Submodules definitions */
    #define LNA_LED_MODULES_NUMBER      1       // It's just the enableHandler

    /* Typedefs */
    //! Current state of the LNA led
    typedef struct {
        //! LNA led state
        /*! This is the state of the LNA led:
                - \ref LNA_LED_DISABLE -> OFF (power up state)
                - \ref LNA_LED_ENABLE -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char   enable;
        //! Last control message: LNA led state
        /*! This is the content of the last control message sent to the LNA led
            state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } LNA_LED;

    /* Globals */
    /* Externs */
    extern unsigned char currentLnaLedModule; //!< Current addressed LNA led submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    /* Externs */
    extern void lnaLedHandler(void); //!< This function deals with the incoming can message

    extern void lnaLedGoStandby2();
    //!< set the specified LNA LED to STANDBY2 mode

#endif /* _LNALED_H */
