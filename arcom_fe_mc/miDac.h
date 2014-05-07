/*! \file       miDac.h
    \ingroup    miSpecialMsgs
    \brief      Modulation input DAC header file

    <b> File informations: </b><br>
    Created: 2007/06/22 16:55:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: miDac.h,v 1.1 2007/06/22 22:03:00 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the modulation input DACspecial messages. In
    normal operation these message shouldn't be needed. */

/*! \defgroup   miDac    Modulation input DAC
    \ingroup    miSpecialMsgs
    \brief      Modulation Input DAC
    \note       The \ref miDac module doesn't include any submodule.

    For more information on this module see \ref miDac.h */

#ifndef _MIDAC_H
    #define _MIDAC_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Defines */
    /* Submodules definitions */
    #define MI_DAC_MODULES_NUMBER      1       // It's just the resetStrobe

    /* Typedefs */
    //! Current state of the modulation input DAC
    /*! This structure represent the current state of the modulation input DAC.
        Since these are only debug control messages and the strobes are triggered
        by the reception of the message independently from the payload, this
        structure is necessary exclusively to prevent monitor messages on these
        control addresses from timing out. The returned payload has no meaning.
        \ingroup    polSpecialMsgs
        \param      lastResetStrobe     This contains a copy of the last issued
                                        control message to the reset strobe. */
    typedef struct {
        //! Last control message: DAC reset strobe
        /*! This is the content of the last control message sent to the dac
            reset strobe. */
        LAST_CONTROL_MESSAGE    lastResetStrobe;
    } MI_DAC;

    /* Globals */
    /* Externs */
    extern unsigned char currentMiDacModule; //!< Current addressed modulation input DAC submodule

    /* Prototypes */
    /* Statics */
    static void resetStrobeHandler(void);
    /* Externs */
    extern void miDacHandler(void); //!< This function deals with the incoming can message

#endif /* _MIDAC_H */
