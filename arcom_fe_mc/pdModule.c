/*! \file   pdModule.c
    \breif  Power Distribution Module functions

    <b> File Informations: </b><br>
    Created: 2006/10/16 16:18:50 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pdModule.c,v 1.15 2012/01/17 16:30:58 avaccari Exp $

    This files contains all the funcions necessary to handle the submodules of
    cartridge power distribution system. */

/* Includes */
#include <stdio.h>      /* printf */
#include <string.h>     /* memcpy */

#include "debug.h"
#include "frontend.h"
#include "error.h"
#include "pdSerialInterface.h"
#include "database.h"
#include "timer.h"
#include "async.h"

/* Globals */
/* Externs */
unsigned char   currentPdModuleModule=0;
/* Statics */
static HANDLER  pdModuleModulesHandler[PD_MODULE_MODULES_NUMBER]={pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  pdChannelHandler,
                                                                  enableHandler};
/* Power distribution module handler */
/*! This function will be called by the CAN message handler when the received
    message is pertinent to the power distribution modules. */
void pdModuleHandler(void){
    #ifdef DEBUG_POWERDIS
        printf("  Power Distribution Module: %d\n",
               currentPowerDistributionModule);
    #endif /* DEBUG_POWERDIS */

    /* Check if the specified submodule is in range */
    currentPdModuleModule=(CAN_ADDRESS&PD_MODULE_MODULES_RCA_MASK)>>PD_MODULE_MODULES_MASK_SHIFT;
    if(currentPdModuleModule>=PD_MODULE_MODULES_NUMBER){
        storeError(ERR_PD_MODULE, ERC_MODULE_RANGE); //Power distribution module submodule out of range
        CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of error
        return;
    }

    /* Check if the cartridge corresponding to the selected power distribution
       module is available. If it isn't then notify the incoming CAN message. */
    #ifdef DATABASE_HARDW
       if(frontend.
            cartridge[currentPowerDistributionModule].
             available==UNAVAILABLE){
            storeError(ERR_PD_MODULE, ERC_MODULE_ABSENT); //corresponding cartridge not installed
            CAN_STATUS = HARDW_BLKD_ERR; // Notify the incoming CAN message
            return;
        }
    #endif /* DATABASE_HARDW */
    /* Call the correct handler */
    (pdModuleModulesHandler[currentPdModuleModule])();
}

static int allowStandby2(int module) {
    // STANDBY2 only allowed for certain bands:
    if (module == BAND6)
        return TRUE;

    return FALSE;
}

static int allowPowerOn(int module, int standby2) {
    // Allow up to four to be powered on, so long as one of them is in STANDBY2 mode.

    int avail = frontend.powerDistribution.maxPoweredModules -
                frontend.powerDistribution.poweredModules;

    // not going to STANDBY2 mode so the simple check is enough:
    if (!standby2) {
        if (avail > 0)
            return TRUE;
        else
            return FALSE;
    }

    // going to STANDBY2 mode, either from OFF or ON...
    // Allow STANDBY2 if we are below the max allowed:
    if (frontend.powerDistribution.standby2Modules < MAX_STANDBY2_BANDS_OPERATIONAL)
        return TRUE;
    else
        return FALSE;
}

void printPoweredModuleCounts(void) {
    #ifdef DEBUG_POWERDIS
        printf("    powered=%d/%d standby2=%d/%d\n", 
                frontend.powerDistribution.poweredModules,
                frontend.powerDistribution.maxPoweredModules,
                frontend.powerDistribution.standby2Modules,
                MAX_STANDBY2_BANDS_OPERATIONAL);
    #endif /* DEBUG_POWERDIS */
}


