/*! \file   ppComm.c
    \brief  Parallel Port communication interface functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:33:14 by avaccari

    This file contains the functions necessary to communicate via the
    parallel port of the ARCOM Pegasus board.
    This interface handles the communications between the ARCOM Pegasus and the
    AMBSI1 board.
 */

/* Includes */
#include <i86.h>        /* _enable, _disable */
#include <conio.h>      /* inp, outp */
#include <dos.h>        /* _dos_setvect, _dos_getvect */
#include <stdio.h>      /* printf */

#include "error.h"
#include "timer.h"
#include "ppComm.h"
#include "pegasus.h"
#include "debug.h"
#include "globalDefinitions.h"

/* Globals */
/* Static */
static unsigned char PPPICAddr; // Pic address to use depending on IRQ number
static unsigned char PPPICMask; // Pic mask
static unsigned char LD3PrimaryInterruptNo;
static unsigned char LD3PrimaryInterruptVector;
static unsigned int  SPPDataPort, SPPStatusPort, SPPControlPort, EPPDataPort;
//! count EPP timeouts after reads and writes.  For display in console.
static unsigned int headerTimeout, payloadTimeout, writeTimeout;

/* Externs */
unsigned char PPRxBuffer[CAN_RX_MESSAGE_SIZE];
unsigned char PPTxBuffer[CAN_TX_MAX_PAYLOAD_SIZE];

/* Helper macros */

/*! Set a bit in the control port */
#define SETCONTROL(WHAT, VAL) { \
    outp(SPPControlPort, VAL ? (inp(SPPControlPort) | WHAT) : (inp(SPPControlPort) &~ WHAT)); }

/*! Get a bit from the control port */
#define GETCONTROL(WHAT) (inp(SPPControlPort) & WHAT) ? 1 : 0

/*! Set a bit in the status port */
#define SETSTATUS(WHAT, VAL) { \
    outp(SPPStatusPort, VAL ? (inp(SPPStatusPort) | WHAT) : (inp(SPPStatusPort) &~ WHAT)); }

/*! Get a bit from the status port. */
#define GETSTATUS(WHAT) (inp(SPPStatusPort) & WHAT) ? 1 : 0

/* Prototypes */
/* Statics */
static void (interrupt far *oldHandler)(void);

