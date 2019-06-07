/*! \file       backingPump.h
    \ingroup    cryostat
    \brief      Backing pump header file

    <b> File information: </b><br>
    Created: 2004/10/27 14:27:40 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the backing pump included in the %cryostat. */

/*! \defgroup   backingPump Backing pump
    \ingroup    cryostat
    \brief      Backing pump module
    \note       The \ref backingPump module doesn't include any submodule.

    For more information on this module see \ref backingPump.h */

#ifndef _BACKINGPUMP_H
    #define _BACKINGPUMP_H

    /* Extra Includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodule definitions */
    #define BACKING_PUMP_MODULES_NUMBER     1       // It's just the supplyCurrent230VHandler

    /* Typedefs */
    //! Current state of the backing pump
    /*! This structure represent the current state of the backing pump.
        \ingroup    cryostat
        \param      enable  This contains the current state of the backing
                                pump. It has to be remembered that this is
                                \em not a read back from the hardware but just a
                                register holding the last issued control:
                                    - \ref BACKING_PUMP_DISABLE -> Disable/OFF
                                    - \ref BACKING_PUMP_ENABLE -> Enable/ON
        \param      lastEnable  This contains a copy of the last issued control
                                message to the state of the backing pump. */
     typedef struct {
        //! Backing pump state
        /*! This is the state of the backing pump:
                - \ref BACKING_PUMP_DISABLE -> OFF (power up state)
                - \ref BACKING_PUMP_ENABLE -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued. */
        unsigned char   enable;
        //! Last control message: backing pump state
        /*! This is the content of the last control message sent to the backing
            pump state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } BACKING_PUMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentBackingPumpModule; //!< Currently addressed backing pump submodule

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    /* Externs */
    extern void backingPumpHandler(void); //!< This function deals with the incoming CAN message


#endif /* _BACKINGPUMP_H */
