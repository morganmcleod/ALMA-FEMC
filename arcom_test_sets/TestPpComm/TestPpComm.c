// Version string global
char version[16]="0.2";

// Includes
#include <i86.h>        /* _enable, _disable */
#include <conio.h>      /* inp, outp */
#include <dos.h>        /* _dos_setvect, _dos_getvect */
#include <stdio.h>      /* printf */
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>
#include "pegasus.h"
#include "globalDefinitions.h"
#include "timer.h"

/* Define */
#define DEBUG_STARTUP
#define DEBUG_PPCOM

/* Parallel port control register */
#define EPPC_NWRITE             0x01    //!< nWrite inverted output: 0 = write, 1 = read
#define EPPC_NDSTROBE           0x02    //!< nDataStrobe inverted output
#define SPPC_INIT               0x04    //<! Initialize
#define SPPC_NSELECT            0x08    //<! nSelect inverted output
#define SPPC_IRQENA             0x10    //<! Enable IRQ Via Ack Line
#define SPPC_DATADIR            0x20    //<! Data direction: 0 = output, 1 = input
/* Parallel port status register */
#define SPPS_NIRQIN             0x04    //<! nIRQ in
#define SPPS_ERRORIN            0x08    //<! nError in
#define SPPS_SELECTIN           0x10    //<! Select in
#define SPPS_PAPEROUT           0x20    //<! Paper-Out
#define EPPS_INTERRUPT          0x40    //<! Interrupt in
#define EPPS_NWAIT              0x80    //<! nWait inverted input
/* Constants */
#define PP_DEFAULT_IRQ_NO       0x07    //!< IRQ to be assigned to parallel port
#define WAIT_ON_AMBSI_NOT_READY 100     // Number of milliseconds to wait before checking AMBSI status at initialization

/* Globals */
volatile unsigned char newCANMsg=0; /*!< This variable is a semaphore which will
                                         notify the entire program of the arrival
                                         of a new CAN message. It is cleared
                                         once the message has been dealt with. */
/* Static */
static unsigned char PPPICAddr; // Pic address to use depending on IRQ number
static unsigned char PPPICMask; // Pic mask
static unsigned char LD3PrimaryInterruptNo;
static unsigned char LD3PrimaryInterruptVector;
static unsigned int  SPPDataPort, SPPStatusPort, SPPControlPort, EPPDataPort;
/* Externs */
unsigned char PPRxBuffer[CAN_RX_MESSAGE_SIZE];
unsigned char PPTxBuffer[CAN_TX_MAX_PAYLOAD_SIZE];
/* Prototypes */
static void interrupt far PPIntHandler(void); // Interrupt function to handle incoming messages
void PicPPIrqCtrl(unsigned char enable);
void PPClear();
/* Statics */
static void (interrupt far *oldHandler)(void);

/* At ARCOM power-up:
 * All lines bounce high then low within 5 ms
 * after 130ms all lines go high
 * after 1s INIT goes low, NSELECT and SELECTIN pulse low
 * after 8s INIT goes high, NSELECT goes low, all others pulse low
 * after 500 ms, INIT pulses low
 *
 * At AMBSI power-up:
 * All lines low except SELECTIN high
 */

/* Helpers and macros */
void setControl(unsigned char what, unsigned char val) {
    // Set or clear a bit on the control port
    if (val)
        outp(SPPControlPort, inp(SPPControlPort) | what);
    else
        outp(SPPControlPort, inp(SPPControlPort) &~ what);
}

// Toggle a bit on the control port
#define toggleControl(WHAT) { outp(SPPControlPort, inp(SPPControlPort) ^ WHAT); }

// Get a bit from the control port
#define getControl(WHAT) (inp(SPPControlPort) & WHAT) ? 1 : 0

// Get a bit from the status port.
#define getStatus(WHAT) (inp(SPPStatusPort) & WHAT) ? 1 : 0




