/*! \file       lprTemp.h
    \ingroup    lpr
    \brief      LPR temperature sensor header file

    <b> File information: </b><br>
    Created: 2007/05/29 14:49:29 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the lpr temperature sensor. */

/*! \defgroup   lprTemp     LO Photonic Receiver temperature sensor
    \ingroup    lpr
    \brief      LO Photonic Receiver temperature sensor
    \note       the \ref lprTemp module doesn't include any submodule

    For more information on this module see \ref lprTemp.h */

#ifndef _LPRTEMP_H
    #define _LPRTEMP_H

    /* Extra includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define LPR_TEMP_SENSORS_NUMBER     2   //!< Number of temperature sensors per lpr

    /* Submodule definitions */
    #define LPR_TEMP_MODULES_NUMBER     1   // It's just the temperature

    /* Typedefs */
    //! Current state of the lpr temperature sensors
    /*! This structure represent the current state of the lpr temperature
        system.
        \ingroup    lpr
        \param      temp    This contains the most recent read-back value
                                for the temperature */
    typedef struct{
        //! Lpr temperature
        /*! This is the temperature (in K) as registered by the sensor. */
        float   temp;
    } LPR_TEMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentLprTempModule; //!< Currently addressed LPR temperature submodule

    /* Prototypes */
    /* Statics */
    static void tempHandler(void);
    /* Externs */
    extern void lprTempHandler(void); //!< This function deals with teh incoming CAN message

#endif /* _LPRTEMP_H */
