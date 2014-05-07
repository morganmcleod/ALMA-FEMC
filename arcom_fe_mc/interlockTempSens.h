/*! \file       interlockTempSens.h
    \ingroup    interlockTemp
    \brief      LPR temperature sensor header file

    <b> File informations: </b><br>
    Created: 2011/03/29 14:49:29 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockTempSens.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the information necessary to define the
    characteristics and operate the interlock temperature sensor. */

/*! \defgroup   interlockTempSens     FETIM interlock temperature sensor
    \ingroup    interlockTemp
    \brief      FETIM interlock temperature sensor
    \note       the \ref interlockTempSens module doesn't include any submodule

    For more information on this module see \ref interlockTempSens.h */

#ifndef _INTERLOCK_TEMP_SENS_H
    #define _INTERLOCK_TEMP_SENS_H

    /* Extra includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodule definitions */
    #define INTERLOCK_TEMP_SENS_MODULES_NUMBER     1   // It's just the temperature

    /* Typedefs */
    //! Current state of the FETIM interlock temperature sensors
    /*! This structure represent the current state of the FETIM interlock
        temperature sensor.
        \ingroup    interlockTemp
        \param      temp[Op]    This contains the most recent read-back value
                                for the temperature */
    typedef struct {
        //! FETIM interlock temperature sensors temeprature
        /*! This is the temperature (in C) as registered by the sensor. */
        float   temp[OPERATION_ARRAY_SIZE];
    } INTRLK_TEMP_SENS;

    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockTempSensModule; //!< Currently addressed FETIM interlock temp sens module

    /* Prototypes */
    /* Statics */
    static void tempHandler(void);
    /* Externs */
    extern void interlockTempSensHandler(void); //!< This function deals with teh incoming CAN message

#endif /* _INTERLOCK_TEMP_SENS_H */
