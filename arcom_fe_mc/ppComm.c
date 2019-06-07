/*! \file   ppComm.c
    \brief  Parallel Port communication interface functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:33:14 by avaccari

    This file contains the functions necessary to cummunicate via the
    parallel port of the ARCOM Pegasus board.
    This interface handles the communications between the ARCOM Pegasus and the
    AMBSI1 board. The communication is always triggered by a request from the
    ARCOM Pegasus board. After the request has been aknowledge, the AMBSI1 board
    will transmit the CAN message. */

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
/* Externs */
unsigned char PPRxBuffer[CAN_RX_MESSAGE_SIZE];
unsigned char PPTxBuffer[CAN_TX_MAX_PAYLOAD_SIZE];

/* Prototypes */
/* Statics */
static void (interrupt far * oldHandler)(void);

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
int PPOpen(void){

    int timedOut=0; // A local to keep track of time out errors

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
    if(LD3PrimaryInterruptNo==0) {
        outp(SIO_DATA_PORT, PP_DEFAULT_IRQ_NO);  // If disabled, enable and assign PP_IRQ_NO
        LD3PrimaryInterruptNo = PP_DEFAULT_IRQ_NO;
        // storeError(ERR_PP, ERC_IRQ_DISABLED);   // Parallel Port Interrupt Disabled (Warning)
    }

    #ifdef DEBUG_STARTUP
        printf(" Current PP interrupt number: %d\n", LD3PrimaryInterruptNo); 
    #endif

    /* Configure pic mask and interrupt vector */
    if(LD3PrimaryInterruptNo<2 || LD3PrimaryInterruptNo>15){
        criticalError(ERR_PP, ERC_IRQ_RANGE);   // Parallel Port Interrupt out of Range
        return ERROR;
    }

    if(LD3PrimaryInterruptNo>=2 && LD3PrimaryInterruptNo<=7){
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
    SPPStatusPort = SPPDataPort+1;  // Define SPP Status Port
    SPPControlPort = SPPDataPort+2; // Define SPP Control Port
    EPPDataPort = SPPDataPort+4;    // Define EPP Data Port

    /* Handshake readiness status with AMBSI */
    outp(SPPControlPort,
         (inp(SPPControlPort)|SPP_CONTROL_INIT)&~SPP_STATUS_SELECT); // INIT line high and SELECT line low

    /* Setup async timer to wait for AMBSI reply. If no reply switch notify and
       continue. This allows console control and debug. */
    if(startAsyncTimer(TIMER_PP_AMBSI_RDY,
                       TIMER_PP_TO_AMBSI_RDY,
                       FALSE)==ERROR){
        timedOut=0; // If error setting up the timer, unlimited wait for AMBSI1 to get ready
    }

    /* Wait for AMBSI1 to reply or timer to expire */
    while((inp(SPPStatusPort)&SPP_STATUS_SELECT)&&!timedOut){
        storeError(ERR_PP, ERC_AMBSI_WAIT); // Warning! Waiting for AMBSI board.
        waitMilliseconds(WAIT_ON_AMBSI_NOT_READY);    // Wait before checking again the ready status
        timedOut=queryAsyncTimer(TIMER_PP_AMBSI_RDY);
        if(timedOut==ERROR){
            timedOut=0; // If error while waiting for the time, ignore the timer and keep waiting
        }
    }

    /* If the timer has expired signal the error and continue */
    if(timedOut==TIMER_EXPIRED){
        printf(" ERROR: AMBSI1 not responding. CAN interface disabled.\n");
        storeError(ERR_PP, ERC_AMBSI_EXPIRED);  // Error: Timeout while waiting for the AMBSI1
    }

    /* In case of no error, clear the asynchronous timer. */
    if(stopAsyncTimer(TIMER_PP_AMBSI_RDY)==ERROR){
        return ERROR;
    }

    /* Set parallel port direction to input */
    outp(SPPControlPort,
         inp(SPPControlPort) | PP_DATA_DIR);

    /* Set the interrupt vector of the parallel port to point to: PPIntHandler */
    PicPPIrqCtrl(DISABLE);
    PPIrqCtrl(DISABLE); // Disable interrupt on the parallel port. This should really disable all the interrupts but I still have to find a way to do it
    oldHandler = _dos_getvect(LD3PrimaryInterruptVector);   // Read the current interrupt vector corresponding to PP_IRQ_NO
    _dos_setvect(LD3PrimaryInterruptVector,
                 PPIntHandler);   // Replace the vector with one pointing to: PPIntHandler

    /* Clear any curren interrupt on the parallel port */
    PPClear();

// If you manage to disable all the interrupts, they should be re-enabled here!

    #ifdef DEBUG_STARTUP
        printf("done!\n\n"); // Initialization
    #endif

    return NO_ERROR;
}


/*! This function will finialize the connection with the AMBSI1 enabling
    effective communication */
int PPStart(void){

    /* Lower the INIT line and write 0x5A to notify AMBSI that ARCOM is ready
       to communicate */
/*    outp(SPPControlPort,
         inp(SPPControlPort)&~SPP_CONTROL_INIT);
*/
    /* Enable parallel port interrupt line */
    PPIrqCtrl(ENABLE);

    /* Modify the PIC to acknowledge interrupt on the parallel port line */
    PicPPIrqCtrl(ENABLE); // Enable parallel port interrupt line in the PIC

    return NO_ERROR;
}


/*! This function will enable/disable the parallel port interrupt. */
void PPIrqCtrl(unsigned char enable){
    if(enable){
        outp(SPPControlPort,
             inp(SPPControlPort) | PP_IRQ_ENA);
    } else {
        outp(SPPControlPort,
             inp(SPPControlPort) & ~PP_IRQ_ENA);
    }
}

/*! This function will enable/disable the PIC parallel port interrupt. */
void PicPPIrqCtrl(unsigned char enable){
    if(enable){
        outp(PPPICAddr+1,
             inp(PPPICAddr+1)&(~PPPICMask));
    } else {
        outp(PPPICAddr+1,
             inp(PPPICAddr+1)|(PPPICMask));
    }
}


/*! This function will close the parallel port, restore interrupt vector table
    and turn off parallel port interrupts.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int PPClose(void){

    #ifdef DEBUG_STARTUP
        printf("Shutting down parallel port communication...\n");
    #endif

    PPIrqCtrl(DISABLE); // Disable interrupt on the parallel port. This should really disable all the interrupts but I still have to find a way to do it

    /* Clear the last interrupt request */
    PPClear();

    /* Set parallel port direction to output */
    outp(SPPControlPort,
         inp(SPPControlPort) & ~PP_DATA_DIR);

    /* Disable parallel port interrupt line (Bit 4 of Control Port = IRQE)*/
    PPIrqCtrl(DISABLE);

    /* Modify the PIC to ignore interrupt on the parallel port line */
    PicPPIrqCtrl(DISABLE); // Disable parallel port interrupt line in the PIC

    /* Restore original interrupt vector table */
    _dos_setvect(LD3PrimaryInterruptVector,
                 oldHandler);

    outp(SPPControlPort,
         inp(SPPControlPort)|SPP_CONTROL_INIT);  // Signal ARCOM not ready

// If you manage to disable all the interrupts, they should be re-enabled here!

    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}

