/*! \file       lna.h
    \ingroup    sideband
    \brief      LNA header file

    <b> File information: </b><br>
    Created: 2004/08/24 14:35:51 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate one of the lna available for each sideband.
    See \ref lna for more information. */

/*! \defgroup   lna Low Noise Amplifier (LNA)
    \ingroup    sideband
    \brief      LNA module

    This group includes all the different \ref lna submodules. For more
    information on the \ref lna module see \ref lna.h */

#ifndef _LNA_H
    #define _LNA_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* LNA Stage */
    #ifndef _LNA_STAGE_H
        #include "lnaStage.h"
    #endif /* _LNA_STAGE */

    /* Submodules definitions */
    #define LNA_MODULES_NUMBER       7       // See list below
    #define LNA_MODULES_RCA_MASK     0x0001C /* Mask to extract the submodule number:
                                                0-5 -> lnaStageHandler
                                                  6 -> enableHandler */
    #define LNA_MODULES_MASK_SHIFT   2       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the LNA
    typedef struct {
        //!LNAstage current state
        /*! Stages \p St are assigned according to the following:
                - St = 0: Stage 1
                - St = 1: Stage 2
                - St = 2: Stage 3

            Please see the definition of the \ref LNA_STAGE structure for more
            information.*/
        LNA_STAGE           stage[LNA_STAGES_NUMBER];
        //!LNAstate
        /*! This is the state of the LNA:\n
            0 -> OFF (power up state)\n
            1 -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char       enable;
        //! Last control message: LNA state
        /*! This is the content of the last control message sent to the LNA
            state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } LNA;

    /* Globals */
    /* Externs */
    extern unsigned char currentLnaModule; //!< Current addressed LNA submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    /* Externs */
    extern void lnaHandler(void); //!< This function deals with the incoming can message
    extern void RESERVEDLNAHandler(void);   //!< Handler for LNA stages 4,5,6 which don't exist
    extern void lnaGoStandby2();
    //!< set the specified LNA to STANDBY2 mode

#endif /* _LNA_H */
