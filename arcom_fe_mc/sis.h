/*! \file       sis.h
    \ingroup    sideband
    \brief      SIS header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:46:21 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sis.h,v 1.20 2006/11/30 23:01:43 avaccari Exp $

    This files contains all the informations necessary to define the
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
    #define SIS_MODULES_NUMBER       3       // See list below
    #define SIS_MODULES_RCA_MASK     0x00018 /* Mask to extract the submodule number:
                                                0 -> voltageHandler
                                                1 -> currentHandler
                                                2 -> openLoopHandler */
    #define SIS_MODULES_MASK_SHIFT   3       // Bits right shift for the submodules mask


    /* Typedefs */
    //! Current state of the SIS mixer
    /*! This structure represent the current state of the SIS mixer.
        \ingroup    sideband
        \param      available       This indicates the availability of the sis:
                                        - 0 -> Unavailable
                                        - 1 -> Available
        \param      resistor        This contains the value of the current sense
                                    resistor. It should be loaded during
                                    initialization
        \param      voltage         This contains the most recent read-back
                                    value for the voltage
        \param      current         This contains the most recent read-back
                                    value for the current
        \param      openLoop        This contains the current state of the feed
                                    back loop for the junction. It has to be
                                    remembered that is \em not a read-back from
                                    the hardware but just a register:
                                        - 0 -> Closed Loop (Normal)
                                        - 1 -> Open Loop
        \param      lastVoltage     This contains a copy of the last issued
                                    control message to the junction voltage
        \param      lastOpenLoop    This contains a copy of the last issued
                                    control message to the feed back loop */
    typedef struct {
        //! SIS mixer availability
        /*! This variable indicates if the sideband is outfitted with this
            particular SIS mixer. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! SIS current sense resistor
        /*! This variables contains the value of the current sense resistor in
            ohm. This value changes depending on the band and it will be filled
            at initialization by the value contained in the configuration data
            base. */
        float           resistor;
        //! SIS mixer voltage
        /*! This is the bias voltage (in mV) applied to the mixer. */
        float           voltage[OPERATION_ARRAY_SIZE];
        //! SIS mixer current
        /*! This is the current (in mA) across the mixer. */
        float           current[OPERATION_ARRAY_SIZE];
        //! SIS mixer mode
        /*! This describes the current mode of operation of the mixer:\n
            0 -> Closed loop (power up state)\n
            1 -> Open loop
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char   openLoop;
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
    static void voltageHandler(void);
    static void currentHandler(void);
    static void openLoopHandler(void);
    /* Externs */
    extern void sisHandler(void); //!< This function deals with the incoming can message

#endif /* _SIS_H */
