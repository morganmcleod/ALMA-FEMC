/*! \file       lpr.h
    \ingroup    lpr
    \brief      LO Photonic Receiver header file

    <b> File informations: </b><br>
    Created: 2007/05/29 14:49:12 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lpr.h,v 1.7 2009/03/23 19:22:04 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the LO photonic receiver. */

/*! \defgroup   lpr         LO Photonic Receiver system
    \ingroup    frontend
    \brief      LO Photonic Reveicer system

    This group includes all the different \ref lpr submodules. For more
    information on the \ref lpr module see \ref lpr.h */

#ifndef _LPR_H
    #define _LPR_H

    /* Extra includes */
    /* LPRTEMP defines */
    #ifndef _LPRTEMP_H
        #include "lprTemp.h"
    #endif /* _LPRTEMP_H */

    /* OPTICALSWITCH defines */
    #ifndef _OPTICALSWITCH_H
        #include "opticalSwitch.h"
    #endif /* _OPTICALSWITCH_H */

    /* EDFA defines */
    #ifndef _EDFA_H
        #include "edfa.h"
    #endif /* _EDFA_H */

    /* Defines */
    /* Configuration data info */
    #define LPR_CONF_FILE_SECTION   "LPR"   // Section containing the LPR configuration file info
    #define LPR_CONF_FILE_KEY       "FILE"  // Key containing the LPR configuration file info
    #define LPR_CONF_FILE_EXPECTED  1       // Expected keys containing the LPR configuration file info

    #define POWER_COEFF_SECTION     "EDFA_PD"       // Section containing the power detector scaling info
    #define POWER_COEFF_KEY         "POWER_COEFF"   // Key containing the power detector scaling info
    #define POWER_COEFF_EXPECTED    1               // Expected keys containing the power detector scaling info in the section
    #define POWER_COEFF_DEFAULT     18.6            // Default value to use if no database is available

    #define LPR_ESN_SECTION         "INFO"  // Section containing the LPR serial number
    #define LPR_ESN_KEY             "ESN"   // Key containing the LPR serial number
    #define LPR_ESN_EXPECTED        1       // Expected keys containing the LPR serial number

    /* Submodule definitions */
    #define LPR_MODULES_NUMBER      4       // See list below
    #define LPR_MODULES_RCA_MASK    0x00030 /* Mask to extract the submodule number:
                                               0-1  -> lprTemp
                                               2    -> opticalSwitch
                                               3    -> edfa */
    #define LPR_MODULES_MASK_SHIFT  4       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the LPR system
    /*! This structure represent the current state of the LPR system
        \ingroup    frontend
        \param      ssi10MHzEnable  This contains the current state of the 10MHz
                                    speed at the remote SSI interface:
                                        - \ref ENABLE   -> Speed is set to 10MHz
                                        - \ref DISABLE  -> Speed is set to 5MHz
        \param      lprTemp[Sn]     This contains the information about the lpr
                                    temperature sensors. There are
                                    \ref LPR_TEMP_SENSORS_NUMBER sensor in each
                                    lpr.
        \param      opticalSwitch   This contains the information about the
                                    state fo the optical switch.
        \param      edfa            This contains the information about the
                                    state of the erbium doped fiber
                                    amplifier.
        \param      serialNumber    This contains the serial number of the
                                    LPR in the current front end assembly
        \param      configFile      This contains the configuration file name as
                                    extracted from the frontend configuration
                                    file. */
    typedef struct {
        //! SSI 10MHz Enable
        /*! This variable indicates the current communication speed for the
            remote device. Allowed speeds are the following:
                - \ref ENABLE   -> Speed is set to 10 MHz
                - \ref DISABLE  -> Speed is set to 5 MHz */
        unsigned char   ssi10MHzEnable;
        //! Lpr temperature current state
        /*! This is the state of the temperature sensors in the lpr. */
        LPR_TEMP        lprTemp[LPR_TEMP_SENSORS_NUMBER];
        //! Optical switch current state
        /*! Please see \ref OPTICAL_SWITCH for more information. */
        OPTICAL_SWITCH  opticalSwitch;
        //! EDFA current state
        /*! Please see \ref EDFA for more information. */
        EDFA            edfa;
        //! Serial Number
        /*! This contains the serial number of the LPR in the current front end
            assembly. */
        char            serialNumber[SERIAL_NUMBER_SIZE];
        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char            configFile[MAX_FILE_NAME_SIZE];
    } LPR;

    /* Globals */
    /* Externs */
    extern unsigned char currentLprModule; //!< Currently addressed LPR submodule

    /* Prototypes */
    /* Externs */
    extern void lprHandler(void); //!< This function deals with the incoming CAN messages
    extern int lprStartup(void); //!< This function initializes the LPR

#endif /* _LPR_H */
