/*! \file       photoDetector.h
    \ingroup    edfa
    \brief      EDFA photodetector header file

    <b> File information: </b><br>
    Created: 2007/05/29 12:51:41 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the EDFA photodetector. */

/*! \defgroup   photoDetector   EDFA Photodetector
    \ingroup    edfa
    \brief      EDFA Photodetector
    \note       The \ref photoDetector module doesn't include any
                submodule.

    For more information on this module see \ref photoDetector.h */

#ifndef _PHOTODETECTOR_H
    #define _PHOTODETECTOR_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodules definitions */
    #define PHOTO_DETECTOR_MODULES_NUMBER       3       // See list below
    #define PHOTO_DETECTOR_MODULES_RCA_MASK     0x00003 /* Mask to extract the submodule number:
                                                           0 -> current
                                                           1 -> conversion coefficient
                                                           2 -> power */

    /* Typedefs */
    //! Current state of the EDFA photodetector
    typedef struct {
        //! Photodetector current
        /*! This is the current used by the photodetector. */
        float   current;
        
        //! Photodetector power
        /*! This is the power used by the photodetector. */
        float   power;
        
        //! Power conversion coefficient
        /*! This is the coefficient necessary to calculate the power of the
            photodetector. */
        float   coeff;

        //! Last control message: conversion coefficient
        /*! This is the content of the last control message sent to the LNA led
            state. */
        LAST_CONTROL_MESSAGE    lastCoeff;

    } PHOTO_DETECTOR;

    /* Globals */
    /* Externs */
    extern unsigned char currentPhotoDetectorModule; //!< Currently addressed EDFA photo detector submodule

    /* Prototypes */
    /* Statics */
    static void currentHandler(void);
    static void conversionCoeffHandler(void);
    static void powerHandler(void);
    /* Externs */
    extern void photoDetectorHandler(void); //!< This function deals with the incoming CAN message

#endif /* _PHOTODETECTOR_H */