/*! This function will configure the parallel port to handle the communication
    with the AMBSI1.

    The parallel port is configured as follows:
        - The parallel port interrupt is enable and set the desired value
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
    _disable();                 // Disable all interrupts
    outp(SIO_CONFIG_PORT,
         SIO_CONFIG_MODE_KEY);  // Enter Super I/O configuration mode
    _enable();                  // Enable all interrupts
    outp(SIO_INDEX_PORT,
         GC_DEVICE_NO);         // Activate the logical device selection
    outp(SIO_DATA_PORT,
         LD3_DEVICE_NO);        // Select the logical device

    /* Check if interrupt is turned on */
    outp(SIO_INDEX_PORT,
         LD3_PRIMARY_INT);      // Select the primary interrupt register
    LD3PrimaryInterruptNo = inp(SIO_DATA_PORT); // Load the primary interrupt number
    if (LD3PrimaryInterruptNo == 0) {
        outp(SIO_DATA_PORT, PP_DEFAULT_IRQ_NO); // If disabled, enable and assign PP_IRQ_NO
        LD3PrimaryInterruptNo = PP_DEFAULT_IRQ_NO;
    }

    #ifdef DEBUG_STARTUP
        printf(" Current PP interrupt number: %d\n", LD3PrimaryInterruptNo);
    #endif

    /* Configure pic mask and interrupt vector */
    if (LD3PrimaryInterruptNo < 2 || LD3PrimaryInterruptNo > 15) {
        printf(" ERROR: Parallel Port Interrupt out of Range.\n");
        return ERROR;
    }

    if (LD3PrimaryInterruptNo >= 2 && LD3PrimaryInterruptNo <= 7) {
        // Assign the vector number corresponding to the interrupt number
        LD3PrimaryInterruptVector = LD3PrimaryInterruptNo + 0x08;
        PPPICAddr = PIC_ADDR1;
        PPPICMask = 0x01 << LD3PrimaryInterruptNo;
    } else {
        // Assign the vector number corresponding to the interrupt number
        LD3PrimaryInterruptVector = LD3PrimaryInterruptNo + 0x68;
        PPPICAddr = PIC_ADDR2;
        PPPICMask = 0x01 << (LD3PrimaryInterruptNo - 8);
    }

    outp(SIO_INDEX_PORT,
         LD3_MODE_REGISTER1);                               // Select the mode register
    outp(SIO_DATA_PORT,
         (inp(LD3_MODE_REGISTER1) & 0xF8) | LD3_EPP_MODE_KEY);  // Switch mode to EPP 1.7
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX2);                               // Select the low order byte of the parallel port address
    SPPDataPort=inp(SIO_DATA_PORT);                         // Write the low order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX1);                               // Select the high order byte of the parallel port address
    SPPDataPort |= inp(SIO_DATA_PORT) << 8;                 // Write the high order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ACTIVATE_INDEX);                               // Select the activation register
    outp(SIO_DATA_PORT,
         LD3_ACTIVATE_KEY);                                 // Activate the registers
    outp(SIO_CONFIG_PORT,
         SIO_RUN_MODE_KEY);                                 // Exit Super I/0 configuration mode

    /* Define other PP ports */
    SPPStatusPort = SPPDataPort + 1;  // SPP Status Port
    SPPControlPort = SPPDataPort + 2; // SPP Control Port
    EPPDataPort = SPPDataPort + 4;    // EPP Data Port

    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif
    return NO_ERROR;
}

