/*! \file       compTemp.h
    \ingroup    compressor
    \brief      FETIM compressor temperature header file

    <b> File informations: </b><br>
    Created: 2011/03/28 17:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: compTemp.h,v 1.2 2011/11/09 00:40:30 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM compressor temperature sensors. */

/*! \defgroup   compTemp       FETIM compressor temperature sensors
    \ingroup    compressor
    \brief      FETIM compressor temperature sensors
    \note       the \ref compTemp module doesn't include any submodule

    For more information on this module see \ref compTemp.h */

#ifndef _COMP_TEMP_H
    #define _COMP_TEMP_H

    /* Extra Includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define COMP_TEMP_SENSORS_NUMBER        2   //!< Number of temperature sensors per FETIM compressor
    #define COMP_TEMP_SENSOR_AMBIENT        0
    #define COMP_TEMP_SENSOR_TURBO          1

    /* Submodule definitions */
    #define COMP_TEMP_MODULES_NUMBER        2       // See list below
    #define COMP_TEMP_MODULES_RCA_MASK      0x00004 /* Mask to extract the submodule number:
                                                       0 -> tempHandler
                                                       1 -> outOfRangeHandler */
    #define COMP_TEMP_MODULES_MASK_SHIFT    2       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM compressor temperature system
    /*! This structure represent the current state of the FETIM compressor
        temperature system:
        \ingroup    compressor
        \param      temp[Op]        This contains the the current value of the
                                    temperature.
        \param      tempOutRng[Op]  This contains the current state of the
                                    temeprature out of range register:
                                        - \ref OK    -> Temperature ok
                                        - \ref ERROR -> Temperature out of range */
    typedef struct {
        //! FETIM compressor temperature
        /*! This contains tha latest read-back value for the temperature
            sensor */
        float           temp[OPERATION_ARRAY_SIZE];
        //! FETIM compressor temperature out of range
        /*! This contains the latest read-back value for the compressor
            temperature out of range register:
                - \ref OK    -> Temperature ok
                - \ref ERROR -> Temperature out of range */
        unsigned char   tempOutRng[OPERATION_ARRAY_SIZE];
    } COMP_TEMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentCompTempModule; //!< Currently addressed compressor temperature sensor module

    /* Prototypes */
    /* Statics */
    static void tempHandler(void);
    static void outOfRangeHandler(void);
    /* Externs */
    extern void compTempHandler(void); //!< This function deals with the incoming CAN messages


#endif /* _COMP_TEMP_H */

