/*! \file       lo.h
    \ingroup    cartridge
    \brief      Local oscillator header file

    <b> File informations: </b><br>
    Created: 2004/08/24 16:46:19 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lo.h,v 1.20 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the local oscillator system in each cartridge.
    See \ref lo for more information. */

/*! \defgroup   lo  Local Oscillator (LO)
    \ingroup    cartridge
    \brief      Local Oscillator module

    This group includes all the different \ref lo submodules. For more
    information on the \ref lo module see \ref lo.h */

#ifndef _LO_H
    #define _LO_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Active Multiplier Chain (AMC) */
    #ifndef _AMC_H
        #include "amc.h"
    #endif /* _AMC_H */

    /* Phase Lock Loop  (PLL) */
    #ifndef _PLL_H
        #include "pll.h"
    #endif /* _PLL_H */

    /* Photomixer */
    #ifndef _PHOTOMIXER_H
        #include "photomixer.h"
    #endif /* _PHOTOMIXER_H */

    /* Power Amplifier (PA) */
    #ifndef _PA_H
        #include "pa.h"
    #endif  /* _PA_H */

    /* YIG Tuneable Oscillator (YTO) */
    #ifndef _YTO_H
        #include "yto.h"
    #endif /* _YTO_H */

    /* Defines */
    /* Configuration data info */
    #define LO_ESN_SECTION   "INFO"  // Section containing the LO serial number
    #define LO_ESN_KEY       "ESN"   // Key containing the LO serial number
    #define LO_ESN_EXPECTED  1       // Expected keys containing the LO serial number

    #define PLL_LOOP_BW_SECTION  "PLL"      // Section containing the PLL loop bandwidth
    #define PLL_LOOP_BW_KEY      "LOOP_BW"  // Key containing the PLL loop bandwidth
    #define PLL_LOOP_BW_EXPECTED 1          // Expected keys containing the PLL loop bandwidth

    #define PLL_LOCK_SECTION    "SCALING"   // Section containing the PLL lock voltage scaling factor
    #define PLL_LOCK_KEY        "PLL_LOCK"  // Key containing the PLL lock voltage scaling factor
    #define PLL_LOCK_EXPECTED   1           // Expected keys containing the PLL lock voltage scaling factor

    #define PLL_CORR_SECTION    "SCALING"   // Section containing the PLL correction voltage scaling factor
    #define PLL_CORR_KEY        "PLL_CORR"  // Key containing the PLL correction voltage scaling factor
    #define PLL_CORR_EXPECTED   1           // Expected keys containing the PLL correction voltage scaling factor

    #define PLL_YIG_C_SCALE_SECTION     "SCALING"           // Section containing the PLL YIG heater current scaling factor
    #define PLL_YIG_C_SCALE_KEY         "PLL_YIG_C_SCALE"   // Key containing the PLL YIG heater current scaling factor
    #define PLL_YIG_C_SCALE_EXPECTED    1                   // Expected keys containing the PLL YIG heater current scaling factor

    #define PLL_YIG_C_OFFSET_SECTION    "SCALING"           // Section containing the PLL YIG heater current scaling factor
    #define PLL_YIG_C_OFFSET_KEY        "PLL_YIG_C_OFFSET"  // Key containing the PLL YIG heater current scaling factor
    #define PLL_YIG_C_OFFSET_EXPECTED   1                   // Expected keys containing the PLL YIG heater current scaling factor

    #define LO_SUPPLY_V_SECTION         "SCALING"           // Section containing the supply voltage scaling factor
    #define LO_SUPPLY_V_KEY             "SUPPLY_V"          // Key containing the supply voltage scaling factor
    #define LO_SUPPLY_V_EXPECTED        1                   // Expected keys containing the supply voltage scaling factor

    #define LO_MULTIPLIER_C_SECTION     "SCALING"           // Section containing the multiplier current scaling factor
    #define LO_MULTIPLIER_C_KEY         "MULT_C"            // Key containing the multiplier current scaling factor
    #define LO_MULTIPLIER_C_EXPECTED    1                   // Expected keys containing the multiplier current scaling factor

    /* Submodules definitions */
    #define LO_MODULES_NUMBER       5       // See list below
    #define LO_MODULES_RCA_MASK     0x00070 /* Mask to extract the submodule number:
                                               0 -> YTO
                                               1 -> Photomixer
                                               2 -> PLL
                                               3 -> AMC
                                               4 -> PA */
    #define LO_MODULES_MASK_SHIFT   4       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the LO
    /*! This structure represent the current state of the LO.
        \ingroup    cartridge
        \param      ssi10MHzEnable  This contains the current state of the 10MHz
                                    speed at the remote SSI interface:
                                        - \ref ENABLE   -> Speed is set to 10MHz
                                        - \ref DISABLE  -> Speed is set to 5MHz
        \param      amc             This contains the information about the AMC
                                    available for this LO. For more information
                                    refer to \ref AMC.
        \param      pll             This contains the information about the PLL
                                    available for this LO. For more information
                                    refer to \ref PLL.
        \param      photomixer      This contains the information about the
                                    PHOTOMIXER available for this LO. For more
                                    information refer to \ref PHOTOMIXER.
        \param      yto             This contains the information about the YTO
                                    available for this LO. For more information
                                    refer to \ref YTO
        \param      serialNumber    This contains the serial number of the LO.
        \param      configFile[MAX_FILE_NAME_SIZE]  This contains the
                                                    configuration file name as
                                                    extracted from the frontend
                                                    configuration file.
        \param      supplyVoltagesScale This contains the scale factor for all
                                        the supply voltages in the LO.
        \param      multiplierCurrentsScale This contains the scale factor for
                                            all the multiplier currents in the
                                            LO. */
    typedef struct {
        //! SSI 10MHz Enable
        /*! This variable indicates the current communication speed for the
            remote device. Allowed speeds are the following:
                - \ref ENABLE   -> Speed is set to 10 MHz
                - \ref DISABLE  -> Speed is set to 5 MHz */
        unsigned char   ssi10MHzEnable;
        //! MC current state
        /*! Please see the definition of the \ref AMC structure for more
            informations. */
        AMC         amc;
        //! LL current state
        /*! Please see the definition of the \ref PLL structure for more
            informations. */
        PLL         pll;
        //! A current state
        /*! Please see the definition of the \ref PA structure for more
            informations. */
        PA          pa;
        //! Photomixer current state
        /*! Please see the definition of the \ref PHOTOMIXER structure for more
            informations. */
        PHOTOMIXER  photomixer;
        //! YTO current state
        /*! Please see the definition of the \ref YTO structure for more
            informations. */
        YTO         yto;
        //! Serial Number
        /*! This contains the serial number of the currently addressed LO. */
        char        serialNumber[SERIAL_NUMBER_SIZE];
        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char        configFile[MAX_FILE_NAME_SIZE];
        //! Supply voltage scale facotr
        /*! This contains the scale factor of all the supply voltages monitored
            in the LO. */
        float       supplyVoltagesScale;
        //! Multiplier currents scale facotr
        /*! This contains the scale factor of all the multiplier currents
            monitored in the LO. */
        float       multiplierCurrentsScale;
    } LO;

    /* Globals */
    /* Externs */
    extern unsigned char currentLoModule; //!< Current addressed LO submodule

    /* Prototypes */
    /* Externs */
    extern int loStartup(void); //!< This function initializes the selected lo during startup
    extern void loHandler(void); //!< This function deals with the incoming can message
    extern int loInit(void); //!< This function initializes the selected LO at runtime

#endif /* _LO_H */
