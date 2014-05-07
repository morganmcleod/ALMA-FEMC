/*! \file       photoDetector.h
    \ingroup    edfa
    \brief      EDFA photodetector header file

    <b> File informations: </b><br>
    Created: 2007/05/29 12:51:41 by avaccari

    <b> CVS informations: </b><br>
    \$Id: photoDetector.h,v 1.2 2007/06/22 22:03:00 avaccari Exp $

    This files contains all the information necessary to define the
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
    #define PHOTO_DETECTOR_MODULES_NUMBER       2       // See list below
    #define PHOTO_DETECTOR_MODULES_RCA_MASK     0x00002 /* Mask to extract the submodule number:
                                                           0 -> current
                                                           1 -> power */
    #define PHOTO_DETECTOR_MODULES_MASK_SHIFT   1       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the EDFA photodetector
    /*! This structure represent the current state of the EDFA photodetector
        \ingroup    edfa
        \param      current[Op]     This contains the most recent read-back
                                    value for the photodetector current.
        \param      power[Op]       This contains the most recent read-back
                                    for the photodetector power.
        \param      coeff           Conversion coefficient for the photodetector
                                    power. */
    typedef struct {
        //! Photodetector current
        /*! This is the current used by the photodetector. */
        float   current[OPERATION_ARRAY_SIZE];
        //! Photodetector power
        /*! This is the power used by the photodetector. */
        float   power[OPERATION_ARRAY_SIZE];
        //! Power conversion coefficient
        /*! This is the coefficient necessary to calculate the power of the
            photodetector. */
        float   coeff;
    } PHOTO_DETECTOR;

    /* Globals */
    /* Externs */
    extern unsigned char currentPhotoDetectorModule; //!< Currently addressed EDFA photo detector submodule

    /* Prototypes */
    /* Statics */
    static void currentHandler(void);
    static void powerHandler(void);
    /* Externs */
    extern void photoDetectorHandler(void); //!< This function deals with the incoming CAN message

#endif /* _PHOTODETECTOR_H */