/*! This function will transmit \p length bytes of data on the parallel port.
    The function has been written in assembly to increase the speed of
    execution.
    \param  length  an unsigned char */
void PPWrite(unsigned char length){
    unsigned char cnt;

    outp(SPPControlPort,
         (inp(SPPControlPort)/*^PP_IRQ_ENA*/)^PP_DATA_DIR);
    outp(EPPDataPort,
         length);

    for(cnt=0;
        cnt<length;
        cnt++){
        outp(EPPDataPort,
             PPTxBuffer[cnt]);
    }

    outp(SPPControlPort,
         (inp(SPPControlPort)/*^PP_IRQ_ENA*/)^PP_DATA_DIR);
}

/* Interrupt function receives CAN_MESSAGE_SIZE bytes from the parallel port and stores them in message */
static void interrupt far PPIntHandler(void){

    unsigned char cnt,cnt2,dump;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Received!\n");
    #endif /* DEBUG_PPCOM */

    /* Input the CAN header from the parallel port */
    #ifdef DEBUG_PPCOM
        printf("  CAN Header:\n");
    #endif /* DEBUG_PPCOM */
    for(cnt=0;
        cnt<CAN_RX_HEADER_SIZE;
        cnt++){
        PPRxBuffer[cnt]=inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        for(cnt=0;
            cnt<CAN_RX_HEADER_SIZE;
            cnt++){
            printf("    PPRxBuffer[%d]=0x%X\n",
                   cnt,
                   PPRxBuffer[cnt]);
        }
    #endif /* DEBUG_PPCOM */

    cnt2 = PPRxBuffer[CAN_RX_HEADER_SIZE-1];

    /* If the payload = 0 then is a monitor message */
    if(cnt2==CAN_MONITOR){
        #ifdef DEBUG_PPCOM
            printf("  Monitor message\n");
        #endif /* DEBUG_PPCOM */
        newCANMsg=1; // Notify program of new message

        #ifdef DEBUG_PPCOM
            printf("Interrupt Serviced!\n");
        #endif /* DEBUG_PPCOM */
        return;
    }

    /* Check the payload size against the limit: if it's to big, clear the
       interrupt and exit. We read the data anyway to prevent the AMBSI from
       getting stuck! */
    if(cnt2>CAN_RX_MAX_PAYLOAD_SIZE){
        #ifdef DEBUG_PPCOM
            printf("  Too much data! dumping...");
        #endif /* DEBUG_PPCOM */
        for(cnt=0;
            cnt<cnt2;
            cnt++){
            dump=inp(EPPDataPort);
        }
        PPClear();
        return;
    }

    /* If it's a control message, load the payload */
    #ifdef DEBUG_PPCOM
        printf("  Control message\n");
        printf("    Payload:\n");
    #endif /* DEBUG_PPCOM */
    for(cnt=0;
        cnt<cnt2;
        cnt++){
        PPRxBuffer[CAN_RX_HEADER_SIZE+cnt]=inp(EPPDataPort);
    }

    #ifdef DEBUG_PPCOM
        for(cnt=0;
            cnt<cnt2;
            cnt++){
            printf("      PPRxBuffer[%d]=0x%X\n",
                   CAN_RX_HEADER_SIZE+cnt,
                   PPRxBuffer[CAN_RX_HEADER_SIZE+cnt]);
        }
    #endif /* DEBUG_PPCOM */

    /* Notify program of new message */
    newCANMsg=1;

    #ifdef DEBUG_PPCOM
        printf("Interrupt Serviced!\n");
    #endif /* DEBUG_PPCOM */
}

/*! This function clears the IRQ of the parallel port. This will allow the port
    to receive another message. Until the IRQ is cleared, all the other icoming
    messages will be ignored.
    An alternative would be create a queue to store incoming messages. This
    could create problem with the 150us timing requirement on monitor messages. */
void PPClear(void){
    outp(PPPICAddr, PIC_INT_CLR);
}
