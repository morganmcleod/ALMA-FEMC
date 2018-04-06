/*! \file       interlockGlitch.h
    \ingroup    interlockState
    \brief      FETIM interlock glitch counter header file

    <b> File informations: </b><br>
    Created: 2011/04/08 16:01:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockGlitch.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM interlock glitch counter. */

/*! \defgroup   interlockGlitch     FETIM interlock glitch counter
    \ingroup    interlockState
    \brief      FETIM interlock glitch counter
    \note       the \ref interlockGlitch module doesn't include any submodule

    For more information on this module see \ref interlockGlitch.h */

#ifndef _INTERLOCK_GLITCH_H
    #define _INTERLOCK_GLITCH_H

    /* Extra includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodule deinitions */
    #define INTERLOCK_GLITCH_MODULES_NUMBER     2       // See list below
    #define INTERLOCK_GLITCH_MODULES_RCA_MASK   0x00002 /* Mask to extract the submodule number:
                                                           0 -> value
                                                           1 -> countTrig */
    #define INTERLOCK_GLITCH_MODULES_MASK_SHIFT 1       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM interlock glitch system
    /*! This structure represent the current state of the FETIM interlock glitch
        system:
        \ingroup    interlockState
        \param      value[Op]       This contains the the current value of the
                                    glitch accumulator.
        \param      countTrig[Op]   This contains the current state of the
                                    glitch trigger register:
                                        - \ref OFF -> Not triggered
                                        - \ref ON  -> Triggered */
    typedef struct {
        //! FETIM glitch counter value
        /*! This contains tha latest read-back value for the glitch counter
            accumulator */
        float           value[OPERATION_ARRAY_SIZE];
        //! FETIM glitch counter triggered
        /*! This contains the latest read-back value for the glitch counter
            triggered status register:
                - \ref OFF -> Not triggered
                - \ref ON  -> Triggered */
        unsigned char   countTrig[OPERATION_ARRAY_SIZE];
    } INTRLK_GLITCH;


    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockGlitchModule; //!< Currently addressed interlock glitch module

    /* Prototypes */
    /* Statics */
    static void valueHandler(void);
    static void countTrigHandler(void);
    /* Externs */
    extern void interlockGlitchHandler(void); //!< This function deals with the incoming CAN messages

#endif /* _INTERLOCK_GLITCH_H */
