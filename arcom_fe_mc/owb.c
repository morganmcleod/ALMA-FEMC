/*! \file   owb.c
    \brief  One Wire Bus functions

    <b> File information: </b><br>
    Created: 2007/09/05 14:33:02 by avaccari

    This file contains all the functions necessary to handle the one wire bus
    communication. */

/*  Includes */
#include <conio.h>      /* inp, outp */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* rand, srand */
#include <time.h>       /* clock, time */

#include "serialMux.h"
#include "timer.h"
#include "owb.h"
#include "error.h"
#include "globalDefinitions.h"
#include "debug.h"
#include "iniWrapper.h"
#include "pegasus.h"

/* Globals */
/* Static */
/* Externs */
unsigned char esnDevicesFound;
unsigned char ESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE];


/*! This function initializes the OWB and evaluates if the simulator should be
    used or not.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */

int owbInit(void){
    #ifdef DEBUG_STARTUP
        printf("Initializing One Wire Bus...\n");
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}

/*! This fuction actually gather the ESNs from the OWB. If a simulator is used
    it will generate the ESN from the information contained in the configuration
    file.
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int owbGetEsn(void){

    /* A few local to deal with the initialization */
    unsigned char device;
    unsigned char loop;
    int TData[SEARCH_BYTES_LENGTH];
    int RData[SEARCH_BYTES_LENGTH];
    int timedOut=0; // A local to keep track of time out errors

    #ifdef DEBUG_OWB
        printf("Gathering ESN... ");
    #endif

    /* Enable the section of the bus extending outside the FEMC */
    #ifdef DEBUG_OWB
        printf(" - Enabling external OWB...");
    #endif /* DEBUG_OWB */

    outp(MUX_OWB_ENABLE, ENABLE);

    #ifdef DEBUG_OWB
        printf("done!\n");
    #endif /* DEBUG_OWB */

    /* Reset the one wire master in the FPGA. The data sent is not important */
    #ifdef DEBUG_OWB
        printf(" - Reset the one wire master...");
    #endif /* DEBUG_OWB */

    outp(MUX_OWB_RESET, 0);

    #ifdef DEBUG_OWB
        printf("done!\n"); // Reset one wire master
    #endif /* DEBUG_OWB */

    /* Select 10-12 MHz clock */
    #ifdef DEBUG_OWB
        printf(" - Set up clock rate...");
    #endif /* DEBUG_OWB */

    outp(MUX_OWB_CLK_DIV, OWB_10_12MHZ);

    #ifdef DEBUG_OWB
        printf("done!\n"); // Setup clock
    #endif /* DEBUG_OWB */

    /* Select Long Line Mode and Presence Pulse Masking Mode */
    #ifdef DEBUG_OWB
        printf(" - Set up LLM and PPM mode...");
    #endif /* DEBUG_OWB */

    outp(MUX_OWB_CONTROL, OWB_LLM | OWB_PPM);

    #ifdef DEBUG_OWB
        printf("done!\n"); // Setup LLM+PPM
    #endif /* DEBUG_OWB */

    /* Initialize the device discovery algorithm */
    #ifdef DEBUG_OWB
        printf(" - Initializing device search algorithm...");
    #endif /* DEBUG_OWB */

    RecoverROM(NULL, TData, NULL);

    #ifdef DEBUG_OWB
        printf("done!\n"); // Initialize search algorithm

        /* Find available devices */
        printf(" - Searching devices...\n");
    #endif /* DEBUG_OWB */

    for(device = 0;
        device < MAX_DEVICES_NUMBER;
        device++){

            #ifdef DEBUG_OWB
                printf("   - Searching device %d...\n",
                       device);
            #endif /* DEBUG_OWB */

        /* Send the bus reset and check for presence pulse. */
        #ifdef DEBUG_OWB
            printf("     - Send bus reset...\n");
        #endif /* DEBUG_OWB */

        /* Set up for 10 seconds and start the asynchronous timer */
        if(startAsyncTimer(TIMER_OWB_RESET,
                           TIMER_TO_OWB_RESET,
                           FALSE)==ERROR){
            return ERROR;
        }

        /* Wait for the one wire bus to be resetted or for the timer to
           expire. */
        do {
            timedOut=queryAsyncTimer(TIMER_OWB_RESET);
            if(timedOut==ERROR){
                return ERROR;
            }
        } while(owbReset()&&!timedOut);

        /* If the timer has expired signal the error */
        if(timedOut==TIMER_EXPIRED){
            storeError(ERR_OWB, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the bus reset
            return ERROR;
        }

        /* In case of no error, clear the asynchronous timer. */
        if(stopAsyncTimer(TIMER_OWB_RESET)==ERROR){
            return ERROR;
        }

        #ifdef DEBUG_OWB
            printf("       done!\n"); // Bus reset
        #endif /* DEBUG_OWB */

        /* Send "search ROM" command to the bus */
        #ifdef DEBUG_OWB
            printf("     - Send search ROM code...\n");
        #endif /* DEBUG_OWB */

        writeOwb(SEARCH_ROM_CODE);

        #ifdef DEBUG_OWB
            printf("       done!\n"); // Search ROM code
        #endif /* DEBUG_OWB */

        /* Enter accelerated search mode */
        #ifdef DEBUG_OWB
            printf("     - Enter accelerated search mode...");
        #endif /* DEBUG_OWB */

        outp(MUX_OWB_COMMAND, ACC_SEARCH_MODE);

        #ifdef DEBUG_OWB
            printf("done!\n"); // Enter accelerated search
        #endif /* DEBUG_OWB */

        /* Transmit first set of search data */
        #ifdef DEBUG_OWB
            printf("     - Sending search data...\n");
        #endif /* DEBUG_OWB */

        for(loop=0;
            loop<SEARCH_BYTES_LENGTH;
            loop++){
            RData[loop]=writeOwb(TData[loop]);
        }

        #ifdef DEBUG_OWB
            printf("       done!\n"); // Send search data
        #endif /* DEBUG_OWB */

        /* Check state of search tree */
        #ifdef DEBUG_OWB
            printf("     - Check state of search tree...");
        #endif /* DEBUG_OWB */

        if(RecoverROM(RData,
                      TData,
                      ESNS[device])==TRUE){

            #ifdef DEBUG_OWB
                printf("done!\n"); // Search tree state
                printf("     done!\n"); // Searching device No.x
            #endif /* DEBUG_OWB */

            break;
        }

        #ifdef DEBUG_OWB
            printf("done!\n"); // Search tree state
            printf("     done!\n"); // Searching device No.x
        #endif /* DEBUG_OWB */

    }
    #ifdef DEBUG_OWB
        printf("   done!\n"); // Search devices
    #endif

    /* Disable the section of the bus extending outside the FEMC */
    #ifdef DEBUG_OWB
        printf("   - Disabling external OWB...");
    #endif /* DEBUG_OWB */

    outp(MUX_OWB_ENABLE, DISABLE);

    #ifdef DEBUG_OWB
        printf("done!\n");
    #endif /* DEBUG_OWB */

    /* If the maximum number of devices was reached, it is likely that there is
       a problem with the bus. Notify the system and set the number of found
       devices to 0. */
    if(device==MAX_DEVICES_NUMBER){
        #ifdef DEBUG_STARTUP
            printf("\n\nWARNING - Maximum number of ESN devices reached.\n\n");
        #endif /* DEBUG_STARTUP */

        storeError(ERR_OWB, ERC_NO_MEMORY); //Maximum number of devices reached
        esnDevicesFound = 0;
    } else {
        /* If not, store the number of devices found. */
        esnDevicesFound = device+1;
    }

    printf("OWB - Devices found: %d\n",
           esnDevicesFound);

    /* Print devices list */
    for(device = 0;
        device < esnDevicesFound;
        device++)
    {
        printf("    - ESN%d: %02X %02X %02X %02X %02X %02X %02X %02X\n",
               device,
               ESNS[device][0],
               ESNS[device][1],
               ESNS[device][2],
               ESNS[device][3],
               ESNS[device][4],
               ESNS[device][5],
               ESNS[device][6],
               ESNS[device][7]);
    }
    #ifdef DEBUG_OWB
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}


