/*! \file       polSpecialMsgs.h
    \ingroup    specialMsgs
    \ingroup    polarization
    \brief      Polarization special messages header file

    <b> File informations: </b><br>
    Created: 2004/08/24 14:46:33 by avaccari

    <b> CVS informations: </b><br>
    \$Id: polSpecialMsgs.h,v 1.10 2007/06/22 22:03:00 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and handle the polarization special messages. */

/*! \defgroup   polSpecialMsgs  Polarization special messages
    \ingroup    specialMsgs
    \ingroup    polarization
    \brief      Polarization special messages module

    This group includes all the different \ref polSpecialMsgs submodules. For
    more information on the \ref polSpecialMsgs module see \ref polSpecialMsgs.h */

#ifndef _POLSPECIALMSGS_H
    #define _POLSPECIALMSGS_H

    /* Extra includes */
    /* polarization DAC */
    #ifndef _POLDAC_H
        #include "polDac.h"
    #endif /* _POLDAC_H */

    /* Submodules definitions */
    #define POL_SPECIAL_MSGS_MODULES_NUMBER     2       // See list below
    #define POL_SPECIAL_MSGS_MODULES_RCA_MASK   0x00040 /* Mask to extract the submodule number:
                                                           0 -> dac
                                                           1 -> dac */
    #define POL_SPECIAL_MSGS_MODULES_MASK_SHIFT 6       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the polarization special messages
    /*! This structure represent the current state of the polarization special
        messages.
        \ingroup    polarization
        \param      This contains the information about the DACs available for
                    this module. There can be up to \ref POL_DACS_NUMBER for
                    each special messages module. */
    typedef struct {
        //! Polarization dac current state
        /*! Please see the definition of the \ref POL_DAC structure for more
            informations.*/
        POL_DAC           polDac[POL_DACS_NUMBER];
    } POL_SPECIAL_MSGS;

    /* Globals */
    /* Externs */
    extern unsigned char currentPolSpecialMsgsModule; //!< Current addressed polarization special submodule

    /* Prototypes */
    /* Externs */
    extern void polSpecialMsgsHandler(void); //!< This function deals with the incoming can message

#endif /* _POLSPECIALMSGS_H */
