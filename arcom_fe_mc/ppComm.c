/*! \file   ppComm.c
    \brief  Parallel Port communication interface functions
    \todo   If possible improve the error handling for the link between AMBSI1
            and ARCOM

    <b> File informations: </b><br>
    Created: 2004/08/24 16:33:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ppComm.c,v 1.31 2008/03/10 22:15:43 avaccari Exp $

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

    printf("Initializing parallel port communication...\n");

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
    if(LD3PrimaryInterruptNo==0){
        outp(SIO_DATA_PORT,
             PP_DEFAULT_IRQ_NO);  // If disabled, enable and assign PP_IRQ_NO
        LD3PrimaryInterruptNo=PP_DEFAULT_IRQ_NO;
        storeError(ERR_PP,
                   0x01);                // Error 0x01 -> Parallel Port Interrupt Disable (Warning)
    }

    printf(" Current PP interrupt number: %d\n",
           LD3PrimaryInterruptNo);

    /* Configure pic mask and interrupt vector */
    if(LD3PrimaryInterruptNo<2 || LD3PrimaryInterruptNo>15){
        criticalError(ERR_PP,
                      0x02);  // Error 0x02 -> Parallel Port Interrupt out of Range
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
         LD3_MODE_REGISTER1);                                // Select the mode register
    outp(SIO_DATA_PORT,
         (inp(LD3_MODE_REGISTER1)&0xF8)|LD3_EPP_MODE_KEY);    // Switch mode to EPP 1.7
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX2);                                // Select the low order byte of the parallel port address
    SPPDataPort=inp(SIO_DATA_PORT);                                         // Write the low order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ADDRESS_INDEX1);                                // Select the high order byte of the parallel port address
    SPPDataPort|=inp(SIO_DATA_PORT)<<8;                                     // Write the high order byte into variable
    outp(SIO_INDEX_PORT,
         LD3_ACTIVATE_INDEX);                                // Select the activation register
    outp(SIO_DATA_PORT,
         LD3_ACTIVATE_KEY);                                   // Activate the registers
    outp(SIO_CONFIG_PORT,
         SIO_RUN_MODE_KEY);                                 // Exit Super I/0 configuration mode

    /* Define other PP ports */
    SPPStatusPort = SPPDataPort+1;  // Define SPP Status Port
    SPPControlPort = SPPDataPort+2; // Define SPP Control Port
    EPPDataPort = SPPDataPort+4;    // Define EPP Data Port

    /* Handshake readiness status with AMBSI */
    outp(SPPControlPort,
         (inp(SPPControlPort)|SPP_CONTROL_INIT)&~SPP_STATUS_SELECT); // INIT line high and SELECT line low
    while((inp(SPPStatusPort)&SPP_STATUS_SELECT)){
            storeError(ERR_PP,
                       0x03);                  // Error 0x03 -> Warning! Waiting for AMBSI board.
            waitSeconds(WAIT_ON_AMBSI_NOT_READY);    // Wait before checking again the ready status
    }

    /* Set parallel port direction to input */
    outp(SPPControlPort,
         inp(SPPControlPort) | PP_DATA_DIR);

    /* Set the interrupt vector of the parallel port to point to: PPIntHandler */
    PicPPIrqCtrl(ENABLE);
    PPIrqCtrl(DISABLE); // Disable interrupt on the parallel port. This should really disable all the interrupts but I still have to find a way to do it
    oldHandler = _dos_getvect(LD3PrimaryInterruptVector);   // Read the current interrupt vector corresponding to PP_IRQ_NO
    _dos_setvect(LD3PrimaryInterruptVector,
                 PPIntHandler);   // Replace the vector with one pointing to: PPIntHandler

    /* Clear any curren interrupt on the parallel port */
    PPClear();

    /* Enable parallel port interrupt line */
    PPIrqCtrl(ENABLE);

    /* Modify the PIC to acknowledge interrupt on the parallel port line */
    PicPPIrqCtrl(ENABLE); // Enable parallel port interrupt line in the PIC


// If you manage to disable all the interrupts, they should be re-enabled here!

    outp(SPPControlPort,
         inp(SPPControlPort)&~SPP_CONTROL_INIT);  // Signal ARCOM ready

    printf("done!\n\n"); // Initialization

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

    printf("Shutting down parallel port communication...\n");

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

    printf("done!\n\n");

    return NO_ERROR;
}

/*! This function will transmit \p length bytes of data on the parallel port.
    The function has been written in assembly to increase the speed of
    execution.
    \param  length  an unsigned char
    \todo   Fix the problem with the assembly code. It always sends out 8 bytes
            doesn't matter what the variable length is loaded with when it doesn't
            freeze the all firmware!!!
            Maybe I've to wait or check to make sure the transmission of the
            data is finished before I switch the parallel port direction to input. */
