/*! \file       pdModule.h
    \ingroup    powerDistribution
    \brief      Power distribution module header file

    <b> File information: </b><br>
    Created: 2004/10/25 10:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the power distribution module subsystem. There
    is one module for each cartridge. */

/*! \defgroup   pdModule            Power ditribution module
    \ingroup    powerDistribution
    \brief      Power ditribution module

    This group includes all the different \ref pdModule submodules. For more
    information on the \ref pdModule module see \ref pdModule.h */

#ifndef _PDMODULE_H
    #define _PDMODULE_H

    /* Extra Includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* PDCHANNEL defines */
    #ifndef _PDCHANNEL_H
        #include "pdChannel.h"
    #endif /* _PDCHANNEL_H */

    /* CARTRIDGE defines */
    #ifndef _CARTRIDGE_H
        #include "cartridge.h"
    #endif /* _CARTRIDGE_H */

    /* Defines */
    #define PD_MODULES_NUMBER   (CARTRIDGES_NUMBER) //!< Number of power distribution modules

    /* Submodules definitions */
    #define PD_MODULE_MODULES_NUMBER        7       // See list below
    #define PD_MODULE_MODULES_RCA_MASK      0x0000E /* Mask to extract the submodule number:
                                                       0 -> pdChannel +6V
                                                       1 -> pdChannel -6V
                                                       2 -> pdChannel +15V
                                                       3 -> pdChannel -15V
                                                       4 -> pdChannel +8V
                                                       5 -> pdChannel +24V
                                                       6 -> enable */
    #define PD_MODULE_MODULES_MASK_SHIFT     1       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the power distribution module system
    /*! This structure represent the current state of the power distribution
        module system.
        \ingroup    powerDistribution
        \param      pdChannel[Ch]       This contains the information about the
                                        channels available in the power
                                        distribution system. There can be up to
                                        \ref PD_CHANNELS_NUMBER channels in the
                                        power distribution system.
        \param      enable          This contains the current state of this
                                        module in the power distribution. It has
                                        to be remembered that this is \em not a
                                        read back from the hardware but just a
                                        register holding the last issued
                                        control:
                                            - \ref PD_MODULE_DISABLE -> Disable/OFF
                                            - \ref PD_MODULE_ENABLE -> Enable/ON
        \param      lastEnable          This contains a copy of the last issued
                                        control message to enable the state of
                                        this module of the power distribution
                                        system. */
    typedef struct {
        //! Current state of the power distribution module channel
        /*! In every power distribution module there is one channel available
            for each voltage provided to the cartridge.

            Channels \p Ch are assigned according to the following:
                - Ch = 0: +6V
                - Ch = 1: -6V
                - Ch = 2: +15V
                - Ch = 3: -15V
                - Ch = 4: +24V
                - Ch = 5: +8V

            Please see \ref PD_CHANNEL for more information. */
        PD_CHANNEL  pdChannel[PD_CHANNELS_NUMBER];
        //! Current state of the module
        /*! This variables stores the current state of the module:
                \ref PD_MODULE_DISABLE -> OFF (power up state)
                \ref PD_MODULE_ENABLE -> ON
            \warning    It is not a read back of the actual value. The returned
                        value is the one stored by the software after a control
                        command has been issued. */
        unsigned char   enable;
        //! Last control message: pdModule state
        /*! This is the content of the last control message sent to the power
            distribution module state. */
        LAST_CONTROL_MESSAGE    lastEnable;
    } PD_MODULE;

    /* Globals */
    /* Externs */
    extern unsigned char currentPdModuleModule; //!< Currently addressed power distribution module submodule
    extern void printPoweredModuleCounts(void);

    /* Prototypes */
    /* Statics */
    static void enableHandler(void);
    static int allowPowerOn(int module, int standby2);
    static int allowStandby2(int module);
    /* Externs */
    extern void pdModuleHandler(void); //!< This function deals wit the incoming CAN message

#endif /* _PDMODULE_H */
