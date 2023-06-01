/*! \file       lpr.h
    \ingroup    lpr
    \brief      LO Photonic Receiver header file

    <b> File information: </b><br>
    Created: 2007/05/29 14:49:12 by avaccari

    This file contains all the information necessary to define the
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
    #define LPR2_ENABLE_KEY         "LPR2"  // If true enable LPR2 for the BELO test set


    /* Submodule definitions */
    #define LPR_MODULES_NUMBER      4       // See list below
    #define LPR_MODULES_RCA_MASK    0x00030 /* Mask to extract the submodule number:
                                               0-1  -> lprTemp
                                               2    -> opticalSwitch
                                               3    -> edfa */
    #define LPR_MODULES_MASK_SHIFT  4       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the LPR system
    /*! This structure represent the current state of the LPR system */
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
    extern int lprStop(void); //!< This function shuts down the LPR

#endif /* _LPR_H */
