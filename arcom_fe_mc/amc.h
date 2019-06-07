/*! \file       amc.h
    \ingroup    lo
    \brief      MC header file

    <b> File information: </b><br>
    Created: 2004/10/20 15:39:19 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the AMC. */

/*! \defgroup   amc Active Multiplier Chain (AMC)
    \ingroup    lo
    \brief      AMC module
    \note       The \ref amc module doesn't include any submodule.

    For more information on this module see \ref amc.h */

#ifndef _AMC_H
    #define _AMC_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodules definitions */
    #define AMC_MODULES_NUMBER      12      // See list below
    #define AMC_MODULES_RCA_MASK    0x0000F /* Mask to extract the submodule number:
                                                0 -> gateAVoltage
                                                1 -> drainAVoltage
                                                2 -> drainACurrent
                                                3 -> gateBVoltage
                                                4 -> drainBVoltage
                                                5 -> drainBCurrent
                                                6 -> multiplierDVoltage
                                                7 -> gateEVoltage
                                                8 -> drainEVoltage
                                                9 -> drainECurrent
                                               10 -> multiplierDCurrent
                                               11 -> supplyVoltage5V */

    /* Typedefs */
    //! Current state of the AMC
    /*! This structure represent the current state of the Active Chain
        Multiplier.
        \ingroup    lo
        \param      gateAVoltage            This contains the most recent
                                            read-back value for the A gate
                                            voltage.
        \param      drainAVoltage           This contains the most recent
                                            read-back value for the A drain
                                            voltage.
        \param      drainACurrent           This contains the most recent
                                            read-back value for the A drain
                                            current.
        \param      gateBVoltage            This contains the most recent
                                            read-back value for the B gate
                                            voltage.
        \param      drainBVoltage           This contains the most recent
                                            read-back value for the B drain
                                            voltage.
        \param      drainBCurrent           This contains the most recent
                                            read-back value for the B drain
                                            current.
        \param      multiplierDVoltage      This contains the most recent
                                            read-back value for the D multiplier
                                            voltage.
        \param      gateEVoltage            This contains the most recent
                                            read-back value for the E gate
                                            voltage.
        \param      drainEVoltage           This contains the most recent
                                            read-back value for the E drain
                                            voltage.
        \param      drainECurrent           This contains the most recent
                                            read-back value for the E drain
                                            current.
        \param      multiplierDCurrent      This contains the most recent
                                            read-back value for the D multiplier
                                            current.
        \param      supplyVoltage5V         This contains the most recent
                                            read-back value for the 5V supply
                                            voltage.
        \param      lastDrainBVoltage       This contains a copy of the last
                                            issued control message to the B
                                            drain voltage.
        \param      lastMultiplierDVoltage  This contains a copy of the last
                                            issued control message to the D
                                            multiplier voltage.
        \param      lastGateEVoltage        This contains a copy of the last
                                            issued control message to the E
                                            gate voltage.
        \param      lastDrainEVoltage       This contains a copy of the last
                                            issued control message to the E
                                            drain voltage. */
    typedef struct {
        //! MC A Gate Voltage
        /*! This is the MC A gate voltage (in V). */
        float   gateAVoltage;
        //! MC A Drain  Voltage
        /*! This is the MC A drain voltage (in V). */
        float   drainAVoltage;
        //! MC A Drain  Current
        /*! This is the MC A drain current (in mA). */
        float   drainACurrent;
        //! MC B Gate Voltage
        /*! This is the MC B gate voltage (in V). */
        float   gateBVoltage;
        //! MC B Drain  Voltage
        /*! This is the MC B drain voltage (in V). */
        float   drainBVoltage;
        //! MC B Drain  Current
        /*! This is the MC A drain current (in mA). */
        float   drainBCurrent;
        //! MC supply voltage 5V
        /*! This is the MC supply voltage 5V (in V). */
        float   supplyVoltage5V;
        //! MC multiplier D Voltage
        /*! This is the MC multiplier D voltage (in counts). */
        unsigned char   multiplierDVoltage;
        //! MC multiplier D Current
        /*! This is the MC multiplier D current (in mA). */
        float   multiplierDCurrent;
        //! MC E Gate Voltage
        /*! This is the MC E gate voltage (in V). */
        float   gateEVoltage;
        //! MC E Drain  Voltage
        /*! This is the MC E drain voltage (in V). */
        float   drainEVoltage;
        //! MC E Drain  Current
        /*! This is the MC E drain current (in mA). */
        float   drainECurrent;
        //! Last control message: MC B drain voltage
        /*! This is the content of the last control message sent to the MC B
            drain voltage. */
        LAST_CONTROL_MESSAGE    lastDrainBVoltage;
        //! Last control message: MC multiplier D voltage
        /*! This is the content of the last control message sent to the MC
            multiplier D voltage. */
        LAST_CONTROL_MESSAGE    lastMultiplierDVoltage;
        //! Last control message: MC E gate voltage
        /*! This is the content of the last control message sent to the MC E
            gate voltage. */
        LAST_CONTROL_MESSAGE    lastGateEVoltage;
        //! Last control message: MC E drain voltage
        /*! This is the content of the last control message sent to the MC E
            drain voltage. */
        LAST_CONTROL_MESSAGE    lastDrainEVoltage;

    } AMC;

    /* Globals */
    /* Externs */
    extern unsigned char currentAmcModule; //!< Current addressed MC submodule

    /* Prototypes */
    /* Statics */
    static void gateAVoltageHandler(void);
    static void drainAVoltageHandler(void);
    static void drainACurrentHandler(void);
    static void gateBVoltageHandler(void);
    static void drainBVoltageHandler(void);
    static void drainBCurrentHandler(void);
    static void multiplierDVoltageHandler(void);
    static void multiplierDCurrentHandler(void);
    static void gateEVoltageHandler(void);
    static void drainEVoltageHandler(void);
    static void drainECurrentHandler(void);
    static void supplyVoltage5VHandler(void);

    /* Externs */
    extern void amcHandler(void); //!< This function deals with the incoming can message

#endif /* _AMC_H */


