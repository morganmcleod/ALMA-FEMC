/*! \file       sideband.h
    \ingroup    polarization
    \brief      Sideband header file

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sideband.h,v 1.15 2010/11/02 14:36:29 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate one of the two sidebands available for each
    polarization. See \ref sideband for more information. */

/*! \defgroup   sideband    Sideband
    \ingroup    polarization
    \brief      Sideband module

    This group includes all the different \ref sideband submodules. For more
    information on the \ref sideband module see \ref sideband.h */

#ifndef _SIDEBAND_H
    #define _SIDEBAND_H

    /* Extra includes */
    /* LNA */
    #ifndef _LNA_H
        #include "lna.h"
    #endif /* _LNA_H */

    /* SIS */
    #ifndef _SIS_H
        #include "sis.h"
    #endif /* _SIS_H */

    /* SIS magnet */
    #ifndef _SISMAGNET_H
        #include "sisMagnet.h"
    #endif /* _SIS_MAGNET_H */

    /* Defines */
    #define SIDEBANDS_NUMBER    2 //!< Number of sidebands per polarization
    #define SIDEBAND0           0 //!< 0: Upper
    #define SIDEBAND1           1 //!< 1: Lower

    /* Submodules definitions */
    #define SIDEBAND_MODULES_NUMBER       3       // See list below
    #define SIDEBAND_MODULES_RCA_MASK     0x00060 /* Mask to extract the submodule number:
                                                     0 -> sis
                                                     1 -> sisMagnet
                                                     2 -> lna */
    #define SIDEBAND_MODULES_MASK_SHIFT   5       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the sideband
    /*! This structure represent the current state of the sideband.
        \ingroup    polarization
        \param      available   an unsigned int
        \param      lna         a LNA
        \param      sis         a SIS
        \param      sisMagnet   a SIS_MAGNET  */
    typedef struct {
        //! Sideband availability
        /*! This variable indicates if the polarization is outfitted with this
            particular sideband. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //!LNAcurrent state
        /*! Please see the definition of the \ref LNA structure for more
            informations. */
        LNA         lna;
        //! SIS mixer current state
        /*! Please see the definition of the \ref SIS structure for more
            informations. */
        SIS         sis;
        //! SIS magnetic coil current state
        /*! Please see the definition of the \ref SIS_MAGNET structure for more
            informations. */
        SIS_MAGNET  sisMagnet;
    } SIDEBAND;

    /* Globals */
    /* Externs */
    extern unsigned char currentSidebandModule; //!< Current addressed sideband submodule

    extern void sidebandGoStandby2(int cartridge, int polarization, int sideband);
    //!< Set the specified sideband to STANDBY2 mode

    /* Prototypes */
    /* Externs */
    extern void sidebandHandler(void); //!< This function deals with the incoming CAN message

#endif /* _SIDEBAND_H */
