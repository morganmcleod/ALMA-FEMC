/*! \file       frontend.h
    \brief      Frontend header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the entire frontend system.
    See \ref frontend for more information. */

/*! \defgroup   frontend    Frontend
    \brief      Frontend    module

    This group includes all the different \ref frontend submodules. For more
    information on the \ref frontend module see \ref frontend.h */

#ifndef _FRONTEND_H
    #define _FRONTEND_H

    /* Defines */
    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* CARTRIDGE defines */
    #ifndef _CARTRIDGE_H
        #include "cartridge.h"
    #endif /* _CARTRIDGE_H */

    /* POWER DISTRIBUTION defines */
    #ifndef _POWERDISTRIBUTION_H
        #include "powerDistribution.h"
    #endif /* _POWERDISTRIBUTION_H */

    /* IF SWITCH defines */
    #ifndef _IFSWITCH_H
        #include "ifSwitch.h"
    #endif /* _IFSWITCH_H */

    /* CRYOSTAT SYSTEM defines */
    #ifndef _CRYOSTAT_H
        #include "cryostat.h"
    #endif /* _CRYOSTAT_H */

    /* LPR defines */
    #ifndef _LPR_H
        #include "lpr.h"
    #endif /* _LPR_H */

    /* FETIM defines */
    #ifndef _FETIM_H
        #include "fetim.h"
    #endif /* _FETIM_H */


    /* Defines */
    /* Configuration defines */
    #define FRONTEND_CONF_FILE    "frontend.ini"    // File containing the frontend configuration info

    #define BAND_SECT_BASE  "BAND"
    #define BAND_SECT(Ca)   buildString(BAND_SECT_BASE,Ca,NULL)

    #define BAND_AVAIL_SECTION(Ca)  BAND_SECT(Ca+1) // Section containing the availability of the band
    #define BAND_AVAIL_KEY          "AVAILABLE"     // Key containing the availability of the band
    #define BAND_AVAIL_EXPECTED     1               // Expected keys contining the availability of the band

    #define CART_SECT_BASE  "CART"
    #define CART_SECT(Ca)   buildString(CART_SECT_BASE,Ca,NULL)

    #define CART_FILE_SECTION(Ca)   CART_SECT(Ca+1) // Section containing the configuration file name for the cartridge
    #define CART_FILE_KEY           "FILE"          // Key containing the configuration file name for the cartridge
    #define CART_FILE_EXPECTED      1               // Expected keys containing the configuration file name for the cartridge

    #define WCA_SECT_BASE   "WCA"
    #define WCA_SECT(Ca)    buildString(WCA_SECT_BASE,Ca,NULL)

    #define WCA_FILE_SECTION(Ca)    WCA_SECT(Ca+1)  // Section containng the configuration file name for the wca
    #define WCA_FILE_KEY            "FILE"          // Key containing the configuration file name for the wca
    #define WCA_FILE_EXPECTED       1               // Expected keys containing the configuration file name

    /* Operation mode defines */
    #define OPERATIONAL_MODE        0
    #define TROUBLESHOOTING_MODE    1
    #define MAINTENANCE_MODE        2
    #define SIMULATION_MODE         3

    /* Typedefs */
    //! Current state of the frontend
    typedef struct {
        //! Frontend current state
        /*! The receiver can be in one of the following modes:
                - \ref OPERATIONAL_MODE     -> The receiver is in standard
                                               operation mode. This will prevent
                                               certain operation from being
                                               executed.
                - \ref TROUBLESHOOTING_MODE -> The receiver is in
                                               troubleshooting mode.
                                               This will allow to perform all
                                               possible operation with the
                                               receiver.
                - \ref MAINTENANCE_MODE     -> The receiver is in maintenance
                                               mode. In this mode only operation
                                               allowed by special CAN addressed
                                               are allowed.
                - \ref SIMULATION_MODE      -> All requests will be processed but
                                               no access to front end hardware.
                                               Simulated monitor data is returned. */
        unsigned char       mode;

        /*! Ethernet IP address */
        unsigned char ipaddress[4];

        //! Cartridge current state
        /*! Cartridges \p Ca are assigned according to the following:
                - Ca = 0: Band 01
                - Ca = 1: Band 02
                - Ca = 2: Band 03
                - Ca = 3: Band 04
                - Ca = 4: Band 05
                - Ca = 5: Band 06
                - Ca = 6: Band 07
                - Ca = 7: Band 08
                - Ca = 8: Band 09
                - Ca = 9: Band 10

            Please see \ref CARTRIDGE for more information. */
        CARTRIDGE           cartridge[CARTRIDGES_NUMBER];
        //! Power distibution system current state
        /*! Please see \ref POWER_DISTRIBUTION for more information. */
        POWER_DISTRIBUTION  powerDistribution;
        //! IF switch current state
        /*! Please see \ref IF_SWITCH for more information. */
        IF_SWITCH           ifSwitch;
        //! Cryostat system current state
        /*! Please see \ref CRYOSTAT for more information. */
        CRYOSTAT            cryostat;
        //! LPR current state
        /*! Please see \ref LPR for more information. */
        LPR                 lpr;
        //! FETIM current state
        /*! Please see \ref FETIM for more information. */
        FETIM               fetim;
    } FRONTEND;

    /* Globals */
    /* Externs */
    extern FRONTEND frontend; //!< Current status of the frontend

    /* Prototypes */
    /* Externs */
    extern int frontendInit(void); //!< This function initializes the frontend
    extern int frontendStop(void); //!< This function stops the frontend
    extern int frontendInitIPAddress(void);  //!< Initialize the ipaddress from SOCKETS
    extern int frontendWriteNVMemory(void);  //!< Implement SET_WRITE_NV_MEMORY
    extern int feAndCartridgesConfigurationReport(void);  //!< Print FE and cartridges configuration report
    extern int loPaLimitsTablesReport(void);        //!< Print LO PA_LIMITS tables report

#endif // _FRONTEND_H
