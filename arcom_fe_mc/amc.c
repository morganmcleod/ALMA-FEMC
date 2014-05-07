/*! \file   amc.c
    \brief  MC functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: amc.c,v 1.17 2008/02/07 16:21:23 avaccari Exp $

    This files contains all the functions necessary to handle the MC events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "frontend.h"
#include "error.h"
#include "loSerialInterface.h"
#include "debug.h"
#include "database.h"


/* Globals */
/* Externs */
unsigned char   currentAmcModule=0;
/* Statics */
static HANDLER  amcModulesHandler[AMC_MODULES_NUMBER]={gateAVoltageHandler,
                                                       drainAVoltageHandler,
                                                       drainACurrentHandler,
                                                       gateBVoltageHandler,
                                                       drainBVoltageHandler,
                                                       drainBCurrentHandler,
                                                       multiplierDVoltageHandler,
                                                       gateEVoltageHandler,
                                                       drainEVoltageHandler,
                                                       drainECurrentHandler,
                                                       multiplierDCurrentHandler,
                                                       supplyVoltage5VHandler};

/* AMC handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the MC. */
void amcHandler(void){

    #ifdef DEBUG
        printf("    AMC\n");
    #endif /* DEBUG */

    /* Since the LO is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentAmcModule=(CAN_ADDRESS&AMC_MODULES_RCA_MASK);
    if(currentAmcModule>=AMC_MODULES_NUMBER){
        storeError(ERR_AMC,
                   0x01); // Error 0x01 -> AMC submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }
    /* Call the correct handler */
    (amcModulesHandler[currentAmcModule])();

}

/* Gate A Voltage Handler */
/* This function deals with all the monitor requests directed to the AMC gate A
   voltage. There are no control messages allowed for the gate A voltage. */