/* Write to the one wire bus */
int writeOwb(int data){
    /* Write the data on the bus */
    outp(MUX_OWB_TXRX, data);

    /* Since every time data is written to the bus, data is received in the
       receive buffer, we need to wait for the receive buffer full flag to be
       set and then clear the receive buffer. */
    if(waitIrq(IRQ_RX_BUF_FULL)==ERROR){
        return ERROR;
    }

    return inp(MUX_OWB_TXRX);
}


/* Reset the one wire bus */
int owbReset(void){

    /* Send bus reset */
    outp(MUX_OWB_COMMAND,
         OWB_RESET);

    /* Wait for the presence detect ready. This is a signal controlled by a
       timer and it's activated by the one wire master after a well defined
       ammount of time necessary for a slave to answer. */
    #ifdef DEBUG_OWB
        printf("       - Wait for presence pulse irq...\n");
    #endif /* DEBUG_OWB */
    if(waitIrq(IRQ_PRESENCE_PULSE)==ERROR){
        return ERROR;
    }
    #ifdef DEBUG_OWB
        printf("         done!\n");
    #endif /* DEBUG_OWB */

    /* Check for the presence pulse. Once the timer has expired then if any
       device is available on the bus it should have issued a presence pulse. */
    #ifdef DEBUG_OWB
        printf("       - Check the presence pulse...\n");
    #endif /* DEBUG_OWB */
    if((inp(MUX_OWB_IRQ)&PRESENCE_PULSE_MASK)==TRUE){
        storeError(ERR_OWB, ERC_HARDWARE_TIMEOUT); //Presence pulse not detected
        return ERROR;
    }
    #ifdef DEBUG_OWB
        printf("         done!\n");
    #endif /* DEBUG_OWB */

    return NO_ERROR;
}


