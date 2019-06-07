/*! \file       fetim.h
    \ingroup    fetim
    \brief      Front End Thermal Interlock Module

    <b> File information: </b><br>
    Created: 2011/03/25 17:29:12 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the Front End Thermal Interlock Module. */

/*! \defgroup   fetim       Front End Thermal Interlock Module
    \ingroup    frontend
    \brief      Front End Thermal Interlock Module

    This group includes all the different \ref fetim submodules. For more
    information on the \ref fetim module see \ref fetim.h */

#ifndef _FETIM_H
    #define _FETIM_H

    /* Extra includes */
    /* Interlock defines */
    #ifndef _INTERLOCK_H
        #include "interlock.h"
    #endif /* _INTERLOCK_H */

    /* Compressor defines */
    #ifndef _COMPRESSOR_H
        #include "compressor.h"
    #endif /* _COMPRESSOR_H */

    /* Dewar defines */
    #ifndef _DEWAR_H
        #include "dewar.h"
    #endif /* _DEWAR_H */

    /* Submodule definitions */
    #define FETIM_MODULES_NUMBER        3       // See list below
    #define FETIM_MODULES_RCA_MASK      0x000C0 /* Mask to extract the submodule number:
                                                   0 -> interlock
                                                   1 -> compressor
                                                   2 -> dewar */
    #define FETIM_MODULES_MASK_SHIFT    6       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM system
    /*! This structure represent the curren tstate of the FETIM system */
    typedef struct {
        //! FETIM availability
        /*! This field indicated if the FETIM is installed or not in the front
            end. This will be determined at startup by the FEMC hardware. */
        unsigned char   available;

        //! FETIM board hardware revision level
        /*! This contains the FETIM board hardware revision level */
        unsigned char   hardwRevision;

        //! FETIM thermal interlock
        /*! Please see \ref INTERLOCK for more information. */
        INTERLOCK       interlock;

        //! FETIM compressor interlock
        /*! Please see \ref COMPRESSOR for more information. */
        COMPRESSOR      compressor;

        //! FETIM dewar module
        /*! Please see \ref DEWAR for more information. */
        DEWAR           dewar;
    } FETIM;

    /* Globals */
    /* Externs */
    extern unsigned char currentFetimModule; //!< Currently addressed FETIM module
    extern unsigned char currentAsyncFetimExtTempModule; //!< A global to keep track of the FETIM external temperature module currently addressed by the async routine
    extern int asyncFetimExtTempError[FETIM_EXT_SENSORS_NUMBER]; //!< A global to keep track of the async error while monitoring FETIM external temperatures
    extern int asyncFetimHePressError; //!< A global to keep track of the async error while monitoring FETIM He2 pressure
    /* Prototypes */
    /* Externs */
    extern void fetimHandler(void); //!< This function deals with the incoming CAN messages
    extern int fetimStartup(void); //!< This function initializes the FETIM subsystem
    extern int fetimAsync(void); //!< This function deals with the asynchronous operation of the FETIM

#endif /* _FETIM_H */
