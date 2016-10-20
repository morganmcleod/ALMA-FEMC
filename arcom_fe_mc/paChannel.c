/*! \file   paChannel.c
    \brief  PA Channel functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    <b> CVS informations: </b><br>
    \$Id: paChannel.c,v 1.26 2012/04/13 15:36:39 avaccari Exp $

    This files contains all the functions necessary to handle the PA Channel
    events.

    The information on the mapping between PA channels and polarizations are
    encoded in this module.

    \todo The mapping information are not complete, there are still few band
          missing. Once the data is available, this module should be updated
          accordingly. There are extra check performed on the static variable
          paMapped to return warning in case an unmapped channel is used. This
          code could be deleted after all channels are mapped. */

/* Includes */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "error.h"
#include "frontend.h"
#include "loSerialInterface.h"
#include "debug.h"
#include "database.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
unsigned char   currentPaChannelModule=0;
/* Statics */
/* A static to deal with the mapping of the PA cahnnels. This global variable is
   used to indicate if the mapping is defined or not. */
static unsigned char paMapped = FALSE;
static HANDLER  paChannelModulesHandler[PA_CHANNEL_MODULES_NUMBER]={gateVoltageHandler,
                                                                    drainVoltageHandler,
                                                                    drainCurrentHandler};

/* PA Channel handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the PA Channel. */
void paChannelHandler(void){

    #ifdef DEBUG
    printf("     PA Channel: %d (mapped to Polarization: %d)\n",
               currentPaModule,
               currentPaChannel());
    #endif /* DEBUG */

    /* Since the LO is always outfitted with all the modules, no hardware check
       is performed. */

    /* Check if the submodule is in range */
    currentPaChannelModule=(CAN_ADDRESS&PA_CHANNEL_MODULES_RCA_MASK);
    if(currentPaChannelModule>=PA_CHANNEL_MODULES_NUMBER){
        storeError(ERR_PA_CHANNEL,
                   0x01); // Error 0x01 -> PA channel submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        return;
    }


/****** If it is a control message, we should check that the temperature of the
4k mixers is < 30k before allowing any control. If it not, notify, store the
hardware blocked error and return. */



    /* Call the correct handler */
    (paChannelModulesHandler[currentPaChannelModule])();
}

/* Gate Voltage Handler */
/* This function will deal with monitor and control requests to the gate
   voltage. */
