/*! \file       fetimExtTemp.h
    \ingroup    compressor
    \brief      FETIM external temperature header file

    This file contains all the information necessary to define the
    characteristics and operate the FETIM external temperature sensors. */

/*! \defgroup   fetimExtTemp    FETIM external temperature sensors
    \ingroup    compressor
    \brief      FETIM external temperature sensors
    \note       the \ref fetimExtTemp module doesn't include any submodule */

#ifndef _FETIM_EXT_TEMP_H
    #define _FETIM_EXT_TEMP_H

    /* Extra Includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define FETIM_EXT_SENSORS_NUMBER        2   //!< Number of temperature sensors per FETIM compressor
    #define FETIM_EXT_SENSOR_AMBIENT        0
    #define FETIM_EXT_SENSOR_TURBO          1

    /* Submodule definitions */
    #define FETIM_EXT_MODULES_NUMBER        2       // See list below
    #define FETIM_EXT_MODULES_RCA_MASK      0x00004 /* Mask to extract the submodule number:
                                                       0 -> tempHandler
                                                       1 -> outOfRangeHandler */
    #define FETIM_EXT_MODULES_MASK_SHIFT    2       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM external temperature system
    typedef struct {
        //! FETIM external temperature
        /*! This contains tha latest read-back value for the temperature
            sensor */
        float           temp;
        
        //! FETIM external temperature out of range
        /*! This contains the latest read-back value for the compressor
            temperature out of range register:
                - \ref OK    -> Temperature ok
                - \ref ERROR -> Temperature out of range */
        unsigned char   tempOutRng;
    } COMP_TEMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentFetimExtTempModule; //!< Currently addressed compressor temperature sensor module

    /* Prototypes */
    /* Statics */
    static void tempHandler(void);
    static void outOfRangeHandler(void);
    /* Externs */
    extern void fetimExtTempHandler(void); //!< This function deals with the incoming CAN messages


#endif /* _FETIM_EXT_TEMP_H */