static void gateAVoltageHandler(void){

    #ifdef DEBUG
        printf("     Gate A Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the amc A gate voltage */
    if(getAmc(AMC_GATE_A_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      gateAVoltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        gateAVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateAVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateAVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateAVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateAVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x04); // Error 0x04: Error: gate A voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x05); // Error 0x05: Warning: gate A voltage in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;

}

/* Drain A Voltage Handler */
/* This function deals with all the monitor requests directed to the AMC drain A
   voltage. There are no control messages allowed for the drain A voltage. */
static void drainAVoltageHandler(void){

    #ifdef DEBUG
        printf("     Drain A Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the amc A drain voltage */
    if(getAmc(AMC_DRAIN_A_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainAVoltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        drainAVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainAVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainAVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainAVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainAVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x06); // Error 0x06: Error: drain A voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x07); // Error 0x07: Warning: drain A voltage in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Drain A Current Handler */
/* This function deals with all the monitor requests directed to the AMC drain A
   current. There are no control messages allowed for the drain A current. */
static void drainACurrentHandler(void){

    #ifdef DEBUG
        printf("     Drain A Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the AMC drain A current */
    if(getAmc(AMC_DRAIN_A_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainACurrent[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        drainACurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainACurrent[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainACurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainACurrent[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainACurrent[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x08); // Error 0x08: Error: drain A current in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x09); // Error 0x09: Warning: drain A current in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Gate B Voltage Handler */
/* This function deals with all the monitor requests directed to the AMC gate B
   voltage. There are no control messages allowed for the gate B voltage. */
static void gateBVoltageHandler(void){

    #ifdef DEBUG
        printf("     Gate B Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the amc B gate voltage */
    if(getAmc(AMC_GATE_B_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      gateBVoltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        gateBVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateBVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateBVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateBVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateBVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x0A); // Error 0x0A: Error: gate B voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x0B); // Error 0x0B: Warning: gate B voltage in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;

}

/* Drain B Voltage Handler */
/* This function will deal with monitor and control requests to the drain b
   voltage. */
static void drainBVoltageHandler(void){

    #ifdef DEBUG
        printf("     Drain B Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainBVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           amc.
            lastDrainBVoltage.
             status=NO_ERROR;

        /* Extract the float from the can message */
        changeEndian(convert.
                      chr,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBVoltage[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBVoltage[MAX_SET_VALUE])){
                storeError(ERR_AMC,
                           0x0C); // Error 0x0C: Drain B voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainBVoltage.
                     status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the AMC drain B voltage. If an error occurs then store the state
           and return the error state then return. */
        if(setAmc(AMC_DRAIN_B_VOLTAGE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                lastDrainBVoltage.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainBVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the AMC drain B voltage */
    if(getAmc(AMC_DRAIN_B_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainBVoltage[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainBVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */

        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainBVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainBVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x0D); // Error 0x0D: Error: AMC drain B voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x0E); // Error 0x0E: Warning: AMC drain B voltage in warning range.
                    /* Store the state in the outgoing CAN message */
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}


/* Drain B Current Handler */
/* This function deals with all the monitor requests directed to the AMC drain B
   current. There are no control messages allowed for the drain B current. */
static void drainBCurrentHandler(void){

    #ifdef DEBUG
        printf("     Drain B Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the AMC drain B current */
    if(getAmc(AMC_DRAIN_B_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainBCurrent[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        drainBCurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBCurrent[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainBCurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainBCurrent[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainBCurrent[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x0F); // Error 0x0F: Error: drain B current in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x10); // Error 0x10: Warning: drain B current in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Multiplier D Voltage Handler */
/* This function will deal with monitor and control requests to the multiplier
   D voltage. */
static void multiplierDVoltageHandler(void){

    #ifdef DEBUG
        printf("     Multiplier D Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastMultiplierDVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           amc.
            lastMultiplierDVoltage.
             status=NO_ERROR;

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              multiplierDVoltage[MIN_SET_VALUE],
                          CAN_BYTE,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              multiplierDVoltage[MAX_SET_VALUE])){
                storeError(ERR_AMC,
                           0x11); // Error 0x11: Multiplier D voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastMultiplierDVoltage.
                     status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the AMC multiplier D voltage. If an error occurs then store the state
           and return the error state then return. */
        if(setAmc(AMC_MULTIPLIER_D_VOLTAGE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                lastMultiplierDVoltage.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastMultiplierDVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the multiplier D voltage tune is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                amc.
                 multiplierDVoltage[CURRENT_VALUE];

    CAN_SIZE=CAN_BYTE_SIZE;
}

/* Multiplier D Current Handler */
/* This function will deal with monitor requests to the multiplier D current. */
static void multiplierDCurrentHandler(void){

    #ifdef DEBUG
        printf("     Multiplier D Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the AMC multiplier D voltage */
    if(getAmc(AMC_MULTIPLIER_D_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      multiplierDCurrent[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      multiplierDCurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              multiplierDCurrent[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              multiplierDCurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  multiplierDCurrent[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  multiplierDCurrent[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x20); // Error 0x20: Error: AMC multiplier D current in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x21); // Error 0x23: Warning: AMC multiplier D current in warning range.
                    /* Store the state in the outgoing CAN message */
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Gate E Voltage Handler */
/* This function will deal with monitor and control requests to the gate E
   voltage. */
static void gateEVoltageHandler(void){

    #ifdef DEBUG
        printf("     Gate E Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastGateEVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           amc.
            lastGateEVoltage.
             status=NO_ERROR;

        /* Extract the float from the can message */
        changeEndian(convert.
                      chr,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateEVoltage[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateEVoltage[MAX_SET_VALUE])){
                storeError(ERR_AMC,
                           0x14); // Error 0x14: Gate E voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastGateEVoltage.
                     status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the AMC gate E voltage. If an error occurs then store the state
           and return the error state then return. */
        if(setAmc(AMC_GATE_E_VOLTAGE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                lastGateEVoltage.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastGateEVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the AMC gate E voltage */
    if(getAmc(AMC_GATE_E_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      gateEVoltage[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      gateEVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateEVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              gateEVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateEVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  gateEVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x15); // Error 0x15: Error: AMC gate E voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x16); // Error 0x16: Warning: AMC gate E voltage in warning range.
                    /* Store the state in the outgoing CAN message */
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Drain E Voltage Handler */
/* This function will deal with monitor and control requests to the gate E
   voltage. */
static void drainEVoltageHandler(void){

    #ifdef DEBUG
        printf("     Drain E Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainEVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           amc.
            lastDrainEVoltage.
             status=NO_ERROR;

        /* Extract the float from the can message */
        changeEndian(convert.
                      chr,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainEVoltage[MIN_SET_VALUE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainEVoltage[MAX_SET_VALUE])){
                storeError(ERR_AMC,
                           0x17); // Error 0x17: Drain E voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainEVoltage.
                     status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the AMC drain E voltage. If an error occurs then store the state
           and return the error state then return. */
        if(setAmc(AMC_DRAIN_E_VOLTAGE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               amc.
                lastDrainEVoltage.
                 status=ERROR;

            return;
        }

        /* If everything went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){ // If monitor on a control RCA
        /* Return last issued control command */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   amc.
                    lastDrainEVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the AMC drain E voltage */
    if(getAmc(AMC_DRAIN_E_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainEVoltage[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainEVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainEVoltage[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainEVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainEVoltage[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainEVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x18); // Error 0x18: Error: AMC drain E voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x19); // Error 0x18: Warning: AMC drain E voltage in warning range.
                    /* Store the state in the outgoing CAN message */
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Drain E Current Handler */
/* This function deals with all the monitor requests directed to the AMC drain E
   current. There are no control messages allowed for the drain E current. */
static void drainECurrentHandler(void){

    #ifdef DEBUG
        printf("     Drain E Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the AMC drain E current */
    if(getAmc(AMC_DRAIN_E_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      drainECurrent[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        drainECurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainECurrent[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              drainECurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainECurrent[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  drainECurrent[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x1A); // Error 0x1A: Error: drain E current in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x1B); // Error 0x1B: Warning: drain E current in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

/* Supply Voltage 5V Handler */
/* This function deals with all the monitor requests directed to the AMC 5V
   supply voltage. There are no control messages allowed for the 5V supply
   voltage. */
static void supplyVoltage5VHandler(void){

    #ifdef DEBUG
        printf("     Supply Voltage 5V\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_AMC,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_AMC,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the AMC 5V supply voltage */
    if(getAmc(AMC_5V_SUPPLY_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     amc.
                      supplyVoltage5V[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       amc.
                        supplyVoltage5V[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              supplyVoltage5V[LOW_WARNING_RANGE],
                          CAN_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             amc.
                              supplyVoltage5V[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  supplyVoltage5V[LOW_ERROR_RANGE],
                              CAN_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 amc.
                                  supplyVoltage5V[HI_ERROR_RANGE])){
                    storeError(ERR_AMC,
                               0x1E); // Error 0x1E: Error: 5V supply voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_AMC,
                               0x1F); // Error 0x1F: Warning: 5V supply voltage in warning range
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
                 convert.
                  chr);
    CAN_SIZE=CAN_FLOAT_SIZE;
}

