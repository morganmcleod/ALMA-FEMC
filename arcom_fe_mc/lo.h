/*! \file       lo.h
    \ingroup    cartridge
    \brief      Local oscillator header file

    <b> File information: </b><br>
    Created: 2004/08/24 16:46:19 by avaccari

    This file contains all the information necessary to define the
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

    /* Power Amplifier (PA) */
    #ifndef __TELEDYNEPA_HPA_H
        #include "teledynePa.h"
    #endif  /* _TELEDYNEPA_H */

    /* YIG Tuneable Oscillator (YTO) */
    #ifndef _YTO_H
        #include "yto.h"
    #endif /* _YTO_H */

    /* Defines */
    /* Configuration data info */
    #define LO_PA_SECTION               "PA"                // Section containing configuration for PA
    #define LO_PA_TELEDYNE_KEY          "TELEDYNE"          // Key to specify using the Teledyne MMIC power control method
    #define LO_PA_TELEDYNE_COLL_POL0    "COLLECTORP0"       // Key for pol0 collector voltage to use with Teledyne PA
    #define LO_PA_TELEDYNE_COLL_POL1    "COLLECTORP1"       // Key for pol1 collector voltage to use with Teledyne PA
    #define LO_PA_LIMITS_SECTION        "PA_LIMITS"         // Section containing the PA max safe power limits
    #define LO_PA_LIMITS_ESN_KEY        "ESN"               // Key for the WCA ESN to which the table applies
    #define LO_PA_LIMITS_ESN_EXPECTED   1                   // Expected keys containting ESN
    #define LO_PA_LIMITS_ENTRIES_KEY    "ENTRIES"           // Key containing number of PA limits entries
    #define LO_PA_LIMITS_EXPECTED       1                   // Expected keys containing PA limits
    #define LO_PA_LIMITS_MAX_ENTRIES    256                 // Maximum number of entries allowed
    #define LO_PA_LIMITS_ENTRY_KEY      "ENTRY_%d"          // Key for individual PA limits entries
                                                            // Entries are formatted as <YTO count>,<PAVD0 limit>,<PAVD1 limit>
    /* Submodules definitions */
    #define LO_MODULES_NUMBER       6       // See list below
    #define LO_MODULES_RCA_MASK     0x00070 /* Mask to extract the submodule number:
                                               0 -> YTO
                                               1 -> Photomixer
                                               2 -> PLL
                                               3 -> AMC
                                               4 -> PA
                                               5 -> TeledynePA */
    #define LO_MODULES_MASK_SHIFT   4       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Max safe LO PA power table entry
    /*! This structure represens a row in the LO PA max safe power settings table.
        \ingroup    cartridge
        \param      ytoEndpoint     Endpoint of a YTO tuning range in the table.
                                    Values between 0 and 4095.
        \param      maxVD0          Maximum allowed setting of LO PA VD0 within
                                    the range between this and the next or prev
                                    endpoint.  Values between 0.0 and 2.5.
        \param      maxVD1          Maximum allowed setting of LO PA VD1 within
                                    the range. Values between 0.0 and 2.5. */
    typedef struct {
        //! ytoEndpoint
        /*! Endpoint of a YTO tuning range in the table.
            Values between 0 and 4095. */
        unsigned int ytoEndpoint;
        //! maxVD0
        /* Maximum allowed setting of LO PA VD0 within
           the range between this and the next or prev
           endpoint.  Values between 0.0 and 2.5. */
        float maxVD0;
        //! maxVD1
        /* Maximum allowed setting of LO PA VD1 within
           the range. Values between 0.0 and 2.5. */
        float maxVD1;

    } MAX_SAFE_LO_PA_ENTRY;

    //! Current state of the LO
    /*! This structure represent the current state of the LO. */
    typedef struct {
        //! SSI 10MHz Enable
        /*! This variable indicates the current communication speed for the
            remote device. Allowed speeds are the following:
                - \ref ENABLE   -> Speed is set to 10 MHz
                - \ref DISABLE  -> Speed is set to 5 MHz */
        unsigned char   ssi10MHzEnable;

        //! AMC current state
        /*! Please see the definition of the \ref AMC structure for more
            information. */
        AMC         amc;

        //! PLL current state
        /*! Please see the definition of the \ref PLL structure for more
            information. */
        PLL         pll;

        //! PA current state
        /*! Please see the definition of the \ref PA structure for more
            information. */
        PA          pa;

        //! Photomixer current state
        /*! Please see the definition of the \ref PHOTOMIXER structure for more
            information. */
        PHOTOMIXER  photomixer;

        //! YTO current state
        /*! Please see the definition of the \ref YTO structure for more
            information. */
        YTO         yto;

        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char        configFile[MAX_FILE_NAME_SIZE];
        
        //! ESN from LO PA entries table
        /*! Contains the WCA ESN to which the max safe LO PA table should apply. */
        char        maxSafeLoPaESN[SERIAL_NUMBER_SIZE];

        //! Max safe LO PA entries table size
        //* Size of the max safe LO PA entries table */
        unsigned char maxSafeLoPaTableSize;

        //! Number of entries currently allocated to the max safe LO PA entries table
        //* Number of entries allocated may be greater than maxSafeLoPaTableSize 
        unsigned char allocatedLoPaTableSize;        

        //! Max safe LO PA entries table
        /*! Table of max safe LO PA entries.  See definition above */
        MAX_SAFE_LO_PA_ENTRY *maxSafeLoPaTable;
    } LO;

    /* Globals */
    /* Externs */
    extern unsigned char currentLoModule; //!< Current addressed LO submodule

    /* Prototypes */
    /* Externs */
    extern int loStartup(void); //!< This function initializes the selected lo during startup
    extern int loShutdown(void); //!< Free resources that were used by all LOs at shutdown time
    extern void loHandler(void); //!< This function deals with the incoming can message
    extern int loInit(void); //!< This function initializes the selected LO at runtime

    extern int loZeroPaDrainVoltage(void);  //!< Helper function to set the LO PA drain voltages to zero
    extern int loZeroPAGateVoltage(void);   //!< Helper function to set the LO PA gate voltages to zero
    extern int loZeroYtoCoarseTuning(void); //!< Helper function to set the YTO coarse tuning to zero

    extern int loResetPaLimitsTable(unsigned char band);  //!< Helper function to clear the PA limits table
    extern int loAddPaLimitsEntry(unsigned char band, unsigned char pol, unsigned int ytoTuning, float maxVD);
                                            //!< Helper function to add a PA limits table entry

    extern int printPaLimitsTable(unsigned char band);

    extern int limitSafePaDrainVoltage(unsigned char paModule);
        //!< Limit the CONV_FLOAT value about to be sent to the PA channel.

    extern int limitSafeYtoTuning();
        //!< Prior to YTO tuning, send commands to reduce the LO PA drain voltages.

#endif /* _LO_H */
