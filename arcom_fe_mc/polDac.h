/*! \file       polDac.h
    \ingroup    polSpecialMsgs
    \brief      Polarization DAC header file

    <b> File information: </b><br>
    Created: 2004/08/24 15:42:19 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the polarization DAC special messages. In normal
    operation these message shouldn't be needed. */

/*! \defgroup   polDac    Polarization DAC
    \ingroup    polSpecialMsgs
    \brief      Polarization Dac
    \note       The \ref polDac module doesn't include any submodule.

    For more information on this module see \ref polDac.h */

#ifndef _POLDAC_H
    #define _POLDAC_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Defines */
    #define POL_DACS_NUMBER   2 //!< Number of DACs per polarization
    #define DAC1  0 //!< 0: DAC 1
    #define DAC2  1 //!< 1: DAC 2

    /* Submodules definitions */
    #define POL_DAC_MODULES_NUMBER      2       // See list below
    #define POL_DAC_MODULES_RCA_MASK    0x00020 /* Mask to extract the submodule number:
                                                   0 -> resetStrobe
                                                   1 -> clearStrobe */
    #define POL_DAC_MODULES_MASK_SHIFT 5        // Bits right shift for the submodules mask

    #define POL_DAC_ALLOW_CLEAR_STROBE DAC1     // Only DAC1 allows a clear strobe

    /* Typedefs */
    //! Current state of the polarization DAC
    /*! This structure represent the current state of the polarization DAC.
        Since these are only debug control messages and the strobes are triggered
        by the reception of the message independently from the payload, this
        structure is necessary exclusively to prevent monitor messages on these
        control addresses from timing out. The returned payload has no meaning.
        \ingroup    polSpecialMsgs
        \param      lastResetStrobe     This contains a copy of the last issued
                                        control message to the reset strobe.
        \param      lastClearStrobe     This contains a copy of the last issued
                                        control message to the clear strobe. */
    typedef struct {
        //! Last control message: DAC reset strobe
        /*! This is the content of the last control message sent to the dac
            reset strobe. */
        LAST_CONTROL_MESSAGE    lastResetStrobe;
        //! Last control message: DAC clear strobe
        /*! This is the content of the last control message sent to the dac
            clear strobe. */
        LAST_CONTROL_MESSAGE    lastClearStrobe;
    } POL_DAC;

    /* Globals */
    /* Externs */
    extern unsigned char currentPolDacModule; //!< Current addressed polarization DAC submodule

    /* Prototypes */
    /* Statics */
    static void resetStrobeHandler(void);
    static void clearStrobeHandler(void);
    /* Externs */
    extern void polDacHandler(void); //!< This function deals with the incoming can message

#endif /* _POLDAC_H */