/*! This function will configure the parallel port to handle the communication
    with the AMBSI1.

    The parallel port is configured as follows:
        - The parallel port interrupt is enabled and set the desired value
        - The interrupt vector table is redirected to point to a new interrupt
          function that will handle the AMBSI1 request to transmit
        - The port is set up as bidirectional EPP 1.7
        - The PIC (Programmable Interrupt Controller) is programmed to accept
          interrupt on the parallel port line.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int PPOpen(void) {
    #ifdef DEBUG_STARTUP
        printf("Initializing parallel port communication...\n");
    #endif

    /* Modify the configuration in the Super I/O Controller (FDC37B72x) to
       change the parallel port mode to EPP 1.7 */
    _disable();                                 // Disable all interrupts
    outp(SIO_CONFIG_PORT,
         SIO_CONFIG_MODE_KEY);  // Enter Super I/O configuration mode
    _enable();                                  // Enable all interrupts
    outp(SIO_INDEX_PORT,
         GC_DEVICE_NO);          // Activate the logical device selection
    outp(SIO_DATA_PORT,
         LD3_DEVICE_NO);          // Select the logical device

    /* Check if interrupt is turned on */
    outp(SIO_INDEX_PORT,
         LD3_PRIMARY_INT);       // Select the primary interrupt register
    LD3PrimaryInterruptNo = inp(SIO_DATA_PORT); // Load the primary interrupt number
    if (LD3PrimaryInterruptNo==0) {
        outp(SIO_DATA_PORT, PP_DEFAULT_IRQ_NO);  // If disabled, enable and assign PP_IRQ_NO
        LD3PrimaryInterruptNo = PP_DEFAULT_IRQ_NO;
    }

    #ifdef DEBUG_STARTUP
        printf(" Current PP interrupt number: %d\n", LD3PrimaryInterruptNo); 
    #endif

    /* Configure pic mask and interrupt vector */
    if (LD3PrimaryInterruptNo<2 || LD3PrimaryInterruptNo>15) {
        criticalError(ERR_PP, ERC_IRQ_RANGE);   // Parallel Port Interrupt out of Range
        return ERROR;
    }

    if (LD3PrimaryInterruptNo>=2 && LD3PrimaryInterruptNo<=7) {
        LD3PrimaryInterruptVector = LD3PrimaryInterruptNo+0x08; // Assign the vector number corresponding to the interrupt number
        PPPICAddr = PIC_ADDR1;
        PPPICMask = 0x01<<LD3PrimaryInterruptNo;
    } else {
        LD3PrimaryInterruptVector = LD3PrimaryInterruptNo+0x68; // Assign the vector number corresponding to the interrupt number
        PPPICAddr = PIC_ADDR2;
        PPPICMask = 0x01<<(LD3PrimaryInterruptNo-8);
    }

    outp(SIO_INDEX_PORT,
         LD3_MODE_REGISTER1);                               // Select the mode register
    outp(SIO_DATA_PORT,
         (inp(LD3_MODE_REGISTER1)&0xF8)|LD3_EPP_MODE_KEY);  // Switch mode to EPP 1.7
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX2);                               // Select the low order byte of the parallel port address
    SPPDataPort=inp(SIO_DATA_PORT);                         // Write the low order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX1);                               // Select the high order byte of the parallel port address
    SPPDataPort|=inp(SIO_DATA_PORT)<<8;                     // Write the high order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ACTIVATE_INDEX);                               // Select the activation register
    outp(SIO_DATA_PORT,
         LD3_ACTIVATE_KEY);                                 // Activate the registers
    outp(SIO_CONFIG_PORT,
         SIO_RUN_MODE_KEY);                                 // Exit Super I/0 configuration mode

    /* Define other PP ports */
    SPPStatusPort = SPPDataPort + 1;  // Define SPP Status Port
    SPPControlPort = SPPDataPort + 2; // Define SPP Control Port
    EPPDataPort = SPPDataPort + 4;    // Define EPP Data Port

    /* Text below is from:
     * Peacock, C. (2000). Interfacing the parallel port.
     * Retrieved December 17, 2021, from http://wearcam.org/seatsale/programs/www.beyondlogic.org/epp/epp.htm
     *
     * Before you can start any EPP cycles by reading and writing to the EPP Data and Address Ports,
     * the port must be configured correctly. In the idle state, an EPP port should have it's
     * nAddress Strobe, nData Strobe, nWrite and nReset lines inactive, high.
     * Some ports require you to set this up before starting any EPP Cycle.
     * Therefore our first task is to manually initialise these lines using the SPP Registers.
     * Writing XXXX0100 to the control port will do this.  */
    SETCONTROL(SPPC_NSELECT, 0);    // hardware inverted output
    SETCONTROL(SPPC_INIT, 1);
    SETCONTROL(EPPC_NDSTROBE, 0);   // hardware inverted output
    SETCONTROL(EPPC_NWRITE, 0);

    /* Reset the timeout bit, which seems to always be set after a warm boot */
    PPClearTimeout();

    /* Set parallel port direction to input */
    SETCONTROL(SPPC_DATADIR, 1);

    /* Set the interrupt vector of the parallel port to point to: PPIntHandler */
    PicPPIrqCtrl(DISABLE);
    // Disable interrupt on the parallel port:
    SETCONTROL(SPPC_IRQENA, DISABLE);
    // Read the current interrupt vector corresponding to PP_IRQ_NO:
    oldHandler = _dos_getvect(LD3PrimaryInterruptVector);
    // Replace the vector with one pointing to: PPIntHandler:
    _dos_setvect(LD3PrimaryInterruptVector, PPIntHandler);

    /* Clear any current interrupt on the parallel port */
    PPClear();

    #ifdef DEBUG_STARTUP
        printf("done!\n\n"); // Initialization
    #endif

    return NO_ERROR;
}