void PPWrite(unsigned char length){
    #ifdef __SMALL__
        _asm{
            /* Assumes Small Model */
            /* Push only the register that the compiler didn't push and that we are
               going to use after the compiler's push */
            push    ax
            push    es

            /* Disable PP interrupt and set direction to output */
            mov     dx,SPPControlPort       ; Select SPP control port
            in      al,dx                   ; Read control port
            xor     al,PP_IRQ_ENA           ; Disable parallel port interrupt line
            xor     al,PP_DATA_DIR          ; Set parallel port direction to output
            out     dx,al                   ; Update control port

            /* Write the first byte that contains the number of unsigned characters to be written */
            mov     dx,EPPDataPort          ; Select EPP data port
            mov     cl,length               ; Move number of byte to be transmitted in cl
            mov     ch,0x0                  ; Clear ch
            mov     al,cl                   ; Move number of byte to be transmitted in al
            out     dx,al                   ; Send length to EPP data port

            /* Write the rest of the unsigned characters */
            cld                             ; Clear increment direction
            mov     si,OFFSET PPTxBuffer    ; Load the offset of the Tx buffer
            mov     bx,ds                   ; Load the segment of the Tx buffer (Small model -> DS)
            mov     es,bx                   ; Setup es to hold the source segment
            rep     outsb                   ; Output a string cx long from dx into es:si one byte at a time

            /* Maybe I've to wait or check to make sure the transmission of the data is
               finished before I switch the parallel port direction to input */

            /* Set direction to input and enable PP interrupt */
            mov     dx,SPPControlPort       ; Select SPP control port
            xor     al,PP_DATA_DIR          ; Set parallel port direction to input
            xor     al,PP_IRQ_ENA           ; Enable parallel port interrupt line
            out     dx,al                   ; Update control port

            quit:
            pop     es
            pop     ax
        }
    #else
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
    #endif /* __SMALL__ */
}

/* Interrupt function receives CAN_MESSAGE_SIZE bytes from the parallel port and stores them in message */
static void interrupt far PPIntHandler(void){

    #ifdef __SMALL__
        _asm{
            /* Assumes Small Model */
            /* Interrupt function -> Everything is pushed on the stack */
            /* Input 16 bytes (the full CAN message) from the EPP and store it in message */
            cld                             ; Clear increment direction
            mov     di,OFFSET PPRxBuffer    ; Load the offset of the Rx buffer
            mov     cx,ds                   ; Load the segment of the Rx buffer (Small model -> DS)
            mov     es,cx                   ; Setup es to hold the destination segment
            mov     dx,EPPDataPort          ; Load the port number to input data from
            mov     cx,CAN_RX_HEADER_SIZE   ; Load the number of bytes to be moved
            rep     insb                    ; Input a string cx long from dx into es:di one byte at a time

            mov     cl, byte ptr -0x1[di]   ; Load cl with last byte of message (if control cl contains the number of extra bytes to download)
            cmp     cl,CAN_RX_MAX_PAYLOAD_SIZE ; Check the received payload size against the maximum allowed
            ja      ignore                  ; If greater then assume error and ignore other data

            cmp     cl,CAN_MONITOR          ; Test if it is a monitor message
            je      end

            rep     insb                    ; Input a string cx long from dx into es:di one byte at a time

            end:
            /* Notify program of new message arrival */
            mov     byte ptr newCANMsg, 0x01

            ignore:
        }
    #else

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
            #ifdef DEBUG_PPCOM
                printf("    PPRxBuffer[%d]=0x%X\n",
                       cnt,
                       PPRxBuffer[cnt]);
            #endif /* DEBUG_PPCOM */

        }

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
            #ifdef DEBUG_PPCOM
                printf("      PPRxBuffer[%d]=0x%X\n",
                       CAN_RX_HEADER_SIZE+cnt,
                       PPRxBuffer[CAN_RX_HEADER_SIZE+cnt]);
            #endif /* DEBUG_PPCOM */
        }

        /* Notify program of new message */
        newCANMsg=1;

        #ifdef DEBUG_PPCOM
            printf("Interrupt Serviced!\n");
        #endif /* DEBUG_PPCOM */

    #endif /* __SMALL__ */
}

/*! This function clears the IRQ of the parallel port. This will allow the port
    to receive another message. Until the IRQ is cleared, all the other icoming
    messages will be ignored.
    An alternative would be create a queue to store incoming messages. This
    could create problem with the 150us timing requirement on monitor messages. */
void PPClear(void){
    #ifdef __SMALL__
        _asm {
            /* Assumes Small Model */
            /* Push register used that the compiler didn't push */
            push    ax;
            /* Clear the interrupt request */
            mov     al,PPPICAddr
            out     PIC_INT_CLR,al
            /* Pop register back */
            pop     ax;
        }
    #else
        outp(PPPICAddr,
             PIC_INT_CLR);
    #endif /* __SMALL__ */
}
