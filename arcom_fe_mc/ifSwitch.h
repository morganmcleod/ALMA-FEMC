/*! \file       ifSwitch.h
    \ingroup    frontend
    \brief      IF switch header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifSwitch.h,v 1.16 2007/06/01 20:55:10 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the IF switch system. This system allows to
    route the 4 output IF channels from a particular cartridge to the backend.
    Every single IF channel has a settable attenuation in order to equalize for
    difference in power between channels. */

/*! \defgroup   ifSwitch  IF switch
    \ingroup    frontend
    \brief      IF switch module

    This group includes all the different \ref ifSwitch submodules. For more
    information on the \ref ifSwitch module see \ref ifSwitch.h */

#ifndef _IFSWITCH_H
    #define _IFSWITCH_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

        /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* CARTRIDGE defines */
    #ifndef _CARTRIDGE_H
        #include "cartridge.h"
    #endif /* _CARTRIDGE_H */

    /* IFCHANNEL defines */
    #ifndef _IFCHANNEL_H
        #include "ifChannel.h"
    #endif /* _IFCHANNEL_H */

    /* Defines */
    #define IF_SWITCH_WAYS                  CARTRIDGES_NUMBER       // Number of 10:1 channels in the switch
    #define WAY0                            0
    #define WAY1                            1
    #define WAY2                            2
    #define WAY3                            3
    #define WAY4                            4
    #define WAY5                            8
    #define WAY6                            9
    #define WAY7                            10
    #define WAY8                            11
    #define WAY9                            12

    /* Submodule definitions */
    #define IF_SWITCH_MODULES_NUMBER        (IF_CHANNELS_NUMBER+1)  // See list below
    #define IF_SWITCH_MODULES_RCA_MASK      0x0001C                 /* Mask to extract the submodule number:
                                                                       0-3  -> ifChannelHandler
                                                                       4    -> bandSelectHandler */
    #define IF_SWITCH_MODULES_MASK_SHIFT    2                       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the IF switch system
    /*! This structure represent the current state of the IF switch system.
        \ingroup    frontend
        \param      ifChannel[Po][Sb]   a IF_CHANNEL
        \param      bandSelect          an unsigned char
        \param      lastBandSelect      This contains a copy of the last issued
                                        control message for the band select. */
     typedef struct {
        //! Current state of the the IF channel
        /*! There are four different IF channel for every cartridge. The
            attenuation of each channel can be set independently.

            Polarizations \p Po and sidebands \p Sb are assigned according to
            the following:
                - Po = 0: Polarization 0
                - Po = 1: Polarization 1
                - Sb = 0: Sideband 1
                - Sb = 1: Sideband 2

            \todo Find out what the backend is expecting from the different
                  channels and assign upper and lower sideband accordingly

            Please see \ref IF_CHANNEL for more informations. */
        IF_CHANNEL      ifChannel[POLARIZATIONS_NUMBER][SIDEBANDS_NUMBER];
        //! Selected cartridge
        /*! This is the currently cartridge selected by the IF switch. */
        unsigned char   bandSelect[OPERATION_ARRAY_SIZE];
        //! Last control message: band select
        /*! This is the content of the last control message sent to the band
            select. */
        LAST_CONTROL_MESSAGE    lastBandSelect;
    } IF_SWITCH;

    /* Globals */
    /* Externs */
    extern unsigned char currentIfSwitchModule; //!< Currently addressed IF switch submodule
    extern unsigned char currentIfSwitchWay[IF_SWITCH_WAYS]; //!< Currently addressed IF switch way

    /* Prototypes */
    /* Statics */
    static void bandSelectHandler(void);
    /* Externs */
    extern void ifSwitchHandler(void); //!< This function deals with the incoming CAN message

#endif /* _IFSWITCH_H */
