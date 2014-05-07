/*! \file       cryostat.h
    \ingroup    frontend
    \brief      Cryostat system header file

    <b> File informations: </b><br>
    Created: 2004/10/25 15:39:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cryostat.h,v 1.20 2008/09/26 23:00:38 avaccari Exp $

    This files contains all the informations necessary to define the
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
    /* Configuration data info */
    #define CRYO_CONF_FILE_SECTION      "CRYO"  // Section containing the cryostat configuration file info
    #define CRYO_CONF_FILE_KEY          "FILE"  // Key containing the cryostat configuration file info
    #define CRYO_CONF_FILE_EXPECTED     1       // Expected keys containing the cryostat configuration file info

    #define TVO_SEC_NAME_SIZE   20  // Number of character needed to describe the section names for the TVO sensors

    #define TVO_COEFFS_SECTION(Se)      sensors[Se]     // Section containing the TVO sensor fitting coefficients
    #define TVO_COEFFS_KEY              "TVO_COEFFS"    // Key containing the TVO sensor fitting coefficients
    #define TVO_COEFFS_EXPECTED         1               // Expected keys containing the TVO sensor fitting coefficients in the section

    #define TVO_NO_SECTION(Se)          sensors[Se] // Section containing the TVO sensor number
    #define TVO_NO_KEY                  "TVO_NO"    // Key containing the TVO sensor number
    #define TVO_NO_EXPECTED             1           // Expected keys containing the TVO sensor number

    #define CRYO_ESN_SECTION            "INFO"  // Section containing the cryostat serial number
    #define CRYO_ESN_KEY                "ESN"   // Key containing the cryostat serial number
    #define CRYO_ESN_EXPECTED           1       // Expected keys containing the cryostat serial number

    /* Submodule definitions */
    #define CRYOSTAT_MODULES_NUMBER     19      // See list below
    #define CRYOSTAT_MODULES_RCA_MASK   0x0007C /* Mask to extract the submodule number:
                                                   0-12 -> cryostatTemp
                                                   13   -> backingPump
                                                   14   -> turboPump
                                                   15   -> gateValve
                                                   16   -> solenoidValve
                                                   17   -> vacuumController
                                                   18   -> supllyCurrent230V*/
    #define CRYOSTAT_MODULES_MASK_SHIFT 2       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the cryostat system
    /*! This structure represent the current state of the cryostat system.
        \ingroup    frontend
        \param      cryostatTemp[Se]        This contains the information about
                                            the cryostat temperature sensors.
                                            There are
                                            \ref CRYOSTAT_TEMP_SENSORS_NUMBER
                                            sensors in each dewar.
        \param      backingPump             This contains the information about
                                            the state of the backing pump.
        \param      turboPump               This contains the information about
                                            the state of the turbo pump.
        \param      gateValve               This contains the information about
                                            the state of the gate valve.
        \param      solenoidValve           This contains the information about
                                            the state of the solenoid valve.
        \param      vacuumController        This contains the information about
                                            the state of the vacuum controller.
        \param      supplyCurrent230V[Op]   This contains the last monitored
                                            value for the 230V supply
                                            current.
        \param      serialNumber            This contains the serial number of
                                            the cryostat in the current front
                                            end assembly
        \param      configFile              This contains the configuration file
                                            name as extracted from the frontend
                                            configuration file. */
    typedef struct {
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

            Please see \ref CRYOSTAT_TEMP for more informations. */
        CRYOSTAT_TEMP       cryostatTemp[CRYOSTAT_TEMP_SENSORS_NUMBER];
        //! Backing pump current state
        /*! Please see \ref BACKING_PUMP for more informations. */
        BACKING_PUMP        backingPump;
        //! Turbo pump current state
        /*! Please see \ref TURBO_PUMP for more informations. */
        TURBO_PUMP          turboPump;
        //! Gate valve current state
        /*! Please see \ref GATE_VALVE for more informations. */
        GATE_VALVE          gateValve;
        //! Solenoid valve current state
        /*! Please see \ref SOLENOID_VALVE for more informations. */
        GATE_VALVE          solenoidValve;
        //! Current state of the vacuum controller
        /*! Please see \ref VACUUM_CONTROLLER for more informations. */
        VACUUM_CONTROLLER   vacuumController;
        //! 230V suplly current
        /*! This contains the last monitored value of the current used by the
            230V supply. */
        float               supplyCurrent230V[OPERATION_ARRAY_SIZE];
        //! Serial Number
        /*! This contains the serial number of the cryostat in the current
            front end assembly. */
        char                serialNumber[SERIAL_NUMBER_SIZE];
        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char                configFile[MAX_FILE_NAME_SIZE];
    } CRYOSTAT;

    /* Globals */
    /* Externs */
    extern unsigned char currentCryostatModule; //!< Currently addressed cryostat submodule

    /* Prototypes */
    /* Statics */
    static void supplyCurrent230VHandler(void);
    /* Externs */
    extern void cryostatHandler(void); //!< This function deals with the incoming CAN messages
    extern int cryostatInit(void); //!< This function deals with the initialization of the cryostat

#endif /* _CRYTOSTAT_H */
