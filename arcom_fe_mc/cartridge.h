/*! \file       cartridge.h
    \ingroup    frontend
    \brief      Cartridge header file
    \todo       Find a way to write down the mapping of the temperatures so that
                it is spell out one way or another in the manual. If only one
                cartridge (Band3) is different from the others than maybe it is
                possible to spell out the table, otherwise just write something
                and refer to wither the .c or the .h file that contains the
                mapping.

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cartridge.h,v 1.26 2011/03/24 13:34:10 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate each cartridge in the frontend system.
    See \ref cartridge for more information. */

/*! \defgroup   cartridge   Cartridge
    \ingroup    frontend
    \brief      Cartridge module

    This group includes all the different \ref cartridge submodules. For more
    information on the \ref cartridge module see \ref cartridge.h */

#ifndef _CARTRIDGE_H
    #define _CARTRIDGE_H

    /* Extra includes */
    /* POLARIZATION defines */
    #ifndef _POLARIZATION_H
        #include "polarization.h"
    #endif  /* _POLARIZATION_H */

    /* LO module defines */
    #ifndef _LO_H
        #include "lo.h"
    #endif /* _LO_H */

    /* Cartridge temperature sensors */
    #ifndef _CARTRIDGETEMP_H
        #include "cartridgeTemp.h"
    #endif /* _CARTRIDGETEMP_H */

    /* Global Definitions */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    /* Configuration data info */
    #define POL_BASE                "P"                             // Base for polarization section name
    #define POL_AVAIL_SECT(Po)      buildString(POL_BASE,Po,NULL)   // Section containing the polarization availability info
    #define POL_AVAIL_KEY           "AVAILABLE"                     // Key containing the polarization availability info
    #define POL_AVAIL_EXPECTED      1                               // Expedted keys in the section containing the polarization availability info

    #define LNA_LED_BASE(Po)        buildString(POL_BASE,Po,"_LNA_LED") // Base for LNA LED section name
    #define LNA_LED_AVAIL_SECT(Po)  LNA_LED_BASE(Po)                    // Section containing the LNA LED availability info
    #define LNA_LED_AVAIL_KEY       "AVAILABLE"                         // Key containing the LNA LED availability info
    #define LNA_LED_AVAIL_EXPECTED  1                                   // Expected keys in the section containing the LNA LED availability info

    #define SIS_HEATER_BASE(Po)         buildString(POL_BASE,Po,"_SIS_HEATER")  // Base for SIS heater section name
    #define SIS_HEATER_AVAIL_SECT(Po)   SIS_HEATER_BASE(Po)                     // Section containing the SIS heater availability info
    #define SIS_HEATER_AVAIL_KEY        "AVAILABLE"                             // Key containing the SIS heater availability info
    #define SIS_HEATER_AVAIL_EXPECTED   1                                       // Expected keys in the section containing the SIS heater availability info

    #define SCHOTTKY_BASE(Po)       buildString(POL_BASE,Po,"_SCHOTTKY")    // Base for Schottky mixer heater section name
    #define SCHOTTKY_AVAIL_SECT(Po) SCHOTTKY_BASE(Po)                       // Section containing the Schottky mixer availability info
    #define SCHOTTKY_AVAIL_KEY      "AVAILABLE"                             // Key containing the Schottky mixer availability info
    #define SCHOTTKY_AVAIL_EXPECTED 1                                       // Expected keys in the section containing the Schottky mixer availability info

    #define SB_BASE(Po)             buildString(POL_BASE,Po,"_S")       // Base for sideband section name
    #define SB_AVAIL_SECT(Po,Sb)    buildString(SB_BASE(Po),Sb,NULL)    // Section containing the sideband availability info
    #define SB_AVAIL_KEY            "AVAILABLE"                         // Key containing the sideband availability info
    #define SB_AVAIL_EXPECTED       1                                   // Expedted keys in the section containing the sideband availability info

    #define SIS_BASE(Po,Sb)         buildString(SB_BASE(Po),Sb,"_SIS")  // Base for SIS section name
    #define SIS_AVAIL_SECT(Po,Sb)   SIS_BASE(Po,Sb)                     // Section containing the SIS availability info
    #define SIS_AVAIL_KEY           "AVAILABLE"                         // Key containing the SIS availability info
    #define SIS_AVAIL_EXPECTED      1                                   // Expected keys in the section containing the SIS availability info

    #define SIS_MAG_BASE(Po,Sb)         buildString(SB_BASE(Po),Sb,"_SIS")  // Base for SIS magnet section name
    #define SIS_MAG_AVAIL_SECT(Po,Sb)   SIS_MAG_BASE(Po,Sb)                 // Section containing the SIS magnet availability info
    #define SIS_MAG_AVAIL_KEY           "AVAILABLE"                         // Key containing the SIS magnet availability info
    #define SIS_MAG_AVAIL_EXPECTED      1                                   // Expected keys in the section containing the SIS magnet availability info

    #define LNA_BASE(Po,Sb)         buildString(SB_BASE(Po),Sb,"_LNA")  // Base for LNA section name
    #define LNA_AVAIL_SECT(Po,Sb)   LNA_BASE(Po,Sb)                     // Section containing the LNA availability info
    #define LNA_AVAIL_KEY           "AVAILABLE"                         // Key containing the LNA availability info
    #define LNA_AVAIL_EXPECTED      1                                   // Expected keys in the section containing the LNA availability info

    #define LNA_STAGE_BASE(Po,Sb)           buildString(SB_BASE(Po),Sb,"_LNA_S")        // Base for LNA stage section name
    #define LNA_STAGE_AVAIL_SECT(Po,Sb,St)  buildString(LNA_STAGE_BASE(Po,Sb),St,NULL)  // Section containing the LNA STAGE availability info
    #define LNA_STAGE_AVAIL_KEY             "AVAILABLE"                                 // Key containing the LNA STAGE availability info
    #define LNA_STAGE_AVAIL_EXPECTED        1                                           // Expected keys in the section containing the LNA STAGE availability info

    #define CARTRIDGE_ESN_SECTION   "INFO"  // Section containing the cartridge serial number
    #define CARTRIDGE_ESN_KEY       "ESN"   // Key containing the cartridge serial number
    #define CARTRIDGE_ESN_EXPECTED  1       // Expected keys containing the cartridge serial number

    #define RESISTOR_VALUE_SECTION  "RESISTOR"  // Section containing the resistor value
    #define RESISTOR_VALUE_KEY      "VALUE"     // Key containing the resistor value
    #define RESISTOR_VALUE_EXPECTED 1           // Expected keys containing the resistor value

    #define SENSOR_SEC_NAME_SIZE    20  // Number of character needed to describe the section names for the cartridge temperature sensors

    #define SENSOR_AVAIL_SECTION(Se)    sensors[Se]     // Section containing the availability for the temperature sensor
    #define SENSOR_AVAIL_KEY            "AVAILABLE"     // Key containing the availability fro the temperature sensor
    #define SENSOR_AVAIL_EXPECTED       1               // Expected keys containing the availability for the temperature sensor

    #define SENSOR_OFFSET_SECTION(Se)   sensors[Se]     // Section containing the offsets for the temperature sensors
    #define SENSOR_OFFSET_KEY           "OFFSET"        // Key containing the offsets for the temperature sensors
    #define SENSOR_OFFSET_EXPECTED      1               // Expected keys containing the offsets for the temperature sensors



    /* Numbering definitions */
    #define CARTRIDGES_NUMBER   0x0A //!< Number of cartridges per receiver
    #define CARTRIDGE0  0x00
    #define BAND1   CARTRIDGE0  //!< 0: Band 01
    #define CARTRIDGE1  0x01
    #define BAND2   CARTRIDGE1  //!< 1: Band 02
    #define CARTRIDGE2  0x02
    #define BAND3   CARTRIDGE2  //!< 2: Band 03
    #define CARTRIDGE3  0x03
    #define BAND4   CARTRIDGE3  //!< 3: Band 04
    #define CARTRIDGE4  0x04
    #define BAND5   CARTRIDGE4  //!< 4: Band 05
    #define CARTRIDGE5  0x05
    #define BAND6   CARTRIDGE5  //!< 5: Band 06
    #define CARTRIDGE6  0x06
    #define BAND7   CARTRIDGE6  //!< 6: Band 07
    #define CARTRIDGE7  0x07
    #define BAND8   CARTRIDGE7  //!< 7: Band 08
    #define CARTRIDGE8  0x08
    #define BAND9   CARTRIDGE8  //!< 8: Band 09
    #define CARTRIDGE9  0x09
    #define BAND10  CARTRIDGE9  //!< 9: Band 10

    /* Cartrdge states */
    #define CARTRIDGE_ERROR     (-1) // Cartridge is in error state: should be turned off
    #define CARTRIDGE_OFF       0    // Cartridge is off
    #define CARTRIDGE_ON        1    // Cartridge is powered but not initialized
    #define CARTRIDGE_READY     2    // Cartridge is ready to be used
    #define CARTRIDGE_OBSERVING 3    // Cartridge is observing
    #define CARTRIDGE_INITING   4    // Cartridge is initializing

    /* Subsystem definition */
    #define CARTRIDGE_SUBSYSTEMS_NUMBER     2       // See the list below
    #define CARTRIDGE_SUBSYSTEM_RCA_MASK    0x00800 /* Mask to extract the subsystem number:
                                                       0 -> BIAS (2 Polarizations)
                                                       1 -> LO + Cartidge temperatures */
    #define CARTRIDGE_SUBSYSTEM_MASK_SHIFT  11      // Bits right shift for the subsystem mask
    #define CARTRIDGE_SUBSYSTEM_BIAS        0
    #define CARTRIDGE_SUBSYSTEM_LO          1

    /* Subsystem's modules definition */
    /* BIAS */
    #define BIAS_MODULES_NUMBER          2       // See list below
    #define BIAS_MODULES_RCA_MASK        0x00400 /* Mask to extract the submodule number:
                                                    0 -> Polarization
                                                    1 -> Polarization */
    #define BIAS_MODULES_MASK_SHIFT      10      // Bits right shift for the submodule mask

    /* LO + Cartridge Temperatures */
    #define LO_TEMP_MODULES_NUMBER      2       // See list below
    #define LO_TEMP_MODULES_RCA_MASK    0x00780 /* Mask to extract the submodule number:
                                                   0 -> LO
                                                   1 -> Cartridge temperature sensors */
    #define LO_TEMP_MODULES_MASK_SHIFT  7       // Bits right shift for the submodule mask

    /* Cartridge Temperatures */
    #define CARTRIDGE_TEMP_SUBSYSTEM_MODULES_NUMBER     6       // See list below
    #define CARTRIDGE_TEMP_SUBSYSTEM_MODULES_RCA_MASK   0x0070 /* Mask to extract the submodule number:
                                                                    0-5 -> cartridgeTempHandler */
    #define CARTRIDGE_TEMP_SUBSYSTEM_MODULES_MASK_SHIFT 4       // Bits right shift of the submodule mask

    /* Typedefs */
    //! Current state of the cartridge
    /*! This structure represent the current state of the cartridge.
        \ingroup    frontend
        \param      available           an unsigned char
        \param      state               an unsigned char
        \param      polarization[Po]    a POLARIZATION
        \param      lo                  a LO
        \param      cartridgeTemp[Te]   a CARTRIDGE_TEMP
        \param      serialNumber        This contains the serial number of the
                                        cartridge
        \param      configFile          This contains the configuration file
                                        name as extracted from the frontend
                                        configuration file. */
    typedef struct {
        //! Cartridge availability
        /*! This field indicates if a cartridge is installed or not in the
            receiver. This will be determined at startup time depending on the
            results of the handshake with the main software. */
        unsigned char   available;
        //! Cartrdige current state
        /*! This field indicates the current state of the cartridge.
            The cartrdige can be in one of the following states:
                - \ref CARTRIDGE_OFF        -> Cartrdige is not powered
                - \ref CARTRIDGE_ON         -> Cartridge is powered but not yet
                                               initialized
                - \ref CARTRIDGE_READY      -> Cartridge is initialized
                - \ref CARTRIDGE_OBSERVING  -> Cartridge is observing
                - \ref CARTRIDGE_INITING    -> Cartridge being initialized
                - \ref CARTRIDGE_ERROR      -> Cartrdige in error state */
        int             state;
        //! Polarization current state
        /*! Polarizations \p Po are assigned according to the following:
                - Po = 0: Polarization 0
                - Po = 1: Polarization 1

            Please see the definition of the \ref POLARIZATION structure for
            more informations.*/
        POLARIZATION    polarization[POLARIZATIONS_NUMBER];
        //! 1st local oscillator current state
        /*! Please see the definition of the \ref LO structures for more
            informations. */
        LO              lo;
        //! Cartridge temperature sensor current state
        /*! Temperature sensors are assigned to different locations in the
            cartridge according the the ICD. */
        CARTRIDGE_TEMP  cartridgeTemp[CARTRIDGE_TEMP_SENSORS_NUMBER];
        //! Serial Number
        /*! This contains the serial number of the currently addressed
            cartridge. */
        char            serialNumber[SERIAL_NUMBER_SIZE];
        //! Configuration File
        /*! This contains the configuration file name as extracted from the
            frontend configuration file. */
        char            configFile[MAX_FILE_NAME_SIZE];
    } CARTRIDGE;

    /* Globals */
    /* Externs */
    extern unsigned char currentCartridgeSubsystem; //!< Current addressed cartridge subsystem
    extern unsigned char currentLoAndTempModule; //!< Current addressed O and cartridge temperature submodule
    extern unsigned char currentCartridgeTempSubsystemModule; //!< Current addressed cartridge temperature submodule
    extern unsigned char currentBiasModule; //!< Current addressed BIAS submodule

    /* Prototypes */
    /* Statics */
    static void loAndTempSubsystemHandler(void);
    static void cartridgeTempSubsystemHandler(void);
    static void biasSubsystemHandler(void);
    static int asyncCartridgeInit(void);
    /* Externs */
    extern int cartridgeStartup(void); //!< This function initializes the selected cartridge during startup
    extern void cartridgeHandler(void); //!< This function deals with the incoming can message
    extern int cartridgeInit(unsigned char cartridge); //!< This function initializes the selected cartrdige at runtime
    extern int cartridgeStop(unsigned char cartridge); //!< Shut down the selected cartrdige
    extern int cartridgeAsync(void); //!< This function deals with the asynchronous operation of a cartrdige

#endif /* _CARTRIDGE_H */