int PPClearTimeout() {
    /* Text below is from:
     * Axelson, J. (2001). Parallel Port Complete Programming, interfacing & using the PC's parallel printer port. Lakeview Research.
     *
     * IEEE 1284 doesn't specify it, but Status bit 0 usually is a timeout bit that indicates
     * a failed EPP transfer. Unfortunately, the method for clearing the timeout bit varies */
    int timeout;
    timeout = GETSTATUS(EPPS_TIMEOUT);
    if (timeout) {
        // determined experimentally for ARCOM - set a 1 to the timeout bit to clear it:
        SETSTATUS(EPPS_TIMEOUT, 1);
    }
    return timeout;
}

/*! This function will finalize the connection with the AMBSI1 enabling
    effective communication */
int PPStart(void) {

    /* Enable parallel port interrupt line */
    SETCONTROL(SPPC_IRQENA, ENABLE)

    /* Modify the PIC to acknowledge interrupt on the parallel port line */
    PicPPIrqCtrl(ENABLE); // Enable parallel port interrupt line in the PIC

    /* We set INIT low when ready to handle parallel port transactions */
    SETCONTROL(SPPC_INIT, 0);

    return NO_ERROR;
}

/*! This function will enable/disable the PIC parallel port interrupt. */
void PicPPIrqCtrl(unsigned char enable) {
    outp(PPPICAddr + 1, (enable ? (inp(PPPICAddr + 1) &~ PPPICMask) : (inp(PPPICAddr + 1) | PPPICMask)));
}

/*! This function will close the parallel port, restore interrupt vector table
    and turn off parallel port interrupts.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int PPClose(void) {

    #ifdef DEBUG_STARTUP
        printf("Shutting down parallel port communication...\n");
    #endif

    // Signal ARCOM not ready
    SETCONTROL(SPPC_INIT, 1);

    // Disable interrupt on the parallel port:
    SETCONTROL(SPPC_IRQENA, DISABLE)

    /* Clear the last interrupt request */
    PPClear();

    /* Set parallel port direction to output */
    SETCONTROL(SPPC_DATADIR, 0);

    /* Modify the PIC to ignore interrupt on the parallel port line */
    PicPPIrqCtrl(DISABLE); // Disable parallel port interrupt line in the PIC

    /* Restore original interrupt vector table */
    _dos_setvect(LD3PrimaryInterruptVector, oldHandler);

    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}

/*! This function will transmit \p length bytes of data on the parallel port.
    \param  length  an unsigned char */
void PPWrite(unsigned char length) {
    unsigned char i;

    // Set direction to output:
    SETCONTROL(SPPC_DATADIR, 0)

    // Write data length:
    outp(EPPDataPort, length);

    // Write the data bytes:
    for (i = 0; i < length; i++) {
        outp(EPPDataPort, PPTxBuffer[i]);
    }

    // Set direction to input for next message:
    SETCONTROL(SPPC_DATADIR, 1)

    // Detect and clear EPP timeout:
    if (PPClearTimeout())
        writeTimeout++;
}

