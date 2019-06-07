/*! \file       polarization.h
    \ingroup    cartridge
    \brief      polarization header file

    <b> File information: </b><br>
    Created: 2004/08/24 16:33:14 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate one of the two polarization available for each
    cartridge. */

/*! \defgroup   polarization    Polarization
    \ingroup    cartridge
    \brief      Polarization group

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
                                                         4 -> none
                                                         5 -> polSpecialMsgs (only control) */
    #define POLARIZATION_MODULES_MASK_SHIFT   7       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the polarization
    typedef struct {
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

            Please see \ref SIDEBAND for more information. */
        SIDEBAND        sideband[SIDEBANDS_NUMBER];

        //!LNAled current state
        /*! Please see \ref LNA_LED for more information. */
        LNA_LED         lnaLed;

        //! SIS heater current state
        /*! Please see \ref SIS_HEATER for more information. */
        SIS_HEATER      sisHeater;

        //! Polarization special messages current state
        /*! Please see \ref POL_SPECIAL_MSGS for more information. */
        POL_SPECIAL_MSGS polSpecialMsgs;
    } POLARIZATION;

    /* Globals */
    /* Externs */
    extern unsigned char currentPolarizationModule; //!< Current addressed polarization submodule

    /* Prototypes */
    /* Externs */
    extern void polarizationHandler(void); //!< This function deals with the incoming can message
    extern void RESERVEDHandler(void);     //!< Dummy handler for where schottkyMixer used to be.
    extern int polarizationInit(void); //!< This function initializes the selected polarization at runtime

#endif /* _POLARIZATION_H */
