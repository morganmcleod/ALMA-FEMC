/*! \file       turboPump.h
    \ingroup    cryostat
    \brief      Turbo pump header file

    <b> File informations: </b><br>
    Created: 2004/10/27 14:27:40 by avaccari

    <b> CVS informations: </b><br>
    \$Id: turboPump.h,v 1.9 2011/11/09 00:40:30 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the turbo pump included in the cryostat. */

/*! \defgroup   turboPump Turbo pump
    \ingroup    cryostat
    \brief      Turbo pump module
    \note       The \ref turboPump module doesn't include any submodule.

    For more information on this module see \ref turboPump.h */

#ifndef _TURBOPUMP_H
    #define _TURBOPUMP_H

    /* Extra Includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodule definitions */
    #define TURBO_PUMP_MODULES_NUMBER       3       // See list below
    #define TURBO_PUMP_MODULES_RCA_MASK     0x00003 /* Mask to extract the submodule number:
                                                       0 -> enableHandler
                                                       1 -> stateHandler
                                                       2 -> speedHandler */

    /* Operability temperature range */
    #define TURBO_PUMP_MIN_WARN_TEMP        15.0
    #define TURBO_PUMP_MAX_WARN_TEMP        40.0
    #define TURBO_PUMP_MIN_TEMPERATURE      10.0    // Minimum temperature at which turbo pump is operable
    #define TURBO_PUMP_MAX_TEMPERATURE      45.0    // Maximum temperature at which turbo pump is operable

    /* Typedefs */
    //! Current state of the turbo pump
    /*! This structure represent the current state of the turbo pump.
        \ingroup    cryostat
        \param      enable[Op]  This contains the current state of the turbo
                                pump. It has to be remembered that this is
                                \em not a read-back from the hardware but just a
                                register holding the last issued control:
                                    - \ref TURBO_PUMP_DISABLE -> Disable/OFF
                                    - \ref TURBO_PUMP_ENABLE -> Enable/ON
        \param      state[Op]   This contains the current error state for the
                                turbo pump:
                                    - \ref NO_ERROR -> No error
                                    - \ref ERROR -> Error
        \param      speed[Op]   This contains the current speed state for the
                                turbo pump:
                                    - \ref SPEED_OK -> Speed OK
                                    - \ref SPEED_LOW -> Speed Low
        \param      lastEnable  This contains a copy of the last issued control
                                message to the state of the turbo pump. */
    typedef struct {
        //! Turbo pump state
        /*! This is the state of the turbo pump:
                - \ref TURBO_PUMP_DISABLE -> OFF (power up state)
                - \ref TURBO_PUMP_ENABLE -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued. */
        unsigned char   enable[OPERATION_ARRAY_SIZE];
        //! Turbo pump error state
        /*! This is the error state of the turbo pump:
                - \ref NO_ERROR -> no error
                - \ref ERROR -> error
            \todo   Verify that this is correct. */
        unsigned char   state[OPERATION_ARRAY_SIZE];
        //! Turbo pump speed state
        /*! This is the speed state of the turbo pump:
                - \ref SPEED_LOW -> not up to speed
                - \ref SPEED_OK -> up to speed */
        unsigned char   speed[OPERATION_ARRAY_SIZE];
        //! Last control message: turbo pump state
        /*! This is the content of the last control message sent to the turbo
            pump state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } TURBO_PUMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentTurboPumpModule; //!< Currently addressed turbo pump submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    static void stateHandler(void);
    static void speedHandler(void);
    /* Externs */
    extern void turboPumpHandler(void); //!< This function deals with the incoming CAN message

#endif /* _TURBOPUMP_H */
