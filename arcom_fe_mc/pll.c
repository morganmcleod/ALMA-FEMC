/*! \file   pll.c
    \brief  PLL functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pll.c,v 1.25 2009/08/25 21:39:39 avaccari Exp $

    This files contains all the functions necessary to handle the PLL events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "loSerialInterface.h"
#include "debug.h"
#include "database.h"

/* Globals */
/* Externs */
unsigned char   currentPllModule=0;
/* Statics */
static HANDLER  pllModulesHandler[PLL_MODULES_NUMBER]={lockDetectVoltageHandler,
                                                       correctionVoltageHandler,
                                                       assemblyTempHandler,
                                                       YIGHeaterCurrentHandler,
                                                       refTotalPowerHandler,
                                                       ifTotalPowerHandler,
                                                       bogoFunction,
                                                       unlockDetectLatchHandler,
                                                       clearUnlockDetectLatchHandler,
                                                       loopBandwidthSelectHandler,
                                                       sidebandLockPolaritySelectHandler,
                                                       nullLoopIntegratorHandler};

/* PLL handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the PLL. */
void pllHandler(void){

    #ifdef DEBUG
        printf("    PLL\n");
    #endif /* DEBUG */

    /* Since the LO is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentPllModule=(CAN_ADDRESS&PLL_MODULES_RCA_MASK);
    if(currentPllModule>=PLL_MODULES_NUMBER){
        storeError(ERR_PLL,
                   0x01); // Error 0x01 -> PLL submodule out of range

        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }
    /* Call the correct handler */
    (pllModulesHandler[currentPllModule])();
}

/* Lock Detect Voltage Handler */
/* This function deals with all the monitor requests directed to the PLL lock
   detect voltage. There are no control messages allowed for the lock detect
   voltage. */
