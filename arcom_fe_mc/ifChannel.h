/*! \file       ifChannel.h
    \ingroup    ifSwitch
    \brief      IF Switch channel header file

    <b> File informations: </b><br>
    Created: 2004/10/25 14:48:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifChannel.h,v 1.7 2007/05/21 15:49:26 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the power distribution channel subsystem. There
    is one channel for each voltage provided to each cartridge. */

/*! \defgroup   ifChannel            IF Switch channel
    \ingroup    ifSwitch
    \brief      IF switch channel
    \note       The \ref ifChannel module doesn't include any submodule.

    For more information on this module see \ref ifChannel.h */

#ifndef _IFCHANNEL_H
    #define _IFCHANNEL_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

        /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* IFTEMPSERVO defines */
    #ifndef _IFTEMPSERVO_H
        #include "ifTempServo.h"
    #endif /* _IFTEMPSERVO_H */

    /* Defines */
    #define IF_CHANNELS_NUMBER  (POLARIZATIONS_NUMBER*SIDEBANDS_NUMBER) //!< Number of IF channels available
    #define IF_CHANNEL0         0
    #define IF_CHANNEL1         1
    #define IF_CHANNEL2         2
    #define IF_CHANNEL3         3

    /* IF switch temperature evaluation defines */
    #define BRIDGE_RESISTOR         4020.0  // R1
    #define VREF                    0.5     // Reference voltage
    #define BETA_NORDEN             3380.0  // Norden uses a termistor from muRata #NCP15XH103J03RC
    #define TEMP_OFFSET             273.15  // Guess!


    /* Submodules definitions */
    #define IF_CHANNEL_MODULES_NUMBER   3       // See list below
    #define IF_CHANNEL_MODULES_RCA_MASK 0x00003 /* Mask to extract the submodule number:
                                                   0 -> ifTempServo
                                                   1 -> attenuation
                                                   2 -> assemblyTemp */



    /* Typedefs */
    //! Current state of the IF switch channel system
    /*! This structure represent the current state of the if switch channel
        system.
        \ingroup    ifSwitch
        \param      ifTempServo     a IF_TEMP_SERVO
        \param      attenuation     an unsigned char
        \param      assemblyTemp    a float
        \param      lastAttenuation This contains a copy of the last issued
                                    control message for the attenuation. */
    typedef struct {
        //! Temperature servo current state
        /*! Please see \ref IF_TEMP_SERVO for more informations. */
        IF_TEMP_SERVO   ifTempServo;
        //! Channel attenuation
        /*! This is the attenuation for this channel. */
        unsigned char   attenuation[OPERATION_ARRAY_SIZE];
        //! Assembly temperature
        /*! This is the current temperature for the specified IF channel. */
        float           assemblyTemp[OPERATION_ARRAY_SIZE];
        //! Last control message: attenuation
        /*! This is the content of the last control message sent to the IF
            channel attenuation. */
        LAST_CONTROL_MESSAGE    lastAttenuation;
    } IF_CHANNEL;

    /* Globals */
    /* Externs */
    extern unsigned char currentIfChannelModule; //!< Currently addressed IF channel submodule
    extern unsigned char currentIfChannelPolarization[IF_CHANNELS_NUMBER]; //!< Currently addressed IF channel polarization
    extern unsigned char currentIfChannelSideband[IF_CHANNELS_NUMBER]; //!< Currently addressed IF channel sideband

    /* Prototyped */
    /* Statics */
    static void attenuationHandler(void);
    static void assemblyTempHandler(void);
    /* Externs */
    extern void ifChannelHandler(void); //!< This function deals with the incoming CAN message

#endif /* _IFCHANNEL_H */