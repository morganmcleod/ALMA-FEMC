/*! \file       powerDistribution.h
    \ingroup    frontend
    \brief      Power distribution system header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: powerDistribution.h,v 1.14 2006/11/30 23:01:42 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the power distribution system. The part of the
    system that is under the control of this software is only the <em>power
    distribution system</em>. The main power supply unit will have its own CAN
    node. */

/*! \defgroup   powerDistribution Power distribution system
    \ingroup    frontend
    \brief      Power distribution system module

    This group includes all the different \ref powerDistribution submodules. For
    more information on the \ref powerDistribution module see
    \ref powerDistribution.h */

#ifndef _POWERDISTRIBUTION_H
    #define _POWERDISTRIBUTION_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* PDMODULE defines */
    #ifndef _PDMODULE_H
        #include "pdModule.h"
    #endif /* _PDMODULE_H */

    /* Defines */
    #define MAX_POWERED_BANDS_OPERATION     3   //!< Max number of cartridges powered during normal operation
    #define MAX_POWERED_BANDS_DEBUG         10  //!< Max number of cartridges powered in debug mode

    /* Submodules definitions */
    #define POWER_DISTRIBUTION_MODULES_NUMBER       11      // See list below
    #define POWER_DISTRIBUTION_MODULES_RCA_MASK     0x000F0  /* Mask to extract the submodule number:
                                                                0 -> pdModule
                                                                1 -> pdModule
                                                                2 -> pdModule
                                                                3 -> pdModule
                                                                4 -> pdModule
                                                                5 -> pdModule
                                                                6 -> pdModule
                                                                7 -> pdModule
                                                                8 -> pdModule
                                                                9 -> pdModule
                                                                10 -> poweredModules */
    #define POWER_DISTRIBUTION_MODULES_MASK_SHIFT   4       // Bits right shift for the submodules mask

    /* Typedefs */
    //! Current state of the power distribution system
    /*! This structure represent the current state of the power distribution
        system.
        \ingroup    frontend
        \param      pdModule[Ca]        This contains the information about the
                                        distribution modules. There can be up to
                                        \ref PD_MODULES_NUMBER in the power
                                        distribution system.
        \param      poweredModules[Op]  This contains the number of modules
                                        (cartridges) currently powered on. The
                                        max number of cartridges powered at one
                                        time depends on the operation mode the
                                        receiver is in at the current time. */
    typedef struct {
        //! Current state of the different power distribution modules.
        /*! There is one power distribution module available for each cartridge.

            There is distribution module per cartridges. Cartridges \p Ca are
            assigned according to the following:
                - Ca = 0: Band 01
                - Ca = 1: Band 02
                - Ca = 2: Band 03
                - Ca = 3: Band 04
                - Ca = 4: Band 05
                - Ca = 5: Band 06
                - Ca = 6: Band 07
                - Ca = 7: Band 08
                - Ca = 8: Band 09
                - Ca = 9: Band 10

            Please see \ref PD_MODULE for more informations. */
        PD_MODULE       pdModule[PD_MODULES_NUMBER];
        //! Current number of cartridges powered
        /*! This variables stores the number of modules currently powered up.
            There can be at most:
                - \ref MAX_POWERED_BANDS_OPERATION in operational mode
                - \ref MAX_POWERED_BANDS_DEBUG in debug mode
            cartridges on simultaneously. */
        unsigned char   poweredModules[OPERATION_ARRAY_SIZE];
    } POWER_DISTRIBUTION;

    /* Globals */
    /* Externs */
    extern unsigned char currentPowerDistributionModule; //!< Current addressed power distribution module

    /* Prototypes */
    /* Statics */
    static void poweredModulesHandler(void);
    /* Externs */
    extern int powerDistributionInit(void); //!< This function deals with the initialization of the power distribution system
    extern void powerDistributionHandler(void); //!< This function deals with the incoming can message

#endif /* _POWERDISTRIBUTION_H */