static void lockDetectVoltageHandler(void){

    #ifdef DEBUG
        printf("     Lock Detect Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PLL lock detect voltage */
    if(getPll(PLL_LOCK_DETECT_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      lockDetectVoltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        lockDetectVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              lockDetectVoltage[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              lockDetectVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  lockDetectVoltage[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  lockDetectVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x04); // Error 0x04: Error: lock detect voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x05); // Error 0x05: Warning: lock detect voltage in warning range
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

/* Correction Voltage Handler */
/* This function deals with all the monitor requests directed to the PLL
   correction voltage. There are no control messages allowed for the correction
   voltage. */
static void correctionVoltageHandler(void){

    #ifdef DEBUG
        printf("     Correction Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PLL correction voltage */
    if(getPll(PLL_CORRECTION_VOLTAGE)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      correctionVoltage[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        correctionVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              correctionVoltage[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              correctionVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  correctionVoltage[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  correctionVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x06); // Error 0x06: Error: correction voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x07); // Error 0x07: Warning: correction voltage in warning range
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

/* Assembly Temperature Handler */
/* This function deals with all the monitor requests directed to the PLL
   assembly temperature. There are no control messages allowed for the assembly
   temperature. */
static void assemblyTempHandler(void){

    #ifdef DEBUG
        printf("     Assembly Temperature\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PLL assembly temperature */
    if(getPll(PLL_ASSEMBLY_TEMP)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      assemblyTemp[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        assemblyTemp[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              assemblyTemp[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              assemblyTemp[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  assemblyTemp[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  assemblyTemp[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x08); // Error 0x08: Error: assembly temperature in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x09); // Error 0x09: Warning: assembly temperature in warning range
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

/* YIG Heater Current Handler */
/* This function deals with all the monitor requests directed to the PLL
   YIG heater current. There are no control messages allowed for the YIG heater
   current. */
static void YIGHeaterCurrentHandler(void){

    #ifdef DEBUG
        printf("     YIG Heater Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the YIGHeaterCurrent supply voltage */
    if(getPll(PLL_YIG_HEATER_CURRENT)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      YIGHeaterCurrent[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        YIGHeaterCurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              YIGHeaterCurrent[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              YIGHeaterCurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  YIGHeaterCurrent[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  YIGHeaterCurrent[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x0A); // Error 0x0A: Error: YIG heater current voltage in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x0B); // Error 0x0B: Warning: YIG heater current in warning range
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

/* REF Total Power Handler */
/* This function deals with all the monitor requests directed to the PLL
   reference total power. There are no control messages allowed for the ref
   total power. */
static void refTotalPowerHandler(void){

    #ifdef DEBUG
        printf("     REF Total Power\n");
    #endif /* DEBUG */


    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PLL reference total power */
    if(getPll(PLL_REF_TOTAL_POWER)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      refTotalPower[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        refTotalPower[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              refTotalPower[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              refTotalPower[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  refTotalPower[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  refTotalPower[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x0C); // Error 0x0C: Error: reference total power in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x0D); // Error 0x0D: Warning: reference total power in warning range
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

/* IF Total Power Handler */
/* This function deals with all the monitor requests directed to the PLL
   if total power. There are no control messages allowed for the if total
   power. */
static void ifTotalPowerHandler(void){

    #ifdef DEBUG
        printf("     IF Total Power\n");
    #endif /* DEBUG */


    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PLL if total power */
    if(getPll(PLL_IF_TOTAL_POWER)==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pll.
                      ifTotalPower[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        ifTotalPower[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              ifTotalPower[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              ifTotalPower[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  ifTotalPower[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  ifTotalPower[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x0E); // Error 0x0E: Error: if total power in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x0F); // Error 0x0F: Warning: if total power in warning range
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






/* Unlock Detect Latch Handler */
/* This function deals with all the monitor requests directed to the PLL
   unlock detect latch. There are no control messages allowed for the unlock
   detect latch. */
static void unlockDetectLatchHandler(void){

    #ifdef DEBUG
        printf("     Unlock Detect Latch\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PLL,
                   0x02); // Error 0x02: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL,
                   0x03); // Error 0x03: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* If monitor on a monitor RCA */
    /* Get the unlock latch bit status */
    if(getPllStates()==ERROR){
        /* If error durign monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CAN_BYTE=frontend.
                  cartridge[currentModule].
                   lo.
                    pll.
                     unlockDetectLatch[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=frontend.
                  cartridge[currentModule].
                   lo.
                    pll.
                     unlockDetectLatch[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken.
           This is a little different than ususal since the comparison is made
           on a boolean value. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              unlockDetectLatch[LOW_WARNING_RANGE],
                          CAN_BYTE,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pll.
                              unlockDetectLatch[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  unlockDetectLatch[LOW_ERROR_RANGE],
                              CAN_BYTE,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pll.
                                  unlockDetectLatch[HI_ERROR_RANGE])){
                    storeError(ERR_PLL,
                               0x10); // Error 0x10: Error: unlock detect latch bit in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PLL,
                               0x11); // Error 0x11: Warning: unlock detect latchbit in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Load the CAN message payload with the returned value and set the size. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                pll.
                 unlockDetectLatch[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;


}

/* Clear Unlock Detect Latch Handler */
/* This function deals with all the monitor and control requests directed to the
   PLL clear unlock detect latch. */
static void clearUnlockDetectLatchHandler(void){

    #ifdef DEBUG
        printf("     Clear Unlock Detect Latch Handler\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastClearUnlockDetectLatch,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status .*/
        frontend.
         cartridge[currentModule].
          lo.
           pll.
            lastClearUnlockDetectLatch.
             status=NO_ERROR;

        /* Change the status of the PLL unlock detect latch according to the
           content of the CAN message. */
        if(setClearUnlockDetectLatch()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                lastClearUnlockDetectLatch.
                 status=ERROR;

            return;
        }

        /* If everyting went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastClearUnlockDetectLatch,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Return error, no monitor messages are allowed on this RCA. */
    storeError(ERR_PLL,
               0x03); // Error 0x03 -> Monitor message out of range
    CAN_STATUS=MON_CAN_RNG;
}

/* Loop Bandwidth Select Handler */
/* This function deals with all the monitor and control requests directed to the
   PLL loop bandwidth select handler. */
static void loopBandwidthSelectHandler(void){

    #ifdef DEBUG
        printf("     Loop Bandwidth Select Handler\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastLoopBandwidthSelect,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status .*/
        frontend.
         cartridge[currentModule].
          lo.
           pll.
            lastLoopBandwidthSelect.
             status=NO_ERROR;

        /* Change the status of the PLL unlock detect latch according to the
           content of the CAN message. */
        if(setLoopBandwidthSelect(CAN_BYTE?PLL_LOOP_BANDWIDTH_ALTERNATE:
                                           PLL_LOOP_BANDWIDTH_DEFAULT)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                lastLoopBandwidthSelect.
                 status=ERROR;

            return;
        }

        /* If everyting went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastLoopBandwidthSelect,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                pll.
                 loopBandwidthSelect[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

/* Sideband Lock Polarity Select Handler */
/* This function deals with all the monitor and control requests directed to the
   PLL sideband lock polarity select. */
static void sidebandLockPolaritySelectHandler(void){

    #ifdef DEBUG
        printf("     Sideband Lock Polarity Select Handler\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastSidebandLockPolaritySelect,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status .*/
        frontend.
         cartridge[currentModule].
          lo.
           pll.
            lastSidebandLockPolaritySelect.
             status=NO_ERROR;

        /* Change the status of the PLL unlock detect latch according to the
           content of the CAN message. */
           if(setSidebandLockPolaritySelect(CAN_BYTE?PLL_SIDEBAND_LOCK_POLARITY_USB:
                                                     PLL_SIDEBAND_LOCK_POLARITY_LSB)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                lastSidebandLockPolaritySelect.
                 status=ERROR;

            return;
        }

        /* If everyting went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastSidebandLockPolaritySelect,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                pll.
                 sidebandLockPolaritySelect[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;

}

/* Null Loop Integrator Handler */
/* This function deals with all the monitor and control requests directed to the
   PLL null loop integrator. */
static void nullLoopIntegratorHandler(void){

    #ifdef DEBUG
        printf("     Null Loop Integrator Handler\n");
    #endif /* DEBUG */

    /* Check direction and perform the required operation */
    if(CAN_SIZE){ // If control (size !=0)
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastNullLoopIntegrator,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status .*/
        frontend.
         cartridge[currentModule].
          lo.
           pll.
            lastNullLoopIntegrator.
             status=NO_ERROR;

        /* Change the status of the PLL unlock detect latch according to the
           content of the CAN message. */
           if(setNullLoopIntegrator(CAN_BYTE?PLL_NULL_LOOP_INTEGRATOR_NULL:
                                             PLL_NULL_LOOP_INTEGRATOR_OPERATE)==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pll.
                lastNullLoopIntegrator.
                 status=ERROR;

            return;
        }

        /* If everyting went fine, it's a control message, we're done. */
        return;
    }

    /* If monitor on control RCA */
    if(currentClass==CONTROL_CLASS){
        /* Return last issued control command. This automatically copies also
           the state because of the way CAN_LAST_CONTROL_MESSAGE_SIZE is
           initialized */
        memcpy(&CAN_SIZE,
               &frontend.
                 cartridge[currentModule].
                  lo.
                   pll.
                    lastNullLoopIntegrator,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* This monitor point doesn't return an hardware status but just the
       current status that is stored in memory. The memory status is
       update when the state of the mixer bias mode is changed by a
       control command. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                pll.
                 nullLoopIntegrator[CURRENT_VALUE];
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

