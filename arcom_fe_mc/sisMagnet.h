/*! \file       sisMagnet.h
    \ingroup    sideband
    \brief      SIS magnetic coils header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:58:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: sisMagnet.h,v 1.16 2006/11/30 23:01:43 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the SIS magnetic coils used to kill the
    zero voltage Josephson currents in the SIS junctions. */

/*! \defgroup   sisMagnet SIS Magnetic Coil
    \ingroup    sideband
    \brief      SIS magnetic coil module
    \note       The \ref sisMagnet module doesn't include any submodule.

    For more information on this module see \ref sisMagnet.h */

#ifndef _SISMAGNET_H
    #define _SISMAGNET_H

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
    #define SIS_MAGNET_MODULES_NUMBER       2       // See list below
    #define SIS_MAGNET_MODULES_RCA_MASK     0x00010 /* Mask to extract the submodule number:
                                                       0 -> voltageHandler
                                                       1 -> currentHandler */
    #define SIS_MAGNET_MODULES_MASK_SHIFT   4       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the SIS magnetic coil
    /*! This structure represent the current state of the SIS magnetic coil.
        \ingroup    sideband
        \param      available   This indicates the availability of the required
                                magnet:
                                    - 0 -> Unavailable
                                    - 1 -> Available
        \param      voltage     This contains the most recent read-back value
                                for the magnet voltage.
        \param      current     This contains the most recent read-back value
                                for the magnet current.
        \param      lastCurrent This contains a copy of the last issued control
                                message for the current. */
    typedef struct {
        //! SIS magnet availability
        /*! This variable indicates if this sideband is outfitted with this
            particular SIS magnet. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! SIS magnetic coil voltage
        /*! This is the voltage (in mV) across the magnetic coils. */
        float   voltage[OPERATION_ARRAY_SIZE];
        //! SIS magnetic coil current
        /*! This is the current (in mA) across the magnetic coils. */
        float   current[OPERATION_ARRAY_SIZE];
        //! Last control message: SIS magnetic coil current
        /*! This is the content of the last control message sent to the SIS
            magnetic coil current. */
        LAST_CONTROL_MESSAGE    lastCurrent;
    } SIS_MAGNET;

    /* Globals */
    /* Externs */
    extern unsigned char currentSisMagnetModule; //!< Current addressed SIS magnet submodule

    /* Prototypes */
    /* Statics */
    static void voltageHandler(void);
    static void currentHandler(void);
    /* Externs */
    extern void sisMagnetHandler(void); //!< This function deals with the incoming can message

#endif /* _SISMAGNET_H */
