/*! \file       interlock.h
    \ingroup    fetim
    \brief      FETIM interlock header file

    <b> File informations: </b><br>
    Created: 2011/03/25 17:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlock.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM interlock. */

/*! \defgroup   interlock       FETIM interlock
    \ingroup    fetim
    \brief      FETIM interlock system

    This group includes all the different \ref interlock submodules. For more
    information on the \ref interlock module see \ref interlock.h */

#ifndef _INTERLOCK_H
    #define _INTERLOCK_H

    /* Extra includes */
    /* Interlock sensors defines */
    #ifndef _INTERLOCK_SENSORS_H
        #include "interlockSensors.h"
    #endif /* _INTERLOCK_SENSORS_H */

    /* Interlock state defines */
    #ifndef _INTERLOCK_STATE_H
        #include "interlockState.h"
    #endif /* _INTERLOCK_STATE_H */

    /* Submodule definitions */
    #define INTERLOCK_MODULES_NUMBER        2       // See list below
    #define INTERLOCK_MODULES_RCA_MASK      0x00020 /* Mask to extract the submodule number:
                                                       0 -> interlockSensorsHandler
                                                       1 -> interlockStateHandler */
    #define INTERLOCK_MODULES_MASK_SHIFT    5       // Bits right shift for the submodule mask

    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockModule; //!< Currently addressed interlock module

    /* Typedefs */
    //! Current state of the FETIM interlock system
    /*! This structure represent the current state of the FETIM interlock system
        \ingroup    fetim
        \param      sensors     This contains the information about the state of
                                the interlock sensors.
        \param      state       This contains the information about the state of
                                the interlock subsystem. */
    typedef struct {
        //! FETIM interlock sensors
        /*! Please see \ref INTRLK_SENSORS for more information. */
        INTRLK_SENSORS  sensors;
        //! FETIM interlock state
        /*! Please see \ref INTRLK_STATE for more information. */
        INTRLK_STATE    state;
    } INTERLOCK;

    /* Prototypes */
    /* Externs */
    extern void interlockHandler(void); //!< This function deals with the incoming CAN messages


#endif /* _INTERLOCK_H */

