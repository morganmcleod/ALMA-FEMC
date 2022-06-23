/*! \file  teledynePa.c
    \brief Handles the requests to configure the band 7 Teledyne PA chip
*/

#include "teledynePa.h"
#include "error.h"
#include "frontend.h"
#include "debug.h"

unsigned char currentTeledynePaModule;

static HANDLER teledynePaModulesHandler[TELEDYNE_PA_MODULES_NUMBER] = {
        hasTeledynePaHandler,
        collectorByteHandler,
        collectorByteHandler
};

void teledynePaHandler() {

    // Check if the submodule is in range
    currentTeledynePaModule = (CAN_ADDRESS & TELEDYNE_PA_MODULES_RCA_MASK);
    if (currentTeledynePaModule >= TELEDYNE_PA_MODULES_NUMBER) {
        storeError(ERR_TELEDYNE_PA, ERC_MODULE_RANGE);
        CAN_STATUS = HARDW_RNG_ERR;
        return;
    }

    // Call the correct handler
    (teledynePaModulesHandler[currentTeledynePaModule])();
}

static void hasTeledynePaHandler(void) {
    if (CAN_SIZE) { // If control (size !=0)
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].lo.pa.lastHasTeledynePa)

        // Check for other than band 7:
        if (currentModule != 6) {
            /* Store the HARDW_BLKD_ERR state in the last control message variable */
            storeError(ERR_PA_CHANNEL, ERC_COMMAND_VAL);
            frontend.cartridge[currentModule].lo.pa.lastHasTeledynePa.status = HARDW_BLKD_ERR;
            return;
        }

        // Save the setting:
        frontend.cartridge[currentModule].lo.pa.hasTeledynePa = CAN_BYTE ? TRUE : FALSE;
        return;
    }

    /* If monitor on control RCA */
    if (currentClass == CONTROL_CLASS) {
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].lo.pa.lastHasTeledynePa)
        return;
    }

    /* If monitor on a monitor RCA */
    CAN_BYTE=frontend.cartridge[currentModule].lo.pa.hasTeledynePa;
    CAN_SIZE=CAN_BOOLEAN_SIZE;
}

static void collectorByteHandler() {
    int pol = currentTeledynePaModule - 1;

    /* If control (size !=0) */
    if (CAN_SIZE) {
        // save the incoming message:
        SAVE_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].lo.pa.lastTeledyneCollectorByte[pol])

        // Check for other than band 7:
        if (currentModule != 6) {
            /* Store the HARDW_BLKD_ERR state in the last control message variable */
            storeError(ERR_PA_CHANNEL, ERC_COMMAND_VAL);
            frontend.cartridge[currentModule].lo.pa.lastHasTeledynePa.status = HARDW_BLKD_ERR;
            return;
        }

        // Save the setting:
        frontend.cartridge[currentModule].lo.pa.teledyneCollectorByte[pol] = CAN_BYTE;
        return;
    }

    /* If monitor on control RCA */
    if (currentClass == CONTROL_CLASS) {
        // return the last control message and status
        RETURN_LAST_CONTROL_MESSAGE(frontend.cartridge[currentModule].lo.pa.lastTeledyneCollectorByte[pol])
        return;
    }

    CAN_BYTE = frontend.cartridge[currentModule].lo.pa.teledyneCollectorByte[pol];
    CAN_SIZE = CAN_BYTE_SIZE;
}
