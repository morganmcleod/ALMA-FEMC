/*! \file       sisHeater.h
    \ingroup    polarization
    \brief      SIS heater header file

    <b> File informations: </b><br>
    Created: 2004/08/24 14:02:29 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sisHeater.h,v 1.20 2007/06/02 21:23:58 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the SIS heater. */

/*! \defgroup   sisHeater   SIS Heater
    \ingroup    polarization
    \brief      SIS heater module
    \note       The \ref sisHeater module doesn't include any submodule.

    For more information on this module see \ref sisHeater.h */

#ifndef _SISHEATER_H
    #define _SISHEATER_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Submodules definitions */
    #define SIS_HEATER_MODULES_NUMBER       2       // See list below
    #define SIS_HEATER_MODULES_RCA_MASK     0x00040 /* Mask to extract the submodule number:
                                                       0 -> enableHandler
                                                       1 -> currentHandler */
    #define SIS_HEATER_MODULES_MASK_SHIFT   6       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the SIS heater
    /*! This structure represent the current state of the SIS heater.
        \ingroup    polarization
        \param      available   This indicates the availability of the rrequired
                                heater:
                                    - \ref UNAVAILABLE -> Unavailable
                                    - \ref AVAILABLE -> Available
        \param      enable[Op]  This contains the curren state of the lna. It
                                has to be remembered that this is \em not a read
                                back from the hardware but just a register
                                holding the last issued control:
                                    - \ref SIS_HEATER_DISABLE -> OFF
                                    - \ref SIS_HEATER_ENABLE -> ON
        \param      current[Op] This contains the most recent read-back value
                                for the heater current.
        \param      lastEnable  This contains a copy of the last issued control
                                message for the current. */
    typedef struct {
        //! SIS heater availability
        /*! This variable indicates if this sideband is outfitted with this
            particular SIS heater. This value should be part of the device
            dependent information retrived from the configuration database. */
        unsigned char   available;
        //! SIS heater state
        /*! This is the state of the SIS heater:
                - 0 -> disable (power up state)
                - 1 -> enable
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued.*/
         unsigned char   enable[OPERATION_ARRAY_SIZE];
        //! SIS heater current
        /*! This is the current (in mA) across the SIS heater. */
        float           current[OPERATION_ARRAY_SIZE];
        //! Last control message: SIS heater state
        /*! This is the content of the last control message sent to the SIS
            heater state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } SIS_HEATER;

    /* Globals */
    /* Externs */
    extern unsigned char currentSisHeaterModule; //!< Current addressed SIS heater submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    static void currentHandler(void);
    /* Externs */
    extern void sisHeaterHandler(void); //!< This function deals with the incoming CAN message

    extern void sisHeaterGoStandby2(int cartridge, int polarization);
    //!< set the specified SIS heater to STANDBY2 mode

#endif /* _SISHEATER_H */
