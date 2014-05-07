/*! \file       compressor.h
    \ingroup    fetim
    \brief      FETIM compressor header file

    <b> File informations: </b><br>
    Created: 2011/03/28 17:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: compressor.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM compressor. */

/*! \defgroup   compressor       FETIM compressor
    \ingroup    fetim
    \brief      FETIM compressor system

    This group includes all the different \ref compressor submodules. For more
    information on the \ref compressor module see \ref compressor.h */

#ifndef _COMPRESSOR_H
    #define _COMPRESSOR_H

    /* Extra Includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Temperature sensors */
    #ifndef _COMP_TEMP_H
        #include "compTemp.h"
    #endif /* _COMP_TEMP_H */

    /* He2 pressure sensor */
    #ifndef _HE2_PRESS_H
        #include "he2Press.h"
    #endif /* _HE2_PRESS_H */

    /* Submodule definitions */
    #define COMPRESSOR_MODULES_NUMBER       6       // See list below
    #define COMPRESSOR_MODULES_RCA_MASK     0x00038 /* Mask to extract the submodule number:
                                                       0 -> compTemp
                                                       1 -> compTemp
                                                       2 -> he2Press
                                                       3 -> feStatus
                                                       4 -> interlockStatus
                                                       5 -> compCableStatus */
    #define COMPRESSOR_MODULES_MASK_SHIFT   3       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM compressor system
    /*! This structure represent the current state of the FETIM compressor system
        \ingroup    fetim
        \param      temp[Tc]         This contains the state of the FETIM
                                     compressor temperature subsystem. See
                                     \ref COMP_TEMP for more info.
        \param      he2Press         This contains the state of the FETIM
                                     compressor He2 pressure subsystem. See
                                     \ref HE2_PRESS for more info.
        \param      feStatus[Op]     This contains the state of the FE cryogenics.
                                     This value generated by the FEMC based upon
                                     the monitored state of the FE cryogenics
                                     and it indicates if it is ok to enable the
                                     compressor:
                                        - \ref ERROR -> Not ready for cool-down
                                        - \ref OK    -> ready for cool-down
        \param      intrlkStatus[Op] This contains the current status of the
                                     compressor interlock system:
                                        - \ref ERROR -> Interlock triggered
                                        - \ref OK    -> Interlock ok
        \param      cableStatus[Op]  This contains the currenst status of the
                                     cable connection between the FETIM and the
                                     compressor:
                                        - \ref ERROR -> Problem with the cable
                                        - \ref OK    -> Cable OK */
    typedef struct {
        //! Compressor temperatures
        /*! This contains the information about the FETIM compressor temperature
            subsystem. */
        COMP_TEMP       temp[COMP_TEMP_SENSORS_NUMBER];
        //! Compressor He2 pressure
        /*! This contains the information about the FETIM compressor He2 Pressure
            subsystem. */
        HE2_PRESS       he2Press;
        //! FE status
        /*! This contains the FE status bit generated by the FEMC:
                - \ref ERROR -> Not ready for cool-down
                - \ref OK    -> ready for cool-down */
        unsigned char   feStatus[OPERATION_ARRAY_SIZE];
        //! Interlock status
        /*! This contains the status of the FETIM compressor interlock:
                - \ref ERROR -> Interlock triggered
                - \ref OK    -> Interlock ok */
        unsigned char   intrlkStatus[OPERATION_ARRAY_SIZE];
        //! Cable status
        /*! This contains the current status fo the cable connection between the
            FETIM and the compressor:
                - \ref ERROR -> Problem with the cable
                - \ref OK    -> Cable OK */
        unsigned char   cableStatus[OPERATION_ARRAY_SIZE];
    } COMPRESSOR;

    /* Globals */
    /* Externs */
    extern unsigned char currentCompressorModule; //!< Currently addressed compressor module

    /* Prototypes */
    /* Statics */
    static void feStatusHandler(void);
    static void interlockStatusHandler(void);
    static void compCableStatusHandler(void);
    /* Externs */
    extern void compressorHandler(void); //!< This function deals with the incoming CAN messages


#endif /* _INTERLOCK_H */

