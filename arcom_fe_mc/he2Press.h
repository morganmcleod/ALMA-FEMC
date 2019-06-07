/*! \file       he2Press.h
    \ingroup    compressor
    \brief      FETIM compressor He2 pressure header file

    <b> File information: </b><br>
    Created: 2011/03/28 17:51:00 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the FETIM compressor He2 pressure sensor. */

/*! \defgroup   he2Press       FETIM compressor He2 pressure sensor
    \ingroup    compressor
    \brief      FETIM compressor He2 pressure sensor
    \note       the \ref he2Press module doesn't include any submodule

    For more information on this module see \ref he2Press.h */

#ifndef _HE2_PRESS_H
    #define _HE2_PRESS_H

    /* Submodule definitions */
    #define HE2_PRESS_MODULES_NUMBER       2       // See list below
    #define HE2_PRESS_MODULES_RCA_MASK     0x00004 /* Mask to extract the submodule number:
                                                      0 -> pressure
                                                      1 -> outOfRange */
    #define HE2_PRESS_MODULES_MASK_SHIFT   2       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM compressor He2 pressure system
    /*! This structure represent the current state of the FETIM compressor
        He2 pressure system:
        \ingroup    compressor
        \param      pressure    This contains the the current value of the
                                    He2 pressure.
        \param      pressOutRng This contains the current state of the
                                    He2 pressure out of range register:
                                        - \ref OK    -> He2 Pressure ok
                                        - \ref ERROR -> He2 pressure out of range */
    typedef struct {
        //! FETIM compressor He2 pressure
        /*! This contains tha latest read-back value for the He2 pressure
            sensor */
        float           pressure;
        //! FETIM compressor He2 pressure out of range
        /*! This contains the latest read-back value for the compressor He2
            pressure out of range register:
                - \ref OK    -> Pressure ok
                - \ref ERROR -> Pressure out of range */
        unsigned char   pressOutRng;
    } HE2_PRESS;

    /* Globals */
    /* Externs */
    extern unsigned char currentHe2PressModule; //!< Currently addressed compressor He2 pressure sensor module

    /* Prototypes */
    /* Statics */
    static void pressHandler(void);
    static void outOfRangeHandler(void);
    /* Externs */
    extern void he2PressHandler(void); //!< This function deals with the incoming CAN messages


#endif /* _HE2_PRESS_H */

