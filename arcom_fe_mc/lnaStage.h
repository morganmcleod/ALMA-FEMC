/*! \file       lnaStage.h
    \ingroup    lna
    \brief      LNA stage header file

    <b> File informations: </b><br>
    Created: 2004/08/24 15:42:19 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lnaStage.h,v 1.20 2007/05/21 15:49:26 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the LNA stage. */

/*! \defgroup   lnaStage    LNA Stage
    \ingroup    lna
    \brief      LNA stage
    \note       The \ref lnaStage module doesn't include any submodule.

    For more information on this module see \ref lnaStage.h */

#ifndef _LNASTAGE_H
    #define _LNASTAGE_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

        /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Defines */
    #define LNA_STAGES_NUMBER   6 //!< Number of LNA stages per LNA

    /* Submodules definitions */
    #define LNA_STAGE_MODULES_NUMBER      3       // See list below
    #define LNA_STAGE_MODULES_RCA_MASK    0x00003 /* Mask to extract the submodule number:
                                                     0 -> drainVoltage
                                                     1 -> drainCurrent
                                                     2 -> gateVoltage */
    /* Typedefs */
    //! Current state of theLNAstage
    /*! This structure represent the current state of the LNA stage.
        \ingroup    lna
        \param      available           This indicates the availability of the
                                        required stage:
                                            - 0 -> Unavailable
                                            - 1 -> Available
        \param      drainVoltage        This contains the most recent read-back
                                        value for the drain voltage.
        \param      drainCurrent        This contains the most recent read-back
                                        value for the drain current.
        \param      gateVoltage         This contains the most recent read-back
                                        value for the gate voltage.
        \param      lastDrainVolage     This contains a copy of the last issued
                                        control message for the drain voltage.
        \param      lastDrainCurrent    This contains a copy of the last issued
                                        control message for the drain current. */
    typedef struct {
        //!LNAstage availability
        /*! This variable indicates if thisLNAis outfitted with this
            particular stage. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! LNA stage drain voltage
        /*! This is the drain voltage (in V) of the LNA stage. */
        float   drainVoltage[OPERATION_ARRAY_SIZE];
        //! LNA stage drain coltage
        /*! This is the drain coltage (in mA) of the LNA stage. */
        float   drainCurrent[OPERATION_ARRAY_SIZE];
        //! LNA stage gate voltage
        /*! This is the gate voltage (in V) of the LNA stage. */
        float   gateVoltage[OPERATION_ARRAY_SIZE];
        //! Last control message: LNA stage drain voltage
        /*! This is the content of the last control message sent to the LNA
            stage drain voltage. */
        LAST_CONTROL_MESSAGE    lastDrainVoltage;
        //! Last control message: LNA stage drain current
        /*! This is the content of the last control message sent to the LNA
            stage drain current. */
        LAST_CONTROL_MESSAGE    lastDrainCurrent;
    } LNA_STAGE;

    /* Globals */
    /* Externs */
    extern unsigned char currentLnaStageModule; //!< Current addressed LNA stage submodule

    /* Prototypes */
    /* Statics */
    static void drainVoltageHandler(void);
    static void drainCurrentHandler(void);
    static void gateVoltageHandler(void);
    /* Externs */
    extern void lnaStageHandler(void); //!< This function deals with the incoming CAN message

#endif /* _LNASTAGE_H */