/* Interrupt function receives CAN_MESSAGE_SIZE bytes from the parallel port and stores them in message */
static void interrupt far PPIntHandler(void) {

    unsigned char i, payloadSize;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Received!\n");
    #endif /* DEBUG_PPCOM */

    /* Input the CAN header from the parallel port */
    #ifdef DEBUG_PPCOM
        printf("  CAN Header:\n");
    #endif /* DEBUG_PPCOM */
    for (i = 0; i < CAN_RX_HEADER_SIZE; i++) {
        PPRxBuffer[i] = inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        for (i = 0; i < CAN_RX_HEADER_SIZE; i++) {
            printf("    PPRxBuffer[%d]=0x%X\n", i, PPRxBuffer[i]);
        }
    #endif /* DEBUG_PPCOM */

    // Detect and clear EPP timeout:
    if (PPClearTimeout()) {
        headerTimeout++;
        return;
    }

    payloadSize = PPRxBuffer[CAN_RX_HEADER_SIZE - 1];

    /* If the payload size is 0 then it is a monitor message */
    if (payloadSize == CAN_MONITOR) {
        #ifdef DEBUG_PPCOM
            printf("  Monitor message\n");
        #endif /* DEBUG_PPCOM */
        newCANMsg = 1; // Notify program of new message

        #ifdef DEBUG_PPCOM
            printf("Interrupt Serviced!\n");
        #endif /* DEBUG_PPCOM */

        return;
    }

    if (payloadSize > CAN_MESSAGE_PAYLOAD_SIZE) {
        payloadSize = CAN_MESSAGE_PAYLOAD_SIZE;
    }

    /* If it's a control message, load the payload */
    for (i = 0; i < payloadSize; i++) {
        PPRxBuffer[CAN_RX_HEADER_SIZE + i] = inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        printf("  Control message\n");
        printf("    Payload:\n");
        for (i = 0; i < payloadSize; i++) {
            printf("      PPRxBuffer[%d]=0x%X\n", CAN_RX_HEADER_SIZE + i, PPRxBuffer[CAN_RX_HEADER_SIZE + i]);
        }
    #endif /* DEBUG_PPCOM */

    // Detect and clear EPP timeout:
    if (PPClearTimeout())
        payloadTimeout++;

    /* Notify program of new message */
    newCANMsg = 1;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Serviced!\n");
    #endif /* DEBUG_PPCOM */
}

/*! This function clears the IRQ of the parallel port. This will allow the port
    to receive another message. Until the IRQ is cleared, all the other incoming
    messages will be ignored. */
void PPClear(void) {
    outp(PPPICAddr, PIC_INT_CLR);
}

/*! Print a port status report to the console */
void PPStatusReport(void) {
    unsigned char statusPort, controlPort, i;

    // Each message byte will be printed like '00 '
    // This buffer is safe because CAN_RX_MESSAGE_SIZE is larger than CAN_MAX_PAYLOAD_SIZE:
    char buf[3 * CAN_RX_MESSAGE_SIZE + 1];

    statusPort = inp(SPPStatusPort);
    controlPort = inp(SPPControlPort);

    printf(" STATUS PORT    CONTROL PORT\n");
    printf("     nIRQ: %d       nWrite: %d\n", GETSTATUS(SPPS_NIRQIN), GETCONTROL(EPPC_NWRITE));
    printf("   nError: %d     nDStrobe: %d\n", GETSTATUS(SPPS_NERRORIN), GETCONTROL(EPPC_NDSTROBE));
    printf("Select-in: %d      nSelect: %d\n", GETSTATUS(SPPS_SELECTIN), GETCONTROL(SPPC_NSELECT));
    printf("Paper-out: %d         Init: %d\n", GETSTATUS(SPPS_PAPEROUT), GETCONTROL(SPPC_INIT));
    printf("Interrupt: %d      IRQ-ena: %d\n", GETSTATUS(EPPS_INTERRUPT), GETCONTROL(SPPC_IRQENA));
    printf("    nWait: %d     Data-dir: %d\n", GETSTATUS(EPPS_NWAIT), GETCONTROL(SPPC_DATADIR));
    printf(" Timeouts: header: %u  payload: %u  write: %u\n\n", headerTimeout, payloadTimeout, writeTimeout);

    for (i = 0; i < CAN_RX_MESSAGE_SIZE; i++) {
        sprintf(buf + (3 * i), "%02X ", PPRxBuffer[i]);
    }
    printf("RxBuffer: %s\n", buf);

    for (i = 0; i < CAN_TX_MAX_PAYLOAD_SIZE; i++) {
        sprintf(buf + (3 * i), "%02X ", PPTxBuffer[i]);
    }
    printf("TxBuffer: %s\n", buf);
}
