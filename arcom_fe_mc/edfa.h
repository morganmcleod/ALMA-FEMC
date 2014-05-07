/*! \file       edfa.h
    \ingroup    edfa
    \brief      Erbium Doped Fiber Amplifier header file

    <b> File informations: </b><br>
    Created: 2007/05/29 14:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: edfa.h,v 1.3 2009/10/13 15:01:49 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the EDFA. */

/*! \defgroup   edfa        Erbium Doped Fiber Amplifier
    \ingroup    lpr
    \brief      Erbium Doped Fiber Amplifier system

    This group includes all the different \ref edfa submodules. For more
    information on the \ref edfa module see \ref edfa.h */

#ifndef _EDFA_H
    #define _EDFA_H

    /* Extra includes */
    /* LASER defines */
    #ifndef _LASER_H
        #include "laser.h"
    #endif /* _LASER_H */

    /* PHOTODETECTOR defines */
    #ifndef _PHOTODETECTOR_H
        #include "photoDetector.h"
    #endif /* _PHOTODETECTOR_H */

    /* MODULATIONINPUT defines */
    #ifndef _MODULATIONINPUT_H
        #include "modulationInput.h"
    #endif /* _MODULATIONINPUT_H */

    /* Submodule definitions */
    #define EDFA_MODULES_NUMBER     4       // See list below
    #define EDFA_MODULES_RCA_MASK   0x0000C /* Mask to extract the submodule number:
                                               0 -> edfaLaser
                                               1 -> edfaPhotoDetector
                                               2 -> edfaModulationInput
                                               3 -> driverState*/
    #define EDFA_MODULES_MASK_SHIFT 2       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the EDFA system
    /*! This structure represent the current state of the EDFA system
        \ingroup    lpr
        \param      laser               This contains the information about the
                                        laser.
        \param      photoDetector       This contains the information about the
                                        photo detector.
        \param      modulationInput     This contains the information about the
                                        modulation input.
        \param      driverTempAlarm[Op] This contains the current state of the
                                        EDFA driver temperature alarm:
                                            - \ref NO_ERROR -> No Error
                                            - \ref ERROR -> Error */
    typedef struct {
        //! Laser current state
        /*! This is the current state of the EDFA laser. */
        LASER              laser;
        //! Photo detector state
        /*! This is the current state of the EDFA photo detector */
        PHOTO_DETECTOR     photoDetector;
        //! Modulation input state
        /*! This is the state of the EDFA modulation input */
        MODULATION_INPUT   modulationInput;
        //! EDFA driver error status
        /*! This is the status of the EDFA driver temperature alarm */
        unsigned char      driverTempAlarm[OPERATION_ARRAY_SIZE];
    } EDFA;

    /* Globals */
    /* Externs */
    extern unsigned char currentEdfaModule; //!< Currently addressed EDFA submodule

    /* Prototypes */
    /* Statics */
    static void driverStateHandler(void);
    /* Externs */
    extern void edfaHandler(void); //!< This function deals with the incoming CAN messages

#endif /* _EDFA_H */
