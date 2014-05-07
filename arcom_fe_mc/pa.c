/*! \file   pa.c
    \brief  PA functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pa.c,v 1.13 2009/08/25 21:39:39 avaccari Exp $

    This files contains all the functions necessary to handle the PA events. */

/* Includes */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "loSerialInterface.h"
#include "debug.h"
#include "database.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
unsigned char   currentPaModule=0;
/* Statics */
static HANDLER  paModulesHandler[PA_MODULES_NUMBER]={paChannelHandler,
                                                     paChannelHandler,
                                                     supplyVoltage3VHandler,
                                                     supplyVoltage5VHandler};

/* PA handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the PA. */
void paHandler(void){

    #ifdef DEBUG
        printf("    PA\n");
    #endif /* DEBUG */

    /* Since the LO is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentPaModule=(CAN_ADDRESS&PA_MODULES_RCA_MASK)>>PA_MODULES_MASK_SHIFT;
    if(currentPaModule>=PA_MODULES_NUMBER){
        storeError(ERR_PA,
                   0x01); // Error 0x01 -> PA submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }
    /* Call the correct handler */
    (paModulesHandler[currentPaModule])();
}

/* Supply Voltage 3V Handler */
/* This function deals with all the monitor requests directed to the PA 3V
   supply voltage. There are no control messages allowed for the 3V supply
   voltage. */
void supplyVoltage3VHandler(void){

    #ifdef DEBUG
        printf("     Supply Voltage 3V\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PA,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PA,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the 3V supply voltage */
    if(getPa(PA_3V_SUPPLY_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      supplyVoltage3V[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pa.
                        supplyVoltage3V[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              supplyVoltage3V[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              supplyVoltage3V[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  supplyVoltage3V[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  supplyVoltage3V[HI_ERROR_RANGE])){
                    storeError(ERR_PA,
                               0x04); // Error 0x04: Error: 3V supply voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PA,
                               0x05); // Error 0x05: Warning: 3V supply voltage in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Supply Voltage 5V Handler */
/* This function deals with all the monitor requests directed to the PA 5V
   supply voltage. There are no control messages allowed for the 5V supply
   voltage. */
void supplyVoltage5VHandler(void){

    #ifdef DEBUG
        printf("     Supply Voltage 5V\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PA,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PA,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the 5V supply voltage */
    if(getPa(PA_5V_SUPPLY_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      supplyVoltage5V[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pa.
                        supplyVoltage5V[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              supplyVoltage5V[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              supplyVoltage5V[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  supplyVoltage5V[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  supplyVoltage5V[HI_ERROR_RANGE])){
                    storeError(ERR_PA,
                               0x06); // Error 0x06: Error: 5V supply voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PA,
                               0x06); // Error 0x06: Warning: 5V supply voltage in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

