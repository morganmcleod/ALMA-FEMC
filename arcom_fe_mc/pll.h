/*! \file       pll.h
    \ingroup    lo
    \brief      PLL header file

    <b> File information: </b><br>
    Created: 2004/08/24 16:07:30 by avaccari

    This file contains all the information necessary to define the
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
    typedef struct {
        //! PLL lock detect voltage
        /*! This is the lock detect voltage (in V) of the PLL. */
        float   lockDetectVoltage;
        //! PLL lock detect voltage scale factor
        /*! This is the lock detect voltage scale factor */
        float   correctionVoltage;
        //! PLL correction voltage scale factor
        /*! This is the correction voltage scale factor */
        float   assemblyTemp;
        //! PLL YIG Heater Current
        /*! This is the YIG heater current (in mA) of the PLL. */
        float   YIGHeaterCurrent;
        /*! PLL YIG heater current scale factor */
        /*! This is the scale factor for the YIG heater current */
        float   refTotalPower;
        //! PLL IF total power
        /*! This is a measure (in V) of the total power of the IF for the PLL */
        float   ifTotalPower;
        //! Latched unlock detect bit
        /*! This is a bit that monitor the lock state of the PLL. If the PLL
            looses lock, the unlock state will be latched until manually
            cleared. */
        char    unlockDetectLatch;
        //! Select loop bandwidth
        /*! This bit controls the selection of the loop bandwidth for the
            PLL. */
        char    loopBandwidthSelect;
        //! Select sideband lock polarity
        /*! This bit controls the selection of the sideband lock polarity. */
        char    sidebandLockPolaritySelect;
        //! Null the loop integrator
        /*! This bit controls the operation of the PLL loop integrator. */
        char    nullLoopIntegrator;
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
