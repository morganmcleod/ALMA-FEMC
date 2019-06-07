/*! \file       solenoidValve.h
    \ingroup    cryostat
    \brief      Solenoid valve header file

    <b> File information: </b><br>
    Created: 2007/03/13 11:16:23 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the solenoid valve. */

/*! \defgroup   solenoidValve  Solenoid valve
    \ingroup    cryostat
    \brief      Solenoid valve module
    \note       The \ref solenoidValve module doesn't include any submodule.

    For more information on this module see \ref solenoidValve.h */

#ifndef _SOLENOIDVALVE_H
    #define _SOLENOIDVALVE_H

    /* Extra Includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodules definitions */
    #define SOLENOID_VALVE_MODULES_NUMBER   1   // It's just the stateHandler

    /* Typedefs */
    //! Current state of the gate valve
    /*! This structure represent the current state of the solenoid valve system.
        \ingroup    cryostat
        \param      state   This contains the current state of the solenoid
                                valve:
                                    - \ref SOLENOID_VALVE_OPEN -> Valve is open
                                    - \ref SOLENOID_VALVE_CLOSE -> Valve is
                                      close
                                    - \ref SOLENOID_VALVE_UNKNOWN -> Valve is in
                                      an unknown state.
        \param      lastState   This contains a copy of the last issued control
                                message to the state of the solenoid valve. */
    typedef struct {
        //! Solenoid valve state
        /*! This is the solenoid valve state as monitored through two limit
            switch.
            The possible states are:
                - \ref SOLENOID_VALVE_OPEN -> Valve is open
                - \ref SOLENOID_VALVE_CLOSE -> Valve is close
                - \ref SOLENOID_VALVE_UNKNOWN -> Valve is in an unknown
                  state. */
        unsigned char   state;
        //! Last control message: solenoid valve state
        /*! This is the content of the last control message sent to the solenoid
            valve state. */
        LAST_CONTROL_MESSAGE    lastState;
    } SOLENOID_VALVE;

    /* Globals */
    /* Externs */
    extern unsigned char currentSolenoidValveModule; //!< Currently addressed solenoid valve submodule

    /* Prototypes */
    /* Statics */
    static void stateHandler(void);
    /* Externs */
    extern void solenoidValveHandler(void); //!< This function deals with the incoming CAN message

#endif /* _SOLENOIDVALVE_H */
