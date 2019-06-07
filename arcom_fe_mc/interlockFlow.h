/*! \file       interlockFlow.h
    \ingroup    interlockSensors
    \brief      FETIM interlock flow sensors header file

    <b> File information: </b><br>
    Created: 2011/03/29 17:51:00 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the FETIM interlock flow sensors. */

/*! \defgroup   interlockFlow        FETIM interlock flow sensors
    \ingroup    interlockSensors
    \brief      FETIM interlock flow sensors

    This group includes all the different \ref interlockFlow submodules. For
    more information on the \ref interlockFlow module see
    \ref interlockFlow.h */

#ifndef _INTERLOCK_FLOW_H
    #define _INTERLOCK_FLOW_H

    /* Extra includes */
    /* Interlock flow sensors defines */
    #ifndef _INTERLOCK_FLOW_SENS_H
        #include "interlockFlowSens.h"
    #endif /* _INTERLOCK_Flow_SENS_H */

    /* Defines */
    #define INTERLOCK_FLOW_SENSORS_NUMBER   2 //!< Number of flow sensors in the FETIM interlock

    /* Submodule definitions */
    #define INTERLOCK_FLOW_MODULES_NUMBER        2      // See list below
    #define INTERLOCK_FLOW_MODULES_RCA_MASK      0x0004 /* Mask to extract the submodule number:
                                                           0-1 -> interlockFlowSensorHandler */
    #define INTERLOCK_FLOW_MODULES_MASK_SHIFT    2      // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM interlock flow sensors */
    /*! This structure reperesent the current state of the FETIM interlock
        flow sensor system
        \ingroup    interlock
        \param      intrlkFlowSens[Tf]  This contains the information about the
                                        FETIM interlock flow sensors.
                                        There are \ref INTERLOCK_FLOW_SENSORS_NUMBER
                                        sensors in each \ref FETIM. */
    typedef struct {
        //! FETIM interlock flow sensors
        /*! This is the state of the flow sensors in the FETIM interlock */
        INTRLK_FLOW_SENS   intrlkFlowSens[INTERLOCK_FLOW_SENSORS_NUMBER];
    } INTRLK_FLOW;


    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockFlowModule; //!< Currently addressed interlock flow module

    /* Prototypes */
    /* Externs */
    extern void interlockFlowHandler(void); //!< This function deals with the incoming CAN messages

#endif /* _INTERLOCK_FLOW_H */