/* Wait for interrupt */
int waitIrq(unsigned char irq){

    int timedOut=0; // A local to keep track of time out errors

    /* Setup for 1 seconds and start the asynchronous timer */
    if(startAsyncTimer(TIMER_OWB_IRQ,
                       TIMER_TO_OWB_IRQ,
                       FALSE)==ERROR){
        return ERROR;
    }

    /* Wait for the one wire bus to get the pulse detection or for the timer to
       expire. */
    while(((inp(MUX_OWB_IRQ)&irq)!=irq)&&!timedOut){
        timedOut=queryAsyncTimer(TIMER_OWB_IRQ);
        if(timedOut==ERROR){
            return ERROR;
        }
    }

    /* If the timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_OWB, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the IRQ
        return ERROR;
    }

    /* In case of no error, clear the asynchronous timer. */
    if(stopAsyncTimer(TIMER_OWB_IRQ)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}

/////////////////////////////////////////////////////////////////////////////////
// RecoverROM performs two functions. Given 16 bytes of receive data taken from
// the 1-Wire Master during a Search ROM function, it will extract the ROM code
// found into an 8 byte array and it will generate the next 16 bytes to be trans-
// mitted during the next Search ROM.
// RecoverROM must be initialized by sending a NULL pointer in ReceivedData. It
// will write 16 bytes of zeros into TransmitData and clear the discrepancy tree.
// The discrepancy tree keeps track of which ROM discrepancies have already been
// explored.
// RecoverROM also returns a value telling whether there are any more ROM codes to
// be found. If a zero is returned, there are still discrepancies. If a one is
// returned all ROMs on the bus have been found. Running RecoverROM again in this
// case will result in repeating ROM codes already found
////////////////////////////////////////////////////////////////////////////////
int RecoverROM(int* ReceiveData, int* TransmitData, unsigned char* ROMCode)
{
  int loop;
  int result;
  int TROM[64];   // the transmit value being generated
  int RROM[64];   // the ROM recovered from the received data
  int RDIS[64];   // the discrepancy bits in the received data

  static int TREE[64];   // used to keep track of which discrepancy bits have
                         // already been flipped.

  // If receivedata is NULL, this is the first run. Transmit data should be all
  // zeros, and the discrepancy tree must also be reset.

  if(ReceiveData == NULL)
  {
    for(loop = 0; loop < 64; loop++) TREE[loop] = 0;
    for(loop = 0; loop < 16; loop++) TransmitData[loop] = 0;
    return 1;
  }
  // de-interleave the received data into the new ROM code and the discrepancy bits
  for(loop = 0; loop < 16; loop++)
  {
    if((ReceiveData[loop] & 0x02) == 0x00) RROM[loop*4] = 0; else RROM[loop*4 ] = 1;
    if((ReceiveData[loop] & 0x08) == 0x00) RROM[loop*4+1] = 0; else RROM[loop*4+1] = 1;
    if((ReceiveData[loop] & 0x20) == 0x00) RROM[loop*4+2] = 0; else RROM[loop*4+2] = 1;
    if((ReceiveData[loop] & 0x80) == 0x00) RROM[loop*4+3] = 0; else RROM[loop*4+3] = 1;

    if((ReceiveData[loop] & 0x01) == 0x00) RDIS[loop*4] = 0; else RDIS[loop*4 ] = 1;
    if((ReceiveData[loop] & 0x04) == 0x00) RDIS[loop*4+1] = 0; else RDIS[loop*4+1] = 1;
    if((ReceiveData[loop] & 0x10) == 0x00) RDIS[loop*4+2] = 0; else RDIS[loop*4+2] = 1;
    if((ReceiveData[loop] & 0x40) == 0x00) RDIS[loop*4+3] = 0; else RDIS[loop*4+3] = 1;
  }

  // initialize the transmit ROM to the recovered ROM

  for(loop = 0; loop < 64; loop++) TROM[loop] = RROM[loop];

  // work through the new transmit ROM backwards setting every bit to 0 until the
  // most significant discrepancy bit which has not yet been flipped is found.
  // The transmit ROM bit at that location must be flipped.

  for(loop = 63; loop >= 0; loop--)
  {
    // This is a new discrepancy bit. Set the indicator in the tree, flip the
    // transmit bit, and then break from the loop.

    if((TREE[loop] == 0) && (RDIS[loop] == 1) && (TROM[loop] == 0))
    {
      TREE[loop] = 1;
      TROM[loop] = 1;
      break;
    }
    if((TREE[loop] == 0) && (RDIS[loop] == 1) && (TROM[loop] == 1))
    {
      TREE[loop] = 1;
      TROM[loop] = 0;
      break;
    }

    // This bit has already been flipped, remove it from the tree and continue
    // setting the transmit bits to zero.

    if((TREE[loop] == 1) && (RDIS[loop] == 1)) TREE[loop] = 0;
    TROM[loop] = 0;
  }
  result = loop;   // if loop made it to -1, there are no more discrepancy bits
                   // and the search can end.

  // Convert the individual transmit ROM bit into a 16 byte format
  // every other bit is don't care.

  for(loop = 0; loop < 16; loop++)
  {
    TransmitData[loop] = (TROM[loop*4]<<1) +
                         (TROM[loop*4+1]<<3) +
                         (TROM[loop*4+2]<<5) +
                         (TROM[loop*4+3]<<7);
  }

  // Convert the individual recovered ROM bits into an 8 byte format

  for(loop = 0; loop < 8; loop++)
  {
    ROMCode[loop] = (RROM[loop*8]) +
                    (RROM[loop*8+1]<<1) +
                    (RROM[loop*8+2]<<2) +
                    (RROM[loop*8+3]<<3) +
                    (RROM[loop*8+4]<<4) +
                    (RROM[loop*8+5]<<5) +
                    (RROM[loop*8+6]<<6) +
                    (RROM[loop*8+7]<<7);
  }
  if(result == -1) return 1;   // There are no DIS bits that haven't been flipped
                               // Tell the main loop the seach is over
  return 0;   // else continue
}


