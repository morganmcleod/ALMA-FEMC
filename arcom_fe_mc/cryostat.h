/*! \file       cryostat.h
    \ingroup    frontend
    \brief      Cryostat system header file

    <b> File information: </b><br>
    Created: 2004/10/25 15:39:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the cryostat system. */

/*! \defgroup   cryostat  Cryostat system
    \ingroup    frontend
    \brief      Cryostat system module

    This group includes all the different \ref cryostat submodules. For more
    information on the \ref cryostat module see \ref cryostat.h */

#ifndef _CRYOSTAT_H
    #define _CRYOSTAT_H

    /* Extra includes */
    /* CRYOSTATTEMP defines */
    #ifndef _CRYOSTATTEMP_H
        #include "cryostatTemp.h"
    #endif  /* _CRYOSTATTEMP_H */

    /* BACKINGPUMP defines */
    #ifndef _BACKINGPUMP_H
        #include "backingPump.h"
    #endif  /* _BACKINGPUMP_H */

    /* TURBOPUMP defines */
    #ifndef _TURBOPUMP_H
        #include "turboPump.h"
    #endif  /* _TURBOPUMP_H */

    /* GATEVALVE defines */
    #ifndef _GATEVALVE_H
        #include "gateValve.h"
    #endif  /* _GATEVALVE_H */

    /* SOLENOIDVALVE defines */
    #ifndef _SOLENOIDVALVE_H
        #include "solenoidValve.h"
    #endif  /* _SOLENOIDVALVE_H */

    /* VACUUMCONTROLLER defines */
    #ifndef _VACUUMCONTROLLER_H
        #include "vacuumController.h"
    #endif  /* _VACUUMCONTROLLER_H */

    /* GLOBALDEFINITIONS defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif  /* _GLOBALDEFINITIONS_H */

    /* Defines */
    /* Hardware revision definitions */
    #define CRYO_HRDW_REV0              0
    #define CRYO_HRDW_REV1              1
    /* Configuration data info */
    #define CRYO_CONF_FILE_SECTION      "CRYO"  // Section containing the cryostat configuration file info

    #define CRYO_AVAIL_KEY              "AVAILABLE" // Key containing the availability of the cryostat
    #define CRYO_AVAIL_EXPECTED         1

    #define CRYO_CONF_FILE_KEY          "FILE"  // Key containing the cryostat configuration file info
    #define CRYO_CONF_FILE_EXPECTED     1       // Expected keys containing the cryostat configuration file info

    #define CRYO_HOURS_FILE_SECTION      "CRYO_HOURS"  // Section containing the cryostat cold head hours info
    #define CRYO_HOURS_FILE_KEY          "FILE"  // Key containing the cryostat cold head hours info
    #define CRYO_HOURS_FILE_EXPECTED     1       // Expected keys containing the cryostat cold head hours info
    #define CRYO_HOURS_KEY               "HOURS"  // Key containing the cryostat cold head hours info

    #define TVO_SEC_NAME_SIZE   20  // Number of character needed to describe the section names for the TVO sensors

    #define TVO_COEFFS_SECTION(Se)      sensors[Se]     // Section containing the TVO sensor fitting coefficients
    #define TVO_COEFFS_KEY              "TVO_COEFFS"    // Key containing the TVO sensor fitting coefficients
    #define TVO_COEFFS_EXPECTED         1               // Expected keys containing the TVO sensor fitting coefficients in the section

    #define TVO_NO_SECTION(Se)          sensors[Se] // Section containing the TVO sensor number
    #define TVO_NO_KEY                  "TVO_NO"    // Key containing the TVO sensor number
    #define TVO_NO_EXPECTED             1           // Expected keys containing the TVO sensor number

    /* Submodule definitions */
    #define CRYOSTAT_MODULES_NUMBER     0xA0    // See list below
    #define CRYOSTAT_MODULES_RCA_MASK   0x000FC /* Mask to extract the submodule number:
                                                   0-12 -> cryostatTemp
                                                   13   -> backingPump
                                                   14   -> turboPump
                                                   15   -> gateValve
                                                   16   -> solenoidValve
                                                   17   -> vacuumController
                                                   18   -> supplyCurrent230V
                                                   19   -> coldHeadHours
                                                   20..31 unassigned
                                                   0x20...0x9F -> cryostatTVOSpecificCoeffs */
    #define CRYOSTAT_MODULES_MASK_SHIFT 2        // Bits right shift for the submodules mask

    #define CRYOSTAT_MODULES_UNNASIGNED_RANGE_START 20    // start of unassigned range 20..30
    #define CRYOSTAT_MODULES_TVO_RANGE_START        0x20  // Start of range for specific TVO coeffs
    #define CRYOSTAT_MODULES_TVO_RANGE_END          0x9F  // End of range for specific TVO coeffs
    #define CRYOSTAT_TVO_SENSOR_MASK                0x78  // Mask to extract TVO sensor number from currentCryostatModule
    #define CRYOSTAT_TVO_COEFF_MASK                 0x07  // Mask to extract coeff number from currentCryostatModule

    #define CRYOSTAT_TEMP_RANGE_LOW   0.0         // cryostat sensor readings are considered valid if they
    #define CRYOSTAT_TEMP_RANGE_HIGH  350.0       //   are within this range of real-world temperatures.

    #define CRYOSTAT_TEMP_SANITY_CHECK(T) \
        ((T != FLOAT_ERROR) && (T != FLOAT_UNINIT) && \
         (CRYOSTAT_TEMP_RANGE_LOW < T) && (T < CRYOSTAT_TEMP_RANGE_HIGH))
                                                //!< sanity check macro for testing temperature values

    #define CRYOSTAT_TEMP_BELOW_MAX(T, MAX) \
        (CRYOSTAT_TEMP_SANITY_CHECK(T) && (T < MAX))
                                                //!< below threshold and sanity check macro for temps.

    #define CRYOSTAT_LOG_HOURS_THRESHOLD 265.0  // if any cryocooler stage is below this value,
                                                // we log cryostat cooling hours.

    /* Typedefs */
    //! Current state of the cryostat system
    /*! This structure represent the current state of the cryostat system. */
    typedef struct {
        //! Cryostat availability
        /*! This field indicates if a cryostat is installed in the receiver.
            Added for 2.8.6 for use in test sets.
            Previously the cryostat was assumed always present. */
        unsigned char   available;

        //! Current state of the Cryostat
        //! Cryostat M&C board hardware revision level
        /*! This contains the Cryostat M&C board hardware revision level */
        unsigned char   hardwRevision;

        //! Cryostat temperature current state
        /*! Sensors \p Se are assigned according to the following:
                - Se = 00: 4K cryocooler
                - Se = 01: 4K plate near link1
                - Se = 02: 4K plate near link2
                - Se = 03: 4K plate far side1
                - Se = 04: 4K plate far side2
                - Se = 05: 12K cryocooler
                - Se = 06: 12K plate near link
                - Se = 07: 12K plate far side
                - Se = 08: 12K shield top
                - Se = 09: 90K cryocooler
                - Se = 10: 90K plate near link
                - Se = 11: 90K plate far side
                - Se = 12: 90K shield top

            Please see \ref CRYOSTAT_TEMP for more information. */
        CRYOSTAT_TEMP       cryostatTemp[CRYOSTAT_TEMP_SENSORS_NUMBER];
        
        //! Backing pump current state
        /*! Please see \ref BACKING_PUMP for more information. */
        BACKING_PUMP        backingPump;
        
        //! Turbo pump current state
        /*! Please see \ref TURBO_PUMP for more information. */
        TURBO_PUMP          turboPump;
        
        //! Gate valve current state
        /*! Please see \ref GATE_VALVE for more information. */
        GATE_VALVE          gateValve;
        
        //! Solenoid valve current state
        /*! Please see \ref SOLENOID_VALVE for more information. */
        GATE_VALVE          solenoidValve;
        
        //! Current state of the vacuum controller
        /*! Please see \ref VACUUM_CONTROLLER for more information. */
        VACUUM_CONTROLLER   vacuumController;
        
        //! 230V suplly current
        /*! This contains the last monitored value of the current used by the
            230V supply. */
        float               supplyCurrent230V;
       
        //! Cold head operating hours since last reset
        unsigned long       coldHeadHours;

        //! Cold head hours need to be written to NV memory?
        unsigned char       coldHeadHoursDirty;

        //! Last cold head reset message
        LAST_CONTROL_MESSAGE    lastColdHeadHours;

        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char                configFile[MAX_FILE_NAME_SIZE];

        //! Cold head hours File
        char                coldHeadHoursFile[MAX_FILE_NAME_SIZE];
    } CRYOSTAT;

    /* Globals */
    /* Externs */
    extern unsigned char currentCryostatModule; //!< Currently addressed cryostat submodule
    extern unsigned char currentAsyncCryoTempModule; //!< A global to keep track of the cryostat temperature module currently addressed by the async routine
    extern int asyncCryoTempError[CRYOSTAT_TEMP_SENSORS_NUMBER]; //!< A global to keep track of the async error while monitoring cryostat temperatures
    extern unsigned char currentAsyncVacuumControllerModule; //!< A global to keep track of the cryostat pressure module currently addressed by the async routine
    extern int asyncVacuumControllerError[VACUUM_SENSORS_NUMBER]; //!< A global to keep track of the async error while monitoring cryostat pressures
    extern int asyncSupplyCurrent230VError; //!< A global to keep track of the async error while monitoring the cryostat supply voltage current

    /* Prototypes */
    /* Statics */
    static void supplyCurrent230VHandler(void);
    static void coldHeadHoursHandler(void);
    /* Externs */
    extern void cryostatHandler(void); 
    //!< This function deals with the incoming CAN messages
    extern int cryostatStartup(void); 
    //!< This function deals with the initialization of the cryostat
    extern int cryostatAsync(void); 
    //!< This function deals with the asychronous monitoring of the cryostat
    extern int cryostatAsyncLogHours(void);
    //!< Asynchronous recording of cryostat cold head hours.
    extern int cryostatSensorTablesReport(void);    
    //!< Print cryostat sensor tables report
#endif /* _CRYTOSTAT_H */
