/*! \file       pll.h
    \ingroup    lo
    \brief      PLL header file

    <b> File informations: </b><br>
    Created: 2004/08/24 16:07:30 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pll.h,v 1.23 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the PLL. */

/*! \defgroup   pll Phase Locked Loop (PLL)
    \ingroup    lo
    \brief      PLL module
    \note       The \ref pll module doesn't include any submodule */

#ifndef _PLL_H
    #define _PLL_H

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
    #define PLL_MODULES_NUMBER      12       // See list below
    #define PLL_MODULES_RCA_MASK    0x0000F /* Mask to extract the submodule number:
                                               0 -> lockDetectVoltage
                                               1 -> correctionVoltage
                                               2 -> assemblyTemp
                                               3 -> YIGHeaterCurrent
                                               4 -> refTotalPower
                                               5 -> ifTotalPower
                                               6 -> bogoFunction
                                               7 -> unlockDetectLatch
                                               8 -> clearUnlockDetectLatch
                                               9 -> loopBandwidthSelect
                                               A -> sidebandLockPolaritySelect
                                               B -> nullLoopIntegrator */

    /* Typedefs */
    //! Current state of the PLL
    /*! This structure represent the current state of the Phase Lock Loop.
        \ingroup    lo
        \param      lockDetectVoltage               This contains the most
                                                    recent read-back value for
                                                    the lock detect voltage.
        \param      lockDetectVoltageScale          This contains the scale
                                                    factor for the
                                                    lockDetectVoltage as read
                                                    from the configuration file
        \param      correctionVoltage               This contains the most
                                                    recent read-back value for
                                                    the correction voltage
        \param      correctionVoltageScale          This contains the scale
                                                    factor for the
                                                    correctionVoltage as read
                                                    from the configuration file
        \param      assemblyTemp                    This contains the most
                                                    recent read-back value for
                                                    the PLL assembly temperature
        \param      YIGHeaterCurrent                This contains the most
                                                    recent read-back value for
                                                    the YIG heater current
        \param      YIGHeaterCurrentScale           This contains the scale
                                                    factor for the
                                                    YIGHeaterCurrent as read
                                                    from the configuration file
        \param      YIGHeaterCurrentOffset          This contains the offset
                                                    factor for the
                                                    YIGHeaterCurrent as read
                                                    from the configuration file
        \param      refTotalPower                   This contains the most
                                                    recent read-back value for
                                                    the REF total power
        \param      ifTotalPower                    This contains the most
                                                    recent read-back value for
                                                    the IF total power
        \param      unlockDetectLatch               This contains the most
                                                    recent read-back value for
                                                    the latched unlock status
                                                    bit
        \param      loopBandwidthSelect             This contains the current
                                                    state of the control to
                                                    operate the PLL loop
                                                    bandwidth select. It has to
                                                    be stored since this is \em
                                                    not a read-back from the
                                                    hardware but just a register
                                                    holding the last issued
                                                    control:
                                                        - 0 -> Default
                                                        - 1 -> Alternate
        \param      sidebandLockPolaritySelect      This contains the current
                                                    state of the control to
                                                    operate the PLL sideband
                                                    lock polarity select. It has
                                                    to be stored since this is
                                                    \em not a read-back from the
                                                    hardware but just a register
                                                    holding the last issued
                                                    control:
                                                        - 0 -> Lower (LSB)
                                                        - 1 -> Upper (USB)
        \param      nullLoopIntegrator              This contains the current
                                                    state of the control to
                                                    zero the PLL loop
                                                    integrator. It has to be
                                                    stored since this is \em not
                                                    a read-back from the
                                                    hardware but just a register
                                                    holding the last issued
                                                    control:
                                                        - 0 -> Operate
                                                        - 1 -> Null/Zero
        \param      lastClearUnlockDetectLatch      This contains a copy of the
                                                    last issued control message
                                                    to clear the latched unlock
                                                    detect bit.
        \param      lastLoopBandwidthSelect         This contains a copy of the
                                                    last issued control message
                                                    to select the PLL loop
                                                    bandwidth.
        \param      lastSidebandLockPolaritySelect  This contains a copy of the
                                                    last issued control message
                                                    to select the PLL sideband
                                                    lock polarity.
        \param      lastNullLoopIntegrator          This contains a copy of the
                                                    last issued control message
                                                    to zero the PLL loop
                                                    integrator. */
    typedef struct {
        //! PLL lock detect voltage
        /*! This is the lock detect voltage (in V) of the PLL. */
        float   lockDetectVoltage[OPERATION_ARRAY_SIZE];
        //! PLL lock detect voltage scale factor
        /*! This is the lock detect voltage scale factor */
        float   lockDetectVoltageScale;
        //! PLL correction voltage
        /*! This is the correction voltage (in V) of the PLL. */
        float   correctionVoltage[OPERATION_ARRAY_SIZE];
        //! PLL correction voltage scale factor
        /*! This is the correction voltage scale factor */
        float   correctionVoltageScale;
        //! PLL assembly temperature
        /*! This is the temperature (in deg C) of the PLL. */
        float   assemblyTemp[OPERATION_ARRAY_SIZE];
        //! PLL YIG Heater Current
        /*! This is the YIG heater current (in mA) of the PLL. */
        float   YIGHeaterCurrent[OPERATION_ARRAY_SIZE];
        /*! PLL YIG heater current scale factor */
        /*! This is the scale factor for the YIG heater current */
        float   YIGHeaterCurrentScale;
        /*! PLL YIG heater current offset factor */
        /*! This is the offset factor for the YIG heater current */
        float   YIGHeaterCurrentOffset;
        //! PLL REF total power
        /*! This is a measure (in V) of the total power of the reference for
            the PLL. */
        float   refTotalPower[OPERATION_ARRAY_SIZE];
        //! PLL IF total power
        /*! This is a measure (in V) of the total power of the IF for the PLL */
        float   ifTotalPower[OPERATION_ARRAY_SIZE];
        //! Latched unlock detect bit
        /*! This is a bit that monitor the lock state of the PLL. If the PLL
            looses lock, the unlock state will be latched until manually
            cleared. */
        char    unlockDetectLatch[OPERATION_ARRAY_SIZE];
        //! Select loop bandwidth
        /*! This bit controls the selection of the loop bandwidth for the
            PLL. */
        char    loopBandwidthSelect[OPERATION_ARRAY_SIZE];
        //! Select sideband lock polarity
        /*! This bit controls the selection of the sideband lock polarity. */
        char    sidebandLockPolaritySelect[OPERATION_ARRAY_SIZE];
        //! Null the loop integrator
        /*! This bit controls the operation of the PLL loop integrator. */
        char    nullLoopIntegrator[OPERATION_ARRAY_SIZE];
        //! Last control message: clear unlock latched bit
        /*! This is the content of the last control message sent to the PLL
            clear unlock latched bit. */
        LAST_CONTROL_MESSAGE    lastClearUnlockDetectLatch;
        //! Last control message: loop bandwidth select
        /*! This is the content of the last control message sent to the PLL
            select lock polarity register. */
        LAST_CONTROL_MESSAGE    lastLoopBandwidthSelect;
        //! Last control message: sideband lock polarity select
        /*! This is the content of the last control message sent to the PLL
            sideband lock polarity select. */
        LAST_CONTROL_MESSAGE    lastSidebandLockPolaritySelect;
        //! Last control message: null loop integrator
        /*! This is the content of the last control message sent to the PLL
            null loop integrator. */
        LAST_CONTROL_MESSAGE    lastNullLoopIntegrator;
    } PLL;

    /* Globals */
    /* Externs */
    extern unsigned char currentPllModule; //!< Current addressed PLL submodule

    /* Prototypes */
    /* Statics */
    static void lockDetectVoltageHandler(void);
    static void correctionVoltageHandler(void);
    static void assemblyTempHandler(void);
    static void YIGHeaterCurrentHandler(void);
    static void refTotalPowerHandler(void);
    static void ifTotalPowerHandler(void);
    static void unlockDetectLatchHandler(void);
    static void clearUnlockDetectLatchHandler(void);
    static void loopBandwidthSelectHandler(void);
    static void sidebandLockPolaritySelectHandler(void);
    static void nullLoopIntegratorHandler(void);
    /* Externs */
    extern void pllHandler(void); //!< This function deals with the incoming can message

#endif /* _PLL_H */
