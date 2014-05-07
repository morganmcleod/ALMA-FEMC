/*! \file       interlockFlowSens.h
    \ingroup    interlockFlow
    \brief      LPR flow sensor header file

    <b> File informations: </b><br>
    Created: 2011/03/29 14:49:29 by avaccari

    <b> CVS informations: </b><br>
    \$Id: interlockFlowSens.h,v 1.1 2011/08/05 19:18:06 avaccari Exp $

    This files contains all the information necessary to define the
    characteristics and operate the interlock flow sensor. */

/*! \defgroup   interlockFlowSens     FETIM interlock flow sensor
    \ingroup    interlockFlow
    \brief      FETIM interlock Flow sensor
    \note       the \ref interlockFlowSens module doesn't include any submodule

    For more information on this module see \ref interlockFlowSens.h */

#ifndef _INTERLOCK_FLOW_SENS_H
    #define _INTERLOCK_FLOW_SENS_H

    /* Extra includes */
    /* GLOBALDEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Submodule definitions */
    #define INTERLOCK_FLOW_SENS_MODULES_NUMBER     1   // It's just the flow

    /* Typedefs */
    //! Current state of the FETIM interlock flow sensors
    /*! This structure represent the current state of the FETIM interlock
        flow sensor.
        \ingroup    interlockFlow
        \param      flow[Op]    This contains the most recent read-back value
                                for the flow */
    typedef struct {
        //! FETIM interlock flow sensors temeprature
        /*! This is the flow as registered by the sensor. */
        float   flow[OPERATION_ARRAY_SIZE];
    } INTRLK_FLOW_SENS;


    /* Globals */
    /* Externs */
    extern unsigned char currentInterlockFlowSensModule; //!< Currently addressed FETIM interlock flow sens module

    /* Prototypes */
    /* Statics */
    static void flowHandler(void);
    /* Externs */
    extern void interlockFlowSensHandler(void); //!< This function deals with teh incoming CAN message

#endif /* _INTERLOCK_FLOW_SENS_H */