/* Power distribution module enable handler */
static void enableHandler(void) {
    unsigned char cmdStandby2;      
    //!< true if the command is to set STANDBY2 mode
    //!<  or the former state was STANDBY2 mode.

    // State transitions ending in powered-on or STANDBY2 states states:
    static enum {
        ST_UNKNOWN                      = 0,
        ST_CARTRIDGE_OFF__CARTRIDGE_ON  = 1,
        ST_CARTRIDGE_OFF__STANDBY2      = 2,
        ST_STANDBY2__CARTRIDGE_ON       = 3,
        ST_CARTRIDGE_ON__STANDBY2       = 4
    } cmdStateTransition = ST_UNKNOWN;

    #ifdef DEBUG_POWERDIS
        printf("   enable\n");
    #endif /* DEBUG_POWERDIS */

    /* If it's a control message (size !=0) */
    if (CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.
                                   powerDistribution.
                                    pdModule[currentPowerDistributionModule].
                                     lastEnable)

        // If the command is to one of the powered on states:
        if(CAN_BYTE) {

            // Handle constraints:
            //  Cartridge in error state
            //  Illegal CAN payload
            //  STANDBY2 mode only allowed for certain band(s)
            //  Cartridge already in commanded state
            //  Limit number powered on at a time
            //  Limit number in STANDBY2 mode

            // Disallow if the cartridge is in the error state:
            //  only allowed action is to power off
            if((frontend.
                 cartridge[currentPowerDistributionModule].
                  state==CARTRIDGE_ERROR)) {
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status = HARDW_ERROR; // Store in the last CAN message variable
                return;
            }

            // Check command payload:
            switch (CAN_BYTE) {
                case 1:
                    cmdStandby2 = FALSE;
                    break;

                case 2:
                    cmdStandby2 = TRUE;

                    // Is STANDBY2 allowed for this band?
                    if (!allowStandby2(currentPowerDistributionModule)) {
                        // STANDBY2 not allowed:
                        storeError(ERR_PD_MODULE, ERC_COMMAND_VAL);

                        // Store error in the last CAN message variable:
                        frontend.
                         powerDistribution.
                          pdModule[currentPowerDistributionModule].
                           lastEnable.
                            status = HARDW_BLKD_ERR;
                        return;
                    }
                    break;

                default:
                    // illegal pdModule enable command:
                    storeError(ERR_PD_MODULE, ERC_COMMAND_VAL);

                    // Store error in the last CAN message variable:
                    // Its not a HARDW_BLKD_ERR just an illegal value so ERROR.
                    frontend.
                     powerDistribution.
                      pdModule[currentPowerDistributionModule].
                       lastEnable.
                        status = ERROR;                        
                    return;
            }

            // Already in the commanded state?
            if((frontend.
                 cartridge[currentPowerDistributionModule].
                  state != CARTRIDGE_OFF) &&
               (frontend.
                 cartridge[currentPowerDistributionModule].
                  standby2 == cmdStandby2))
            {
                // Yes. nothing to do
                return;
            }

            // Which state transition?
            if (frontend.
                 cartridge[currentPowerDistributionModule].
                  state == CARTRIDGE_OFF)
            {
                // Starting from powered off:
                //  are we going to STANDBY2 state?
                if (cmdStandby2)
                    cmdStateTransition = ST_CARTRIDGE_OFF__STANDBY2;
                else
                    cmdStateTransition = ST_CARTRIDGE_OFF__CARTRIDGE_ON;
            
            } else {
                // Starting from powered on:
                //  are we going to STANDBY2 state?
                if (cmdStandby2)
                    cmdStateTransition = ST_CARTRIDGE_ON__STANDBY2;
                else
                    cmdStateTransition = ST_STANDBY2__CARTRIDGE_ON;
            }

            #ifdef DEBUG_POWERDIS
                printf("    cmdStateTransition=%d\n", cmdStateTransition);
            #endif /* DEBUG_POWERDIS */
            
            // Update maxPoweredModules depending on the FE mode:
            switch(frontend.mode[CURRENT_VALUE]) {
                case TROUBLESHOOTING_MODE:
                    frontend.
                     powerDistribution.
                      maxPoweredModules = MAX_POWERED_BANDS_TROUBLESHOOTING;
                    break;
                default:
                    frontend.
                     powerDistribution.
                      maxPoweredModules = MAX_POWERED_BANDS_OPERATIONAL;
                    break;
            }

            // State transitions power-off to any:
            if (cmdStateTransition == ST_CARTRIDGE_OFF__CARTRIDGE_ON ||
                cmdStateTransition == ST_CARTRIDGE_OFF__STANDBY2) {

                // Check max number powered on:
                if (!allowPowerOn(currentPowerDistributionModule, cmdStandby2)) {
                    // max number of bands powered on:
                    storeError(ERR_PD_MODULE, ERC_HARDWARE_BLOCKED);

                    // Store error in the last CAN message variable:
                    frontend.
                     powerDistribution.
                      pdModule[currentPowerDistributionModule].
                       lastEnable.
                        status = HARDW_BLKD_ERR;

                    return;
                }

                // Turn on the cartridge:
                if (setPdModuleEnable(PD_MODULE_ENABLE) == ERROR) {
                    // Store error in the last CAN message variable:                        
                    frontend.
                     powerDistribution.
                      pdModule[currentPowerDistributionModule].
                       lastEnable.
                        status = ERROR;

                    return;
                }

                // Set the state of the cartrdige to CARTRIDGE_ON (powered but not
                // yet initialized. This state will trigger the initialization by
                // the cartrdige async routine.
                frontend.
                 cartridge[currentPowerDistributionModule].
                  state = CARTRIDGE_ON;
                
                // Force the priority of the async to address the cartrdige next.
                // This will also re-eable the async procedure if it has been
                // disabled via CAN message or console
                asyncState = ASYNC_CARTRIDGE;

                // Increse the number of currently turned on cartridges.
                //  OR STANDBY2 cartridges.
                // This is done here since the initialization is 
                //  going to be performed asynchronously.
                // This prevents turning on too many cartridges 
                //  before each initialization is completed.
                if (cmdStandby2) {
                    // Set the STANDBY2 state to the cartidge:
                    frontend.
                     cartridge[currentPowerDistributionModule].
                      standby2 = TRUE;

                    // Increase the number of STANDBY2 cartridges:
                    frontend.
                     powerDistribution.
                      standby2Modules++;

                    #ifdef DEBUG_POWERDIS
                        printPoweredModuleCounts();
                    #endif /* DEBUG_POWERDIS */

                } else {
                    // Increase the number of powered on cartridges:
                    frontend.
                     powerDistribution.
                      poweredModules++;

                    #ifdef DEBUG_POWERDIS
                        printPoweredModuleCounts();
                    #endif /* DEBUG_POWERDIS */
                }
                return;
            }

            // Handle remaining states:
            switch (cmdStateTransition) {
                case ST_STANDBY2__CARTRIDGE_ON:
                    // The same rules apply as for turning on a fourth cartridge:
                    // before:  on=3, standby2=1
                    // after:   on=4, standby2=0  NOT ALLOWED

                    // before:  on=2, standby2=1
                    // after:   on=3, standby2=0  ALLOWED

                    if (!allowPowerOn(currentPowerDistributionModule, FALSE)) {
                        // max number of bands powered on:
                        storeError(ERR_PD_MODULE, ERC_HARDWARE_BLOCKED);

                        // Store error in the last CAN message variable:
                        frontend.
                         powerDistribution.
                          pdModule[currentPowerDistributionModule].
                           lastEnable.
                            status = HARDW_BLKD_ERR;

                        return;
                    }

                    // Clear the STANDBY2 state of the cartidge:
                    frontend.
                     cartridge[currentPowerDistributionModule].
                      standby2 = FALSE;

                    // Decrease the number of STANDBY2 cartridges:
                    frontend.
                     powerDistribution.
                      standby2Modules--;

                    // Increase the number of powered cartridges:
                    frontend.
                     powerDistribution.
                      poweredModules++;

                    #ifdef DEBUG_POWERDIS
                        printPoweredModuleCounts();
                    #endif /* DEBUG_POWERDIS */

                    return;

                case ST_CARTRIDGE_ON__STANDBY2:
                    // before:  on=3, standby2=1
                    // after:   on=2, standby2=2  NOT ALLOWED

                    // before:  on=3, standby2=0
                    // after:   on=2, standby2=1  ALLOWED

                    // Disallow STANDBY2 if we are at the max allowed:
                    if (frontend.powerDistribution.standby2Modules >= MAX_STANDBY2_BANDS_OPERATIONAL) {
                        // max number of bands powered on:
                        storeError(ERR_PD_MODULE, ERC_HARDWARE_BLOCKED);

                        // Store error in the last CAN message variable:
                        frontend.
                         powerDistribution.
                          pdModule[currentPowerDistributionModule].
                           lastEnable.
                            status = HARDW_BLKD_ERR;

                        return;
                    }
                    // Set the STANDBY2 state of the cartidge:
                    frontend.
                     cartridge[currentPowerDistributionModule].
                      standby2 = TRUE;

                    // Increase the number of STANDBY2 cartridges:
                    frontend.
                     powerDistribution.
                      standby2Modules++;

                    // Decrease the number of powered cartridges:
                    frontend.
                     powerDistribution.
                      poweredModules--;

                    #ifdef DEBUG_POWERDIS
                        printPoweredModuleCounts();
                    #endif /* DEBUG_POWERDIS */

                    // Set the state of the cartrdige to CARTRIDGE_GO_STANDBY2
                    // This state will trigger shutting down cold electronics in
                    // the cartrdige async routine.
                    frontend.
                     cartridge[currentPowerDistributionModule].
                      state = CARTRIDGE_GO_STANDBY2;

                    // Force the priority of the async to address the cartrdige next.
                    // This will also re-eable the async procedure if it has been
                    // disabled via CAN message or console
                    asyncState = ASYNC_CARTRIDGE;
                    return;

                default:
                    // illegal state transtition.  Should never happen.
                    storeError(ERR_PD_MODULE, ERC_DEBUG_ME);
                    return;
            }
        
        // CAN_BYTE == 0: The command is one of the power off transitions:
        } else {

            // Handle constraints:
            //  Cartridge already off?   If so ignore.
            //  Power off from ON vs. STANDBY2 state.   Decerement different counters

            // Check if the cartridge is already powered off and if it is, just return:
            if ((frontend.
                  cartridge[currentPowerDistributionModule].
                   state == CARTRIDGE_OFF)){
                return;
            }

            // Cache whether the cartridge was in STANDBY2 mode prior to cartridgeStop()
            cmdStandby2 = frontend.
                           cartridge[currentPowerDistributionModule].
                            standby2;

            // Stop the cartridge.
            if (cartridgeStop(currentPowerDistributionModule) == ERROR) {
                // If an error occurs while stopping
                //  store the Error state in the last control message variable:
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status = ERROR;
            }

            // Turn off the power distributrion module.
            if (setPdModuleEnable(PD_MODULE_DISABLE) == ERROR) {
                // If an error occurs while stopping
                //  store the Error state in the last control message variable:
                frontend.
                 powerDistribution.
                  pdModule[currentPowerDistributionModule].
                   lastEnable.
                    status=ERROR;

                /* Set the state of the cartridge to 'error'. If this occurs then
                   the knowledge of the state of the hardware is compromised and
                   the only allowed action should be to try again to turn off the
                   cartridge. */
                frontend.
                 cartridge[currentPowerDistributionModule].
                  state=CARTRIDGE_ERROR;

                return;
            }

            // Decrement the counter of currently turned on cartridges.
            if (cmdStandby2) {
                frontend.
                 powerDistribution.
                  standby2Modules--;

            } else {
                frontend.
                 powerDistribution.
                  poweredModules--;
            }

            #ifdef DEBUG_POWERDIS
                printPoweredModuleCounts();
            #endif /* DEBUG_POWERDIS */

            return;
        }
    }


    /* If it's a monitor message on a control RCA */
    if(currentClass==CONTROL_CLASS){
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.
                                     powerDistribution.
                                      pdModule[currentPowerDistributionModule].
                                       lastEnable)
        return;
    }

    /* If monitor on monitor RCA */
    /* This monitor point doesn't return an hardware status but just the current
       status that is stored in memory. The memory status is updated when the
       state of the power distribution module is changed by a control
       message. */
    if((frontend.
         cartridge[currentPowerDistributionModule].
          state==CARTRIDGE_ERROR)){
        CAN_BYTE=HARDW_ERROR;
        CAN_SIZE=CAN_BYTE_SIZE;
        return;
    }

    // Return 2 if we are in STANDBY2 mode:
    CAN_BYTE=frontend.
              powerDistribution.
               pdModule[currentPowerDistributionModule].
                enable[CURRENT_VALUE]
            +frontend.
              cartridge[currentPowerDistributionModule].
               standby2;
    CAN_SIZE=CAN_BYTE_SIZE;
}

