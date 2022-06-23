/*! \file       paChannel.h
    \brief      PA channel header file
*/

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
     typedef struct {
        //! A channel Gate Voltage
        /*! This is the PA channel gate voltage (in V). */
        float   gateVoltage;
        //! A channel Drain  Voltage
        /*! This is the PA channel drain voltage (in V). */
        float   drainVoltage;
        //! A channel Drain  Current
        /*! This is the PA channel drain current (in mA). */
        float   drainCurrent;
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
