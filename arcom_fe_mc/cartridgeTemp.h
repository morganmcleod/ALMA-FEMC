/*! \file       cartridgeTemp.h
    \ingroup    cartridge
    \brief      Cartridge temperature sensor header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cartridgeTemp.h,v 1.17 2007/06/01 20:55:10 avaccari Exp $

    This files contains all the informations necessary to define the
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
    #define CARTRIDGE_TEMP_MODULES_NUMBER       1   // It's just the temperature



    /* Typedefs */
    //! Current state of the cartridge temperature sensor
    /*! This structure represent the current state of the cartridge temperature sensor.
        \ingroup    cartridge
        \param      available   This indicates the availability of the sensor:
                                    - 0 -> Unavailable
                                    - 1 -> Available
        \param      temp[Op]    This contains the most recent read-back value
                                for the temperature
        \param      offset      This contains the offset value to be applied to
                                the readback from the sensor */
    typedef struct {
        //! Cartridge temperature sensor availability
        /*! This variable indicates if the cartridge is outfitted with this
            particular temperature sensor. This value should be part of the
            device dependent informations retrived from the configuration
            database. */
        unsigned char           available;
        //! Cartridge temperature sensor temperature
        /*! This is the temperature (in K) of the cartridge temperature
            sensor. */
        float                   temp[OPERATION_ARRAY_SIZE];
        //! Cartridge temperature sensor offset
        /*! This is the offset (in K) respect to the standard calibration
            curve which is applied to all the sensors in the cartrdige. */
        float                   offset;
        //! Last control message: set offset
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
    static void tempHandler(void);
    /* Externs */
    extern void cartridgeTempHandler(void); //!< This function deals with the incoming can message

#endif /* _CARTRIDGETEMP_H */