int PPSetupLink() {
// The original implementation of establishing the AMBSI-ARCOM link was setting the status port
//  select line SPPS_SELECTIN on the control port (which is actually SPPC_IRQENA).  Somehow it worked.

    int timedOut=0;

    #ifdef DEBUG_STARTUP
        printf("\nEstablishing link to AMBSI...\n");
    #endif

    // Signal ARCOM ready: INIT line low and SELECT line high:
    setControl(SPPC_INIT, 0);
    setControl(SPPC_NSELECT, 0); // hardware inverted output

    /* Setup async timer to wait for AMBSI reply. If no reply switch notify and
       continue. This allows console control and debug. */
    if (startAsyncTimer(TIMER_PP_AMBSI_RDY, TIMER_PP_TO_AMBSI_RDY, FALSE) == ERROR) {
        printf(" ERROR: starting Async Timer. CAN interface disabled.\n");
        return ERROR;
    }

    /* Wait for AMBSI1 to reply or timer to expire */
    timedOut = 0;
    while (timedOut == 0 && getStatus(SPPS_SELECTIN)) {   // Wait for select in low
        waitMilliseconds(WAIT_ON_AMBSI_NOT_READY);    // Wait before checking again the ready status
        timedOut = queryAsyncTimer(TIMER_PP_AMBSI_RDY);
    }

    /* If the timer has expired signal the error and continue */
    if (timedOut == TIMER_EXPIRED) {
        printf(" ERROR: AMBSI1 not responding. CAN interface disabled.\n");
    }

    /* In case of no error, clear the asynchronous timer. */
    if (stopAsyncTimer(TIMER_PP_AMBSI_RDY) == ERROR) {
        return ERROR;
    }

    /* Set parallel port direction to input */
    setControl(SPPC_DATADIR, PP_INPUT);

    /* Set the interrupt vector of the parallel port to point to: PPIntHandler */
    PicPPIrqCtrl(DISABLE);

    // Disable interrupt on the parallel port:
    setControl(SPPC_IRQENA, DISABLE);

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


/*! This function will finalize the connection with the AMBSI1 enabling
    effective communication */
int PPStart(void) {

    /* Enable parallel port interrupt line */
    setControl(SPPC_IRQENA, ENABLE);

    /* Modify the PIC to acknowledge interrupt on the parallel port line */
    PicPPIrqCtrl(ENABLE); // Enable parallel port interrupt line in the PIC

    return NO_ERROR;
}

int PPStop(void) {

    /* Modify the PIC to ignore interrupt on the parallel port line */
    PicPPIrqCtrl(DISABLE); // Enable parallel port interrupt line in the PIC

    /* Disable parallel port interrupt line */
    setControl(SPPC_IRQENA, DISABLE);

    return NO_ERROR;
}

/*! This function will enable/disable the PIC parallel port interrupt. */
void PicPPIrqCtrl(unsigned char enable) {
    if (enable) {
        outp(PPPICAddr + 1,
             inp(PPPICAddr + 1) & (~PPPICMask));
    } else {
        outp(PPPICAddr + 1,
             inp(PPPICAddr + 1) | (PPPICMask));
    }
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

    // Disable interrupt on the parallel port.
    setControl(SPPC_IRQENA, DISABLE);

    /* Clear the last interrupt request */
    PPClear();

    /* Set parallel port direction to output */
    setControl(SPPC_DATADIR, PP_OUTPUT);

    // Disable parallel port interrupt line in the PIC
    PicPPIrqCtrl(DISABLE);

    /* Restore original interrupt vector table */
    _dos_setvect(LD3PrimaryInterruptVector, oldHandler);

    // Signal ARCOM not ready:
    setControl(SPPC_INIT, 1);

    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif
    return NO_ERROR;
}

/*! This function will transmit \p length bytes of data on the parallel port.
    \param  length  an unsigned char */
void PPWrite(unsigned char length) {
    unsigned char cnt;

    setControl(SPPC_DATADIR, PP_OUTPUT);
    outp(EPPDataPort, length);

    for(cnt = 0; cnt < length; cnt++) {
        outp(EPPDataPort, PPTxBuffer[cnt]);
    }
    setControl(SPPC_DATADIR, PP_INPUT);
}

/*! This function clears the IRQ of the parallel port. This will allow the port
    to receive another message. Until the IRQ is cleared, all the other incoming
    messages will be ignored.
    An alternative would be create a queue to store incoming messages. This
    could create problem with the 150us timing requirement on monitor messages. */
void PPClear() {
    outp(PPPICAddr, PIC_INT_CLR);
}

/* Interrupt function receives CAN_MESSAGE_SIZE bytes from the parallel port and stores them in message */
static void interrupt far PPIntHandler(void) {

    unsigned char cnt,cnt2,dump;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Received!\n");
    #endif /* DEBUG_PPCOM */

    /* Input the CAN header from the parallel port */
    #ifdef DEBUG_PPCOM
        printf("  CAN Header:\n");
    #endif /* DEBUG_PPCOM */
    for(cnt = 0; cnt < CAN_RX_HEADER_SIZE; cnt++) {
        PPRxBuffer[cnt] = inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        for(cnt = 0; cnt < CAN_RX_HEADER_SIZE; cnt++) {
            printf("    PPRxBuffer[%d]=0x%X\n", cnt, PPRxBuffer[cnt]);
        }
    #endif /* DEBUG_PPCOM */

    cnt2 = PPRxBuffer[CAN_RX_HEADER_SIZE - 1];

    /* If the payload = 0 then is a monitor message */
    if (cnt2 == CAN_MONITOR) {
        #ifdef DEBUG_PPCOM
            printf("  Monitor message\n");
        #endif /* DEBUG_PPCOM */
        newCANMsg = 1; // Notify program of new message

        #ifdef DEBUG_PPCOM
            printf("Interrupt Serviced!\n");
        #endif /* DEBUG_PPCOM */
        return;
    }

    /* Check the payload size against the limit: if it's to big, clear the
       interrupt and exit. We read the data anyway to prevent the AMBSI from
       getting stuck! */
    if (cnt2 > CAN_RX_MAX_PAYLOAD_SIZE) {
        #ifdef DEBUG_PPCOM
            printf("  Too much data! dumping...");
        #endif /* DEBUG_PPCOM */
        for(cnt = 0; cnt < cnt2; cnt++) {
            dump = inp(EPPDataPort);
        }
        PPClear();
        return;
    }

    /* If it's a control message, load the payload */
    #ifdef DEBUG_PPCOM
        printf("  Control message\n");
        printf("    Payload:\n");
    #endif /* DEBUG_PPCOM */
    for(cnt = 0; cnt < cnt2; cnt++) {
        PPRxBuffer[CAN_RX_HEADER_SIZE + cnt] = inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        for(cnt = 0; cnt < cnt2; cnt++) {
            printf("      PPRxBuffer[%d]=0x%X\n",
                   CAN_RX_HEADER_SIZE + cnt,
                   PPRxBuffer[CAN_RX_HEADER_SIZE + cnt]);
        }
    #endif /* DEBUG_PPCOM */

    /* Notify program of new message */
    newCANMsg = 1;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Serviced!\n");
    #endif /* DEBUG_PPCOM */
}

// Position the cursor to the desired location (x -> row, y -> column)
void cursorAt(unsigned char x, unsigned char y) {

    union REGS r;

    // Check for limits
    if ((x > 24) || (y > 79)) {
        return;
    }

    r.h.ah = 2;
    r.h.bh = 0;
    r.h.dl = y;
    r.h.dh = x;

    int86(0x10, &r, &r);
}

// Printf at a particular location
void printfAt(unsigned char x, unsigned char y, char * format, ... ) {

    va_list arglist;

    cursorAt(x, y);

    va_start(arglist, format);
    vprintf(format, arglist);
    va_end(arglist);

    flushall(); // Make sure everything is printed
}

// Wait for a key to be pressed
int waitKey(char key) {

	int choice;

	fflush(stdin); // Make sure the input buffer is empty

	switch(key) {
		case ' ':
			printfAt(24, 54, "Press ENTER to continue...");
			while (!getche());
			return 0;
			break;
		default:
			choice = getche();
			break;
	}

	return choice;
}

// Writes error on the screen
void showMessage(char *message) {
    if (message) {
        printfAt(24 ,0 ,message);
        //waitKey(' ');
    } else {
        printfAt(24, 0, "No errors");
    }
}

// Clear screen
void clearScreen(void) {

    union REGS r;

    r.h.ah = 10;
    r.h.al = 32;
    r.h.bh = 0;
    r.x.cx = 2000;

    int86(0x10, &r, &r);
}

// Display current state
void showCurrentState(void) {
    unsigned char statusPort, controlPort, cnt;
    char buf[3 * CAN_RX_MESSAGE_SIZE + 1];

    statusPort = inp(SPPStatusPort);
    controlPort = inp(SPPControlPort);

    printfAt(0, 0, "STATUS PORT:");
    printfAt(0, 14, "nIRQ");
    printfAt(0, 24, "nError");
    printfAt(0, 34, "Sel-in");
    printfAt(0, 44, "Paper-out");
    printfAt(0, 54, "Interrupt");
    printfAt(0, 64, "nWait");

    printfAt(1, 14, "%d", getStatus(SPPS_NIRQIN));
    printfAt(1, 24, "%d", getStatus(SPPS_ERRORIN));
    printfAt(1, 34, "%d", getStatus(SPPS_SELECTIN));
    printfAt(1, 44, "%d", getStatus(SPPS_PAPEROUT));
    printfAt(1, 54, "%d", getStatus(EPPS_INTERRUPT));
    printfAt(1, 64, "%d", getStatus(EPPS_NWAIT));

    printfAt(3, 0, "CONTROL PORT:");
    printfAt(3, 14, "nWrite");
    printfAt(3, 24, "nDStrobe");
    printfAt(3, 34, "Init");
    printfAt(3, 44, "nSelect");
    printfAt(3, 54, "IRQ-ena");
    printfAt(3, 64, "Data-dir");

    printfAt(4, 14, "%d", getControl(EPPC_NWRITE));
    printfAt(4, 24, "%d", getControl(EPPC_NDSTROBE));
    printfAt(4, 34, "%d", getControl(SPPC_INIT));
    printfAt(4, 44, "%d", getControl(SPPC_NSELECT));
    printfAt(4, 54, "%d", getControl(SPPC_IRQENA));
    printfAt(4, 64, "%d", getControl(SPPC_DATADIR));

    for (cnt = 0; cnt < CAN_RX_MESSAGE_SIZE; cnt++) {
        sprintf(buf + (3 * cnt), "%02X ", PPRxBuffer[cnt]);
    }
    printfAt(6, 0, "RxBuffer: %s", buf);

    for (cnt = 0; cnt < CAN_TX_MAX_PAYLOAD_SIZE; cnt++) {
        sprintf(buf + (3 * cnt), "%02X ", PPTxBuffer[cnt]);
    }
    printfAt(7, 0, "TxBuffer: %s", buf);

    printfAt(8, 0, "--------------------------------------------------------------------------------");
}

// menu and prompt for keypress
int displayMenu(int menuNo) {

    int menuChoice;
    int startRow = 9;

    printfAt(startRow, 5, "--- PPComm tester: %s ---", version);

    if (menuNo == 1) {
        printfAt(startRow + 1, 5, "q - Exit");
    } else {
        printfAt(startRow + 1, 5, "q - Back");
    }

    switch(menuNo) {
        case 0:
            break;
        case 1: // Main menu
            printfAt(startRow + 2, 5, "1 - PPSetupLink(): Establish the AMBSI-ARCOM link.");
            printfAt(startRow + 3, 5, "2 - PPStart() / PPStop(): Toggle the interrupt handler.");
            printfAt(startRow + 4, 5, "3 - PPClear(): Clear the current interrupt.");
            printfAt(startRow + 5, 5, "4 - Toggle nWrite (0=output, 1=input)");
            printfAt(startRow + 6, 5, "5 - Toggle nDStrobe");
            printfAt(startRow + 7, 5, "6 - Toggle Init");
            printfAt(startRow + 8, 5, "7 - Toggle nSelect");
            printfAt(startRow + 9, 5, "8 - Toggle IRQ-ena");
            printfAt(startRow + 10, 5, "9 - Toggle Data-dir (0=write, 1=read)");
            break;
        case 2:
        default:
            break;
    }
    cursorAt(24,79);
	menuChoice=waitKey(0);
	return menuChoice;
}

int main(int argc, char *argv[]) {

    int menuChoice = 0;
    char message[81];
    sprintf(message, "");

    PPOpen();

    while (menuChoice != 'q') {
        clearScreen();
        showCurrentState();
        showMessage(message);
        sprintf(message, "");

        switch(menuChoice=displayMenu(1)) {
            case 'q':
                break;
            case '1':
                PPSetupLink();
                sprintf(message, "PPSetupLink() done");
                break;
            case '2':
                if (getControl(SPPC_IRQENA)) {
                    PPStop();
                    sprintf(message, "PPStop() done");
                } else {
                    PPStart();
                    sprintf(message, "PPStart() done");
                }
                break;
            case '3':
                PPClear();
                sprintf(message, "PPClear() done");
                break;
            case '4':
                toggleControl(EPPC_NWRITE);
                break;
            case '5':
                toggleControl(EPPC_NDSTROBE);
                break;
            case '6':
                toggleControl(SPPC_INIT);
                break;
            case '7':
                toggleControl(SPPC_NSELECT);
                break;
            case '8':
                toggleControl(SPPC_IRQENA);
                break;
            case '9':
                toggleControl(SPPC_DATADIR);
                break;
            default:
                break;
        }
    }
    PPClose();
    return NO_ERROR;
}



