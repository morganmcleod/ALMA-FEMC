/*! \file   pll.c
    \brief  PLL functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to handle the PLL events. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "loSerialInterface.h"
#include "debug.h"

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
        storeError(ERR_PLL, ERC_MODULE_RANGE); //PLL submodule out of range
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      lockDetectVoltage;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        lockDetectVoltage;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      correctionVoltage;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        correctionVoltage;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      assemblyTemp;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        assemblyTemp;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      YIGHeaterCurrent;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        YIGHeaterCurrent;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      refTotalPower;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        refTotalPower;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                      ifTotalPower;
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pll.
                        ifTotalPower;
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
        storeError(ERR_PLL, ERC_RCA_RANGE); //Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
                     unlockDetectLatch;
    } else {
        /* If no error during monitor process, gather the stored data */
        CAN_BYTE=frontend.
                  cartridge[currentModule].
                   lo.
                    pll.
                     unlockDetectLatch;
    }
    /* Load the CAN message payload with the returned value and set the size. */
    CAN_BYTE=frontend.
              cartridge[currentModule].
               lo.
                pll.
                 unlockDetectLatch;
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
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    lo.
                                     pll.
                                      lastClearUnlockDetectLatch)

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
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      lo.
                                       pll.
                                        lastClearUnlockDetectLatch)
        return;
    }

    /* If monitor on a monitor RCA */
    /* Return error, no monitor messages are allowed on this RCA. */
    storeError(ERR_PLL, ERC_RCA_RANGE); //Monitor message out of range
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
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    lo.
                                     pll.
                                      lastLoopBandwidthSelect)

        /* Change the status of the PLL loop BW according to the content of the CAN message. */
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
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      lo.
                                       pll.
                                        lastLoopBandwidthSelect)
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
                 loopBandwidthSelect;
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
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    lo.
                                     pll.
                                      lastSidebandLockPolaritySelect)

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
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      lo.
                                       pll.
                                        lastSidebandLockPolaritySelect)
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
                 sidebandLockPolaritySelect;
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
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   cartridge[currentModule].
                                    lo.
                                     pll.
                                      lastNullLoopIntegrator)

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
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     cartridge[currentModule].
                                      lo.
                                       pll.
                                        lastNullLoopIntegrator)
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
                 nullLoopIntegrator;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

