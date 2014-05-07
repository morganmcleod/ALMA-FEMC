/*! \file       modulationInput.h
    \ingroup    edfa
    \brief      EDFA modulation input header file

    <b> File informations: </b><br>
    Created: 2007/05/29 14:51:56 by avaccari

    <b> CVS informations: </b><br>
    \$Id: modulationInput.h,v 1.2 2007/06/22 22:03:00 avaccari Exp $

    This file contains all the information necessary to define the
    characteristics and operate the modulation input port of the edfa. */

/*! \defgroup   modulationInput     EDFA Modulation Input port
    \ingroup    edfa
    \brief      EDFA modulation input
    \note       The \ref modulationInput module doesn't include any
                submodule

    For more information on this module see \ref modulationInput.h */

#ifndef _MODULATIONINPUT_H
    #define _MODULATIONINPUT_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Modulation Input special messages */
    #ifndef _MISPECIALMSGS_H
        #include "miSpecialMsgs.h"
    #endif /* _MISPECIALMSGS_H */

    /* Submodule definitions */
    #define MODULATION_INPUT_MODULES_NUMBER     2       // See list below
    #define MODULATION_INPUT_MODULES_RCA_MASK   0x00002 /* Mask to extract the submodule nubmer:
                                                           0 -> valueHandler
                                                           1 -> MISpecialMsgs (only control) */
    #define MODULATION_INPUT_MODULES_MASK_SHIFT 1       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the EDFA modulation input
    /*! This structure represent the current state of the EDFA modulation input
        \ingroup    edfa
        \param      value[Op]   This contains the current value of the
                                modulation input. It has to be remembered that
                                this is \em not a read-back from the hardware
                                but just a register holding the last issued
                                control value.
        \param      lastValue   This contains a copy of the last issued control
                                message to the modulation input value. */
    typedef struct {
        //! Modulation input value
        /*! This is the current value of the modulation input.
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued. */
        float                   value[OPERATION_ARRAY_SIZE];
        //! Last control message: value
        /*! This is the content of the last control message sent to the value
            of the modulation input. */
        LAST_CONTROL_MESSAGE    lastValue;
        //! Modulation Input special messages current state
        /*! Please see \ref MI_SPECIAL_MSGS for more informations. */
        MI_SPECIAL_MSGS         miSpecialMsgs;
    } MODULATION_INPUT;

    /* Globals */
    /* Externs */
    extern unsigned char currentModulationInputModule; //!< Currently addressed EDFA modulation input submodule

    /* Prototypes */
    /* Statics */
    static void valueHandler(void);
    /* Externs */
    extern void modulationInputHandler(void); //!< This function deals with the incoming CAN message

#endif /* _MODULATIONINPUT_H */
