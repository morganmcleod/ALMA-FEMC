/*! \file       ifTempServo.h
    \ingroup    ifChannel
    \brief      IF switch temperature servo header file

    <b> File informations: </b><br>
    Created: 2006/11/30 11:30:33 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifTempServo.h,v 1.2 2006/12/01 22:55:55 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the IF switch temperatue servo. */

/*! \defgroup   ifTempServo     IF temperature servo
    \ingroup    ifChannel
    \brief      IF temperature servo
    \note       The \ref lnaLed module doesn't include any submodule.

    For more information on this module see \ref ifTempServo.h */

#ifndef _IFTEMPSERVO_H
    #define _IFTEMPSERVO_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

        /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Submodule definitions */
    #define IF_TEMP_SERVO_MODULES_NUMBER    1   // It's just the enableHandler

    /* Typedefs */
    //! Current state of the IF switch temperature servo
    /*! This structure represent the current state of the IF switch temperature
        servo.
        \ingroup    ifSwitch
        \param      enable[Op]  This contains the current state of the
                                temperature servo. It has to be remembered that
                                this is \em not a read-back from the hardware
                                but just a register holding the last issued
                                control:
                                    - 0 -> OFF
                                    - 1 -> ON
        \param      lastEnable  This contains a copy of the last issued control
                                message for the enable. */
    typedef struct {
        //! IF temperature servo state
        /*! This is the state of the IF temperature servo:
                - 0 -> OFF (power up state)
                - 1 -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has ben issued. */
        unsigned char   enable[OPERATION_ARRAY_SIZE];
        //! Last control message: IF temperature servo enable
        /*! This is the content of the last control message sent to the LNA led
            state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } IF_TEMP_SERVO;

    /* Globals */
    /* Externs */
    extern unsigned char currentIfTempServoModule; //!< Current addressed IF temperature servo submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    /* Externs */
    extern void ifTempServoHandler(void); //!< This function deals with the incoming CAN message

#endif /* _IFTEMPSERVO_H */
