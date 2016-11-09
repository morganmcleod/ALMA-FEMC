/*! \file       polarization.h
    \ingroup    cartridge
    \brief      polarization header file

    <b> File informations: </b><br>
    Created: 2004/08/24 16:33:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: polarization.h,v 1.18 2007/08/27 21:08:41 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate one of the two polarization available for each
    cartridge. */

/*! \defgroup   polarization    Polarization
    \ingroup    cartridge
    \brief      Polarization group
    \todo       Go back in all the submodules and modify the software that deals
                with single bytes contol/monitor. The higher level shouldn't
                know about the particularity of the message. Only the serial
                interface document relative to each hardware should know about
                that. The higher level should only call function of the type
                get/set without knowing how many bytes are in the message.
                Use the YTO as an example.

    This group includes all the different \ref polarization submodules. For more
    information on the \ref polarization module see \ref polarization.h*/


#ifndef _POLARIZATION_H
    #define _POLARIZATION_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Sideband */
    #ifndef _SIDEBAND_H
        #include "sideband.h"
    #endif /* _SIDEBAND_H */

    /* LNA LED */
    #ifndef _LNALED_H
        #include "lnaLed.h"
    #endif /* _LNALED_H */

    /* SIS Heater */
    #ifndef _SISHEATER_H
        #include "sisHeater.h"
    #endif /* _SIS_HEATER_H */

    /* Schottky mixer */
    #ifndef _SCHOTTKYMIXER_H
        #include "schottkyMixer.h"
    #endif /* _SCHOTTKYMIXER_H */

    /* Polarization special messages */
    #ifndef _POLSPECIALMSGS_H
        #include "polSpecialMsgs.h"
    #endif /* _POLSPECIALMSGS_H */

    /* Defines */
    #define POLARIZATIONS_NUMBER    2 //!< Number of polarizations per cartridge
    #define POLARIZATION0           0 //!< 0: Polarization 0
    #define POLARIZATION1           1 //!< 1: Polarization 1

    /* Submodules definitions */
    #define POLARIZATION_MODULES_NUMBER       6       // See list below
    #define POLARIZATION_MODULES_RCA_MASK     0x00380 /* Mask to extract the submodule number:
                                                         0 -> sideband
                                                         1 -> sideband
                                                         2 -> lnaLed
                                                         3 -> sisHeater
                                                         4 -> schottkyMixer
                                                         5 -> polSpecialMsgs (only control) */
    #define POLARIZATION_MODULES_MASK_SHIFT   7       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the polarization
    /*! This structure represent the current state of the polatization.
        \ingroup    cartridge
        \param      available       This contains the avilability state for the
                                    addressed polarization.
        \param      ssi10MHzEnable  This contains the current state of the 10MHz
                                    speed at the remote SSI interface:
                                        - \ref ENABLE   -> Speed is set to 10MHz
                                        - \ref DISABLE  -> Speed is set to 5MHz
        \param      sideband[Sb]    This contains the information about the
                                    sidebands available for this polarization.
                                    There can be up to \ref SIDEBANDS_NUMBER for
                                    each polarization.
        \param      lnaLed          This contains the information about the lna
                                    led available for this polarization. There
                                    is one lna led module for each
                                    polarization.
        \param      sisHeater       This contains the information about the sis
                                    heater available for this polarization.
                                    There is one sis heater module for each
                                    polarization.
        \param      schottkyMixer   This contains the information about the
                                    Schottky mixer available for this
                                    polarization. There is one Schottky mixer
                                    for each polarization. */
    typedef struct {
        //! Polarization availability
        /*! This variable indicates if the cartridge is outfitted with this
            particular polarization. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! SSI 10MHz Enable
        /*! This variable indicates the current communication speed for the
            remote device. Allowed speeds are the following:
                - \ref ENABLE   -> Speed is set to 10 MHz
                - \ref DISABLE  -> Speed is set to 5 MHz */
        unsigned char   ssi10MHzEnable;
        //! Sideband current state
        /*! Sidebands \p Sb are assigned according to the following:
                - Sb = 0: Sideband 1
                - Sb = 1: Sideband 2

            Please see \ref SIDEBAND for more informations. */
        SIDEBAND        sideband[SIDEBANDS_NUMBER];
        //!LNAled current state
        /*! Please see \ref LNA_LED for more informations. */
        LNA_LED         lnaLed;
        //! SIS heater current state
        /*! Please see \ref SIS_HEATER for more informations. */
        SIS_HEATER      sisHeater;
        //! Schottky mixer current state
        /*! Please see \ref SCHOTTKY_MIXER for more informations. */
        SCHOTTKY_MIXER  schottkyMixer;
        //! Polarization special messages current state
        /*! Please see \ref POL_SPECIAL_MSGS for more informations. */
        POL_SPECIAL_MSGS polSpecialMsgs;
    } POLARIZATION;

    /* Globals */
    /* Externs */
    extern unsigned char currentPolarizationModule; //!< Current addressed polarization submodule

    extern void polarizationGoStandby2(int cartridge, int polarization);
    //!< Set the specified polarization to STANDBY2 mode

    /* Prototypes */
    /* Externs */
    extern void polarizationHandler(void); //!< This function deals with the incoming can message
    extern int polarizationInit(void); //!< This function initializes the selected polarization at runtime

#endif /* _POLARIZATION_H */
