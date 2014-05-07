/*! \file       interlockTemp.h
    \ingroup    interlockSensors
    \brief      FETIM interlock temperature sensors header file

    <b> File informations: </b><br>
    Created: 2011/03/29 17:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockTemp.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM interlock temperature sensors. */

/*! \defgroup   interlockTemp        FETIM interlock temperature sensors
    \ingroup    interlockSensors
    \brief      FETIM interlock temperature sensors

    This group includes all the different \ref interlockTemp submodules. For
    more information on the \ref interlockTemp module see
    \ref interlockTemp.h */

#ifndef _INTERLOCK_TEMP_H
    #define _INTERLOCK_TEMP_H

    /* Extra includes */
    /* Interlock temperature sensors defines */
    #ifndef _INTERLOCK_TEMP_SENS_H
        #include "interlockTempSens.h"
    #endif /* _INTERLOCK_TEMP_SENS_H */

    /* Defines */
    #define INTERLOCK_TEMP_SENSORS_NUMBER   5   //!< Number of temperature sensors in the FETIM interlock

    /* Submodule definitions */
    #define INTERLOCK_TEMP_MODULES_NUMBER        5      // See list below
    #define INTERLOCK_TEMP_MODULES_RCA_MASK      0x0007 /* Mask to extract the submodule number:
                                                           0-4 -> interlockTempSensorHandler */
    #define INTERLOCK_TEMP_MODULES_MASK_SHIFT    0      // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM interlock temperature sensors */
    /*! This structure reperesent the current state of the FETIM interlock
        temperature sensor system
        \ingroup    interlock
        \param      intrlkTempSens[Tf]  This contains the information about the
                                        FETIM interlock temperature sensors.
                                        There are \ref INTERLOCK_TEMP_SENSORS_NUMBER
                                        sensors in each \ref FETIM. */
    typedef struct {
        //! FETIM interlock temperature sensors
        /*! This is the state of the temperature sensors in the FETIM interlock */
        INTRLK_TEMP_SENS   intrlkTempSens[INTERLOCK_TEMP_SENSORS_NUMBER];
    } INTRLK_TEMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockTempModule; //!< Currently addressed interlock temeprature module

    /* Prototypes */
    /* Externs */
    extern void interlockTempHandler(void); //!< This function deals with the incoming CAN messages

#endif /* _INTERLOCK_TEMP_H */
