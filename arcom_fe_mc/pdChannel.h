/*! \file       pdChannel.h
    \ingroup    pdModule
    \brief      Power distribution channel header file

    <b> File informations: </b><br>
    Created: 2004/10/25 11:20:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pdChannel.h,v 1.12 2007/03/13 21:02:05 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the power distribution channel subsystem. There
    is one channel for each voltage provided to each cartridge. */

/*! \defgroup   pdChannel            Power ditribution channel
    \ingroup    pdModule
    \brief      Power ditribution channel
    \note       The \ref pdChannel module doesn't include any submodule.

    For more information on this module see \ref pdChannel.h */

#ifndef _PDCHANNEL_H
    #define _PDCHANNEL_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define PD_CHANNELS_NUMBER   6 //!< Number of voltages provided for each cartridge
    #define PLUS_6      0   //!< 0: +6
    #define MINUS_6     1   //!< 1: -6
    #define PLUS_15     2   //!< 2: +15
    #define MINUS_15    3   //!< 3: -15
    #define PLUS_24     4   //!< 4: +24
    #define PLUS_8      5   //!< 5: +8

    /* Submodules definitions */
    #define PD_CHANNEL_MODULES_NUMBER       2       // See list below
    #define PD_CHANNEL_MODULES_RCA_MASK     0x00001 /* Mask to extract the submodule number:
                                                       0 -> voltage
                                                       1 -> current */


    /* Typedefs */
    //! Current state of the power distribution channel system
    /*! This structure represent the current state of the power distribution
        channel system.
        \ingroup    pdModule
        \param      voltage[Op]     This contains the most recent read-back
                                    value for this channel voltage.
        \param      current[Op]     This contains the most recent read-back
                                    value for this channel current. */
    typedef struct {
        //! Channel voltage
        /*! This is the voltage provided by this channel. */
        float   voltage[OPERATION_ARRAY_SIZE];
        //! Channel current
        /*! This is the current drawn by this channel. */
        float   current[OPERATION_ARRAY_SIZE];
    } PD_CHANNEL;

    /* Globals */
    /* Externs */
    extern unsigned char currentPdChannelModule; //!< Current addressed power distribution channel submodule

    /* Prototypes */
    /* Statics */
    static void voltageHandler(void);
    static void currentHandler(void);
    /* Externs */
    extern void pdChannelHandler(void); //!< This function deals with the incoming CAN message

#endif /* _PDCHANNEL_H */
