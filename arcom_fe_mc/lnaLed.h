/*! \file       lnaLed.h
    \ingroup    polarization
    \brief      LNA led header file

    <b> File informations: </b><br>
    Created: 2004/08/24 14:46:33 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lnaLed.h,v 1.18 2007/03/14 21:20:17 avaccari Exp $

    This files contains all the informations necessary to define the
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
    /*! This structure represent the current state of the LNA led.
        \ingroup    polarization
        \param      available   This contains the availability state for the
                                addressed lna led:
                                    - \ref UNAVAILABLE -> Unavailable
                                    - \ref AVAILABLE -> Available
        \param      enable[Op]  This contains the current state of the lna led.
                                It has to be rememberd that this is \em not a
                                read-back from the hardware but just a register
                                holding the last issue control:
                                    - \ref LNA_LED_DISABLE -> OFF
                                    - \ref LNA_LED_ENABLE -> ON
        \param      lastEnable  This contains a copy of the last issued control
                                message to the lna led enable. */
    typedef struct {
        //! LNA led availability
        /*! This variable indicates if this sideband is outfitted with this
            particularLNAled. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! LNA led state
        /*! This is the state of the LNA led:
                - \ref LNA_LED_DISABLE -> OFF (power up state)
                - \ref LNA_LED_ENABLE -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char   enable[OPERATION_ARRAY_SIZE];
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

#endif /* _LNALED_H */
