/*! \file       interlockSensors.h
    \ingroup    interlock
    \brief      FETIM interlock sensors header file

    <b> File informations: </b><br>
    Created: 2011/03/29 17:51:00 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockSensors.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the FETIM interlock sensors. */

/*! \defgroup   interlockSensors        FETIM interlock sensors
    \ingroup    interlock
    \brief      FETIM interlock sensors

    This group includes all the different \ref interlockSensors submodules. For
    more information on the \ref interlockSensors module see
    \ref interlockSensors.h */

#ifndef _INTERLOCK_SENSORS_H
    #define _INTERLOCK_SENSORS_H

    /* Extra includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Interlock temeprature sensors defines */
    #ifndef _INTERLOCK_TEMP_H
        #include "interlockTemp.h"
    #endif /* _INTERLOCK_TEMP_H */

    /* Interlock flow sensors defines */
    #ifndef _INTERLOCK_FLOW_H
        #include "interlockFlow.h"
    #endif /* _INTERLOCK_FLOW_H */

    /* Submodule definitions */
    #define INTERLOCK_SENSORS_MODULES_NUMBER        3       // See list below
    #define INTERLOCK_SENSORS_MODULES_RCA_MASK      0x00018 /* Mask to extract the submodule number:
                                                               0 -> interlockTempHandler
                                                               1 -> interlockFlowHandler
                                                               2 -> singleFailHandler */
    #define INTERLOCK_SENSORS_MODULES_MASK_SHIFT    3       // Bits right shift for the submodule mask

    /* Typedefs */
    //! Current state of the FETIM interlock sensors
    /*! This stucture represent the current state of the FETIM interlock sensors
        \ingroup    interlock
        \param      temperature     This contains the state of the interlock
                                    temeprature sensors
        \param      flow            This contains the state of the interlock
                                    flow sensors
        \param      singleFail[Op]  This contains the current state for the
                                    single fail register which record if any one
                                    of the sensors has failed:
                                        - \ref OK    -> Everything is OK
                                        - \ref ERROR -> At least one sensor failed */
    typedef struct {
        //! FETIM temperature sensors
        /*! Please see \ref INTRLK_TEMP for more information. */
        INTRLK_TEMP     temperature;
        //! FETIM flow sensors
        /*! Please see \ref INTRLK_FLOW for more information. */
        INTRLK_FLOW     flow;
        //! Single sensor failure
        /*! This variable indicated if any one of the FETIM interlock sensors
            has failed:
                - \ref OK    -> Everything is OK
                - \ref ERROR -> At least one sensor failed */
        unsigned char   singleFail[OPERATION_ARRAY_SIZE];
    } INTRLK_SENSORS;

    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockSensorsModule; //!< Currently addressed interlock sensor module

    /* Prototypes */
    /* Statics */
    static void singleFailHandler(void);
    /* Externs */
    extern void interlockSensorsHandler(void); //!< This function deals with the incoming CAN messages

#endif /* _INTERLOCK_SENSORS_H */
