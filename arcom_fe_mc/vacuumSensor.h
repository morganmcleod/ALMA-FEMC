/*! \file       vacuumSensor.h
    \ingroup    vacuumController
    \brief      Vacuum sensors header file

    <b> File information: </b><br>
    Created: 2007/04/06 14:25:15 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the dewar vacuum sensors. */

/*! \defgroup   vacuumSensor  Dewar vacuum sensor
    \ingroup    vacuumController
    \brief      Cryostat vacuum sensor module
    \note       The \ref vacuumSensor module doesn't include any submodule.

    For more information on this module see \ref vacuumSensor.h */

#ifndef _VACUUMSENSOR_H
    #define _VACUUMSENSOR_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define VACUUM_SENSORS_NUMBER       2       //!< Number of vacuum sensors in the dewar
    #define CRYOSTAT_PRESSURE           0x0     //!< 00: Cryostat Pressure
    #define VACUUM_PORT_PRESSURE        0x1     //!< 01: Vacuum Port Pressure
    #define MAX_CRYO_COOLING_PRESSURE   0.0005  //!< Top pressure limit to allow cooling down

    /* Submodule definistions */
    #define VACUUM_SENSOR_MODULES_NUMBER        1   // It's just the pressure

    /* Typedefs */
    //! Current state of the dewar temperature sensors
    /*! This structure represent the current state of the dewar temperature
        system.
        \ingroup    vacuumSensor
        \param      pressure    This contains the most recent read-back
                                    value for the pressure */
    typedef struct {
        //! Vacuum Sensor Pressure
        /*! This is the pressure (in mbar) as registered by the sensor. */
        float   pressure;
    } VACUUM_SENSOR;

    /* Globals */
    /* Externs */
    extern unsigned char currentVacuumSensorModule; //!< Currently addressed vacuum sensor submodule

    /* Prototypes */
    /* Statics */
    static void pressureHandler(void);
    /* Externs */
    extern void vacuumSensorHandler(void); //!< This function deals with the incoming CAN message


#endif /* _VACUUMSENSOR_H */
