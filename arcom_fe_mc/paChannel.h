/*! \file       paChannel.h
    \ingroup    pa
    \brief      PA channel header file

    <b> File informations: </b><br>
    Created: 2004/10/20 15:39:19 by avaccari

    <b> CVS informations: </b><br>
    \$Id: paChannel.h,v 1.22 2011/11/09 00:40:30 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate each PA channel. Every PA channel is connected
    to one polarization and the amplification is controllable. */

/*! \defgroup   paChannel   PA Channel
    \ingroup    pa
    \brief      PA Channel
    \note       The \ref paChannel module doesn't include any submodule.

    For more information on this module see \ref paChannel.h */

#ifndef _PACHANNEL_H
    #define _PACHANNEL_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* POLARIZATION defines */
    #ifndef _POLARIZATION_H
        #include "polarization.h"
    #endif  /* _POLARIZATION_H */

    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */


    /* Defines */
    #define PA_CHANNELS_NUMBER  (POLARIZATIONS_NUMBER) //!< Number of PA channel per PA
    #define PA_CHANNEL_A        0
    #define PA_CHANNEL_B        1
    #define PA_MAX_ALLOWED_TEMP 30.0    // Maximum temperature at which PA should be allowed to work
                                        //   This is checked on the 4K and 12K cryostat sensors.

    /* Submodules definitions */
    #define PA_CHANNEL_MODULES_NUMBER      3       // See list below
    #define PA_CHANNEL_MODULES_RCA_MASK    0x00003 /* Mask to extract the submodule number:
                                                      0 -> gateVoltage
                                                      1 -> drainVoltage
                                                      2 -> drainCurrent */

    /* Typedefs */
    //! Current state of the PA channel
    /*! This structure represent the current state of the Power Amplifier
        channel.
        \ingroup    pa
        \param      gateVoltage         This contains the most recent read-back
                                        value for the gate voltage.
        \param      drainVoltage        This contains the most recent read-back
                                        value for the drain voltage.
        \param      drainCurrent        This contains the most recent read-back
                                        value for the drain current.
        \param      lastGateVoltage     This contains a copy of the last issued
                                        control message for the gate voltage.
        \param      lastDrainVoltage    This contains a copy of the last issued
                                        control message for the drain voltage. */
     typedef struct {
        //! A channel Gate Voltage
        /*! This is the PA channel gate voltage (in V). */
        float   gateVoltage[OPERATION_ARRAY_SIZE];
        //! A channel Drain  Voltage
        /*! This is the PA channel drain voltage (in V). */
        float   drainVoltage[OPERATION_ARRAY_SIZE];
        //! A channel Drain  Current
        /*! This is the PA channel drain current (in mA). */
        float   drainCurrent[OPERATION_ARRAY_SIZE];
        //! Last control message: PA channel Gate Voltage
        /*! This is the content of the last control message sent to the PA
            channel gate voltage. */
        LAST_CONTROL_MESSAGE    lastGateVoltage;
        //! Last control message: A channel Drain  Voltage
        /*! This is the content of the last control message sent to the PA
            channel drain voltage. */
        LAST_CONTROL_MESSAGE    lastDrainVoltage;
    } PA_CHANNEL;

    /* Globals */
    /* Externs */
    extern unsigned char currentPaChannelModule; //!< Current addressed PA channel submodule

    /* Prototypes */
    /* Statics */
    static void gateVoltageHandler(void);
    static void drainVoltageHandler(void);
    static void drainCurrentHandler(void);
    /* Externs */
    extern void paChannelHandler(void); //!< This function deals with the incoming can message
    extern int currentPaChannel(void); //!< This function returns the current PA channel

#endif /* _PACHANNEL_H */
