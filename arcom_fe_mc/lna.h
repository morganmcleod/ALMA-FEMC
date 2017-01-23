/*! \file       lna.h
    \ingroup    sideband
    \brief      LNA header file

    <b> File informations: </b><br>
    Created: 2004/08/24 14:35:51 by avaccari

    <b> CVS informations: </b><br>
    \$Id: lna.h,v 1.19 2006/11/30 23:01:42 avaccari Exp $

    This files contains all the informations necessary to define the
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
    /*! This structure represent the current state of the LNA.
        \ingroup    sideband
        \param      available   This contains the availability state for the
                                addressed lna:
                                    - 0 -> Unavailable
                                    - 1 -> Available
        \param      stage[St]   This contains the information about the stages
                                available for this lna. There can be up to
                                \ref LNA_STAGES_NUMBER for each lna.
        \param      enable[Op]  This contains the current state of the lna. It
                                has to be remembered that this is \em not a read
                                back from the hardware but just a register
                                holding the last issued control:
                                    0 -> Disable/OFF
                                    1 -> Enable/ON
        \param      lastEnable  This contains a copy of the last issued control
                                message to the enable state of the lna. */
    typedef struct {
        //! LNA availability
        /*! This variable indicates if this sideband is outfitted with this
            particular LNA. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //!LNAstage current state
        /*! Stages \p St are assigned according to the following:
                - St = 0: Stage 1
                - St = 1: Stage 2
                - St = 2: Stage 3
                - St = 3: Stage 4
                - St = 4: Stage 5
                - St = 5: Stage 6

            Please see the definition of the \ref LNA_STAGE structure for more
            informations.*/
        LNA_STAGE           stage[LNA_STAGES_NUMBER];
        //!LNAstate
        /*! This is the state of the LNA:\n
            0 -> OFF (power up state)\n
            1 -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
        unsigned char       enable[OPERATION_ARRAY_SIZE];
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

    extern void lnaGoStandby2();
    //!< set the specified LNA to STANDBY2 mode

#endif /* _LNA_H */
