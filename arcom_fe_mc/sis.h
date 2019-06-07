/*! \file       sis.h
    \ingroup    sideband
    \brief      SIS header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:46:21 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the SIS mixer. */

/*! \defgroup   sis SIS Mixer
    \ingroup    sideband
    \brief      SIS module
    \note       The \ref sis module doesn't include any submodule.

    For more information on this module see \ref sis.h */

#ifndef _SIS_H
    #define _SIS_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Submodules definitions */
    #define SIS_MODULES_NUMBER       4       // See list below
    #define SIS_MODULES_RCA_MASK     0x00018 /* Mask to extract the submodule number:
                                                0 -> senseResistorHandler
                                                1 -> voltageHandler
                                                2 -> currentHandler
                                                3 -> openLoopHandler */
    #define SIS_MODULES_MASK_SHIFT   3       // Bits right shift for the submodules mask


    /* Typedefs */
    typedef struct {
        //! SIS mixer availability
        unsigned char   available;
        //! SIS current sense resistor
        /*! This variables contains the value of the current sense resistor in ohms. 
            This value changes depending on the band and is initialized at startup 
            to fixed known values.  It can be overridden by a command. */
        float           resistor;
        //! SIS mixer voltage
        /*! This is the bias voltage (in mV) applied to the mixer. */
        float           voltage;
        //! SIS mixer current
        /*! This is the current (in mA) across the mixer. */
        float           current;
        //! SIS mixer mode
        /*! This describes the current mode of operation of the mixer:\n
            0 -> Closed loop (power up state)\n
            1 -> Open loop
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char   openLoop;
        //! Last control message: SIS sense resister
        /*! This is the content of the last control message sent to the SIS
            sense resistor. */
        LAST_CONTROL_MESSAGE    lastSenseResistor;
        //! Last control message: SIS mixer voltage
        /*! This is the content of the last control message sent to the SIS
            mixer voltage. */
        LAST_CONTROL_MESSAGE    lastVoltage;
        //! Last control message: SIS mixer mode
        /*! This is the content of the last control message sent to the SIS
            mixer mode. */
        LAST_CONTROL_MESSAGE    lastOpenLoop;
    } SIS;

    /* Globals */
    /* Externs */
    extern unsigned char currentSisModule; //!< Current addressed SIS submodule

    /* Prototypes */
    /* Statics */
    static void senseResistorHandler(void);
    static void voltageHandler(void);
    static void currentHandler(void);
    static void openLoopHandler(void);
    /* Externs */
    extern void sisHandler(void); //!< This function deals with the incoming can message

    extern void sisGoStandby2();
    //!< set the specified SIS to STANDBY2 mode

#endif /* _SIS_H */
