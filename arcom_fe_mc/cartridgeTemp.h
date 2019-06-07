/*! \file       cartridgeTemp.h
    \ingroup    cartridge
    \brief      Cartridge temperature sensor header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the cartridge temperature sensor . */

/*! \defgroup   cartridgeTemp   Cartridge temperature sensor
    \ingroup    cartridge
    \brief      Cartridge temperature sensor
    \note       The \ref cartridgeTemp module doesn't include any submodule.

    For more information on this module see \ref cartridgeTemp.h */

#ifndef _CARTRIDGETEMP_H
    #define _CARTRIDGETEMP_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* POLARIZATION defines */
    #ifndef _POLARIZATION_H
        #include "polarization.h"
    #endif /* _POLARIZATION_H */

    /* Defines */
    #define CARTRIDGE_TEMP_SENSORS_NUMBER   6       //!< Number of cartridge temperature sensors per cartridge

    /* Definition necessary for the mapping */
    #define SENSOR0     0
    #define SENSOR1     1
    #define SENSOR2     2
    #define P0          POLARIZATION0
    #define P1          POLARIZATION1
    #define S0          SENSOR0
    #define S1          SENSOR1
    #define S2          SENSOR2

    /* Submodules definitions */
    #define CARTRIDGE_TEMP_MODULES_NUMBER       2       // See list below
    #define CARTRIDGE_TEMP_MODULES_RCA_MASK     0x0008  /* Mask to extract the submodule number:
                                                           0 -> Sensor temperature
                                                           1 -> Sensor offset */
    #define CARTRIDGE_TEMP_MODULES_MASK_SHIFT   3       // Bits right shift of the submodule mask

    /* Typedefs */
    //! Current state of the cartridge temperature sensor
    typedef struct {
        //! Cartridge temperature sensor temperature
        /*! This is the temperature (in K) of the cartridge temperature
            sensor. */
        float                   temp;
        //! Cartridge temperature sensor offset
        /*! This is the offset (in K) respect to the standard calibration
            curve which is applied to all the sensors in the cartrdige. */
        float                   offset;
        //! Last control message: set offset
        LAST_CONTROL_MESSAGE    lastOffset;
    } CARTRIDGE_TEMP;


    /* A structure to perform the mapping of the sensors */
    typedef struct{
        unsigned char   polarization;
        unsigned char   sensorNumber;
    } TEMP_SENSOR;


    /* Globals */
    /* Externs */
    extern unsigned char currentCartridgeTempModule; //!< Currently addressed cartridge temperature submodule

    /* Prototypes */
    /* Statics */
    static void tempOffsetHandler(void); //!< This function deals with the incoming can message
    static void tempHandler(void);
    /* Externs */
    extern void cartridgeTempHandler(void); //!< This function deals with the incoming can message

#endif /* _CARTRIDGETEMP_H */