static void gateVoltageHandler(void){

    #ifdef DEBUG
        printf("      Gate Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pa.
                    paChannel[currentPaChannel()].
                     lastGateVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           pa.
            paChannel[currentPaChannel()].
             lastGateVoltage.
              status=NO_ERROR;

        /* Extract the float from the can message */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               gateVoltage[MIN_SET_VALUE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               gateVoltage[MAX_SET_VALUE])){
                storeError(ERR_PA_CHANNEL,
                           0x02); // Error 0x02: Gate voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   pa.
                    paChannel[currentPaChannel()].
                     lastGateVoltage.
                      status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* Set the PA channel gate voltage. If an error occurs then store the
           state and return the error state then return. */
        if(1,2,3()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastGateVoltage.
                  status=ERROR;

            return;
        }

        /* Check if the used PA channel was mapped, if not, store the error. */
        if(paMapped==FALSE){
            storeError(ERR_PA_CHANNEL,
                       0x0C); // Error 0x0C -> Warning: The addressed hardware is not properly defined yet
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastGateVoltage.
                  status=HARDW_UPD_WARN;
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
                   pa.
                    paChannel[currentPaChannel()].
                     lastGateVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the PA channel gate voltage */
    if(getPaChannel()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      paChannel[currentPaChannel()].
                       gateVoltage[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      paChannel[currentPaChannel()].
                       gateVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               gateVoltage[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               gateVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   gateVoltage[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   gateVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_PA_CHANNEL,
                               0x03); // Error 0x03: Error: PA channel gate voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PA_CHANNEL,
                               0x04); // Error 0x04: Warning: PA channel gate voltage in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Check if the used PA channel was mapped, if not, store the error. */
    if(paMapped==FALSE){
        storeError(ERR_PA_CHANNEL,
                   0x0C); // Error 0x0C -> Warning: The addressed hardware is not properly defined yet
        /* Store the state in the outgoing CAN message */
        CAN_STATUS=HARDW_UPD_WARN;
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

}

/* Drain Voltage Handler */
/* This function will deal with monitor and control requests to the drain
   voltage. */
static void drainVoltageHandler(void){

    float temp4K,temp12K;
    unsigned int state=DISABLE;
    signed char ret;

    #ifdef DEBUG
        printf("      Drain Voltage\n");
    #endif /* DEBUG */

    /* If control (size !=0) */
    if(CAN_SIZE){
        /* If the temperature of the dewar is >30K, don't allow the use of the
           PAs as too much power at this temperature could damage the
           multipliers. In the LO async loop, the PAs will be turned off if the
           dewar temepratures grows to above 30K. */
        temp4K=frontend.
                cryostat.
                 cryostatTemp[CRYOCOOLER_4K].
                  temp[CURRENT_VALUE];
        temp12K=frontend.
                 cryostat.
                  cryostatTemp[CRYOCOOLER_12K].
                   temp[CURRENT_VALUE];

        /* We check the 4K sensor first and if that doesn't allow the command,
           we check the 12K sensor next.   Previous algorithm in 2.6.1 checked
           the 12K sensor first and would always reject if that one sensor 
           was above PA_MAX_ALLOWED_TEMP. */

        // start by assuming the command will be blocked:
        state=DISABLE;

        // is the 4K cryostat sensor reading valid?
        if((temp4K != FLOAT_ERROR) && (temp4K != FLOAT_UNINIT)) {
            // yes, is it a reasonable real-world temperature?            
            if (PA_TEMP_SANITY_CHECK(temp4K)) {
                // yes, is it low enough to allow the PA to be enabled?
                if (temp4K <= PA_MAX_ALLOWED_TEMP)
                    state=ENABLE;   // yes.
            }
        }

        // may need to use the 12K sensor instead:
        if (state == DISABLE) {
            // is the 12K cryostat sensor reading valid?
            if((temp12K != FLOAT_ERROR) && (temp12K != FLOAT_UNINIT)) {
                // yes, is it a reasonable real-world temperature?            
                if (PA_TEMP_SANITY_CHECK(temp12K)) {
                    // yes, is it low enough to allow the PA to be enabled?
                    if (temp12K <= PA_MAX_ALLOWED_TEMP)
                        state=ENABLE;   // yes.
                }
            }
        }            

        // MTM commented out cryostat temp check algorithm from 2.6.1.
        // 
        // if((temp12K==FLOAT_ERROR)||(temp12K==FLOAT_UNINIT)){
        //     if((temp4K==FLOAT_ERROR)||(temp4K==FLOAT_UNINIT)||(temp4K>PA_MAX_ALLOWED_TEMP)){
        //         state=DISABLE;
        //     } else {
        //         state=ENABLE;
        //     }
        // } else {
        //     if(temp12K>PA_MAX_ALLOWED_TEMP){
        //         state=DISABLE;
        //     } else {
        //         state=ENABLE;
        //     }
        // }

        // If we are in TROUBLESHOOTING mode, ignore all the above safety checks and allow the voltage to be set:
        if (frontend.mode[CURRENT_VALUE] == TROUBLESHOOTING_MODE)
            state=ENABLE;

        if(state==DISABLE){
            storeError(ERR_PA_CHANNEL,
                       0x0D); // Error 0x0D -> PA temperature above the allowed range -> PAs disabled

            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastDrainVoltage.
                  status=HARDW_BLKD_ERR; // Store the status in the last control message

            return;
        }

        /* Store message in "last control message" location */
        memcpy(&frontend.
                 cartridge[currentModule].
                  lo.
                   pa.
                    paChannel[currentPaChannel()].
                     lastDrainVoltage,
               &CAN_SIZE,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        /* Overwrite the last control message status with the default NO_ERROR
           status. */
        frontend.
         cartridge[currentModule].
          lo.
           pa.
            paChannel[currentPaChannel()].
             lastDrainVoltage.
              status=NO_ERROR;

        /* Extract the float from the can message */
        changeEndian(CONV_CHR_ADD,
                     CAN_DATA_ADD);

        /* Check the value against the store limits. The limits are read from
           the configuration database at configuration time. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainVoltage[MIN_SET_VALUE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainVoltage[MAX_SET_VALUE])){
                storeError(ERR_PA_CHANNEL,
                           0x05); // Error 0x05: Gate voltage set value out of range

                /* Store error in the last control message variable */
                frontend.
                 cartridge[currentModule].
                  lo.
                   pa.
                    paChannel[currentPaChannel()].
                     lastDrainVoltage.
                      status=CON_ERROR_RNG;

                return;
            }
        #endif /* DATABASE_RANGE */

        /* if not in TROUBLESHOOTING mode, 
            Limit the CONV_FLOAT value about to be sent to the PA channel for drain voltage
            to the safe maximum for the current YTO tuning.
           Returns HARDW_BLKD_ERR if the value was reduced. */
        if (frontend.mode[CURRENT_VALUE] == TROUBLESHOOTING_MODE)
            ret = NO_ERROR;
        else
            ret = limitSafePaDrainVoltage(currentPaModule);

        if (ret != NO_ERROR) {
            // report that the limit was violated:
            storeError(ERR_PA_CHANNEL,
                       0x0E); // Error 0x0E -> Warning: Attempted to set LO PA above max safe power level.

            /* save the modified command setting to the "last control message" location */
            changeEndian(frontend.
                          cartridge[currentModule].
                           lo.
                            pa.
                             paChannel[currentPaChannel()].
                              lastDrainVoltage.
                               data,
                         CONV_CHR_ADD);
        }

        /* Set the PA channel drain voltage. If an error occurs then store the
           state and then return. */
        if(setPaChannel()==ERROR){
            /* Store the ERROR state in the last control message variable */
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastDrainVoltage.
                  status=ERROR;
            return;
        }

        /* Check if the used PA channel was mapped, if not, store the error. */
        if(paMapped==FALSE){
            storeError(ERR_PA_CHANNEL,
                       0x0C); // Error 0x0C -> Warning: The addressed hardware is not properly defined yet
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastDrainVoltage.
                  status=HARDW_UPD_WARN;
        
        /* if limitSafePaDrainVoltage() above returned a problem, we want to save that error status */
        } else {
            frontend.
             cartridge[currentModule].
              lo.
               pa.
                paChannel[currentPaChannel()].
                 lastDrainVoltage.
                  status=ret;
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
                   pa.
                    paChannel[currentPaChannel()].
                     lastDrainVoltage,
               CAN_LAST_CONTROL_MESSAGE_SIZE);

        return;
    }

    /* If monitor on a monitor RCA */
    /* Monitor the PA channel drain voltage */
    if(getPaChannel()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      paChannel[currentPaChannel()].
                       drainVoltage[CURRENT_VALUE];
    } else {

        /* If no error during the monitor process gather the stored data */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      paChannel[currentPaChannel()].
                       drainVoltage[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out a warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATBASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainVoltage[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainVoltage[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   drainVoltage[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   drainVoltage[HI_ERROR_RANGE])){
                    storeError(ERR_PA_CHANNEL,
                               0x06); // Error 0x06: Error: PA channel drain voltage in error range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PA_CHANNEL,
                               0x07); // Error 0x07: Warning: PA channel drain voltage in warning range.
                    /* Store the state in the outgoing CAN message */
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Check if the used PA channel was mapped, if not, store the error. */
    if(paMapped==FALSE){
        storeError(ERR_PA_CHANNEL,
                   0x0C); // Error 0x0C -> Warning: The addressed hardware is not properly defined yet
        /* Store the state in the outgoing CAN message */
        CAN_STATUS=HARDW_UPD_WARN;
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

}

/* Drain Current Handler */
/* This function deals with all the monitor requests directed to the PA channel
   drain current. There are no control messages allowed for the PA channel drain
   current. */
static void drainCurrentHandler(void){

    #ifdef DEBUG
        printf("      Drain Current\n");
    #endif /* DEBUG */

    /* If control (size !=0) store error and return. No control message are
       allowed on this RCA. */
    if(CAN_SIZE){
        storeError(ERR_PA_CHANNEL,
                   0x08); // Error 0x08: Control message out of range
        return;
    }

    /* If monitor on control RCA return error since there are no control
       messages allowed on this RCA. */
    if(currentClass==CONTROL_CLASS){ // If monitor on control RCA
        storeError(ERR_PA_CHANNEL,
                   0x09); // Error 0x09: Monitor message out of range
        /* Store the state in the outgoing CAN message */
        CAN_STATUS = MON_CAN_RNG;

        return;
    }

    /* Monitor the PA channel drain current */
    if(getPaChannel()==ERROR){
        /* If error during monitoring, store the ERROR state in the outgoing
           CAN message state. */
        CAN_STATUS = ERROR;
        /* Store the last known value in the outgoing message */
        CONV_FLOAT=frontend.
                   cartridge[currentModule].
                    lo.
                     pa.
                      paChannel[currentPaChannel()].
                       drainCurrent[CURRENT_VALUE];
    } else {
        /* If no error during monitor process, gather the stored data */
        CONV_FLOAT = frontend.
                     cartridge[currentModule].
                      lo.
                       pa.
                        paChannel[currentPaChannel()].
                         drainCurrent[CURRENT_VALUE];

        /* Check the result against the warning and error range. Right now
           this function is only printing out an warning/error message
           depending on the result but no actions are taken. */
        #ifdef DATABASE_RANGE
            if(checkRange(frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainCurrent[LOW_WARNING_RANGE],
                          CONV_FLOAT,
                          frontend.
                           cartridge[currentModule].
                            lo.
                             pa.
                              paChannel[currentPaChannel()].
                               drainCurrent[HI_WARNING_RANGE])){
                if(checkRange(frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   drainCurrent[LOW_ERROR_RANGE],
                              CONV_FLOAT,
                              frontend.
                               cartridge[currentModule].
                                lo.
                                 pa.
                                  paChannel[currentPaChannel()].
                                   drainCurrent[HI_ERROR_RANGE])){
                    storeError(ERR_PA_CHANNEL,
                               0x0A); // Error 0x0A: Error: PA channel drain currrent in error range
                    CAN_STATUS = MON_ERROR_RNG;
                } else {
                    storeError(ERR_PA_CHANNEL,
                               0x0B); // Error 0x0B: Warning: PA channel drain currrent in warning range
                    CAN_STATUS = MON_WARN_RNG;
                }
            }
        #endif /* DATABASE_RANGE */
    }

    /* Check if the used PA channel was mapped, if not, store the error. */
    if(paMapped==FALSE){
        storeError(ERR_PA_CHANNEL,
                   0x0C); // Error 0x0C -> Warning: The addressed hardware is not properly defined yet
        /* Store the state in the outgoing CAN message */
        CAN_STATUS=HARDW_UPD_WARN;
    }

    /* Load the CAN message payload with the returned value and set the
       size. The value has to be converted from little endian (Intel) to
       big endian (CAN). It is done directly instead of using a function
       to save some time. */
    changeEndian(CAN_DATA_ADD,
                 CONV_CHR_ADD);
    CAN_SIZE=CAN_FLOAT_SIZE;

}


/* Current PA channel */
/*! This functions returns the current PA_CHANNEL as expressed by:
        - \ref PA_CHANNEL_A -> Current active PA_CHANNEL is channel A
        - \ref PA_CHANNEL_B -> Current active PA_CHANNEL is channel B

    This is necessary because the mapping between 'A' and 'B' channel and
    polarizations '0' and '1' is not consistend throught the cartridges.
    The following is the current mapping:
    <TABLE>
     <CAPTION>
      <SMALL>Polarization to channel mapping for the different bands.</SMALL>
     </CAPTION>
     <TR>
      <TH>Bands<TH>Polarization<TH>Channel
     <TR>
      <TD>1,2<TD>0...1<TD>N/D
     <TR>
      <TD>3,4,8,9,10<TD>0<TD>B
     <TR>
      <TD>3,4,8,9,10<TD>1<TD>A
     <TR>
      <TD>5,6,7<TD>0<TD>A
     <TR>
      <TD>5,6,7<TD>1<TD>B
    </TABLE>

    \return
        - \ref PA_CHANNEL_A
        - \ref PA_CHANNEL_B */
int currentPaChannel(void){
    /* Switch on cartridge number */
    switch(currentModule){
        case BAND3:
        case BAND4:
        case BAND8:
        case BAND9:
        case BAND10:
            paMapped = TRUE;
            return currentPaModule==0?PA_CHANNEL_B:
                                      PA_CHANNEL_A;
            break;

        case BAND1: // Band 1 preproduction.
                    // TODO: Assign correctly for 2.8.x
        case BAND2: // Band 2 prototype.
                    // TODO: Assign correctly for 2.8.x
        case BAND5:
        case BAND6:
        case BAND7:
            paMapped = TRUE;
            return currentPaModule==0?PA_CHANNEL_A:
                                      PA_CHANNEL_B;
            break;
        /* The following are assumption on the bands we don't have information
           about at this point: 1 and 2. When the information are available,
           this table should be updated. */
        default:
            paMapped = FALSE;
            return currentPaModule==0?PA_CHANNEL_A:
                                      PA_CHANNEL_B;
            break;
    }
}



