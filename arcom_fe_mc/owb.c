/*! \file   owb.c
    \brief  One Wire Bus functions

    <b> File informations: </b><br>
    Created: 2007/09/05 14:33:02 by avaccari

    <b> CVS informations: </b><br>
    \$Id: owb.c,v 1.2 2007/10/02 22:04:58 avaccari Exp $

    This files contains all the functions necessary to handle the one wire bus
    communication. */

/*  Includes */
#include <conio.h>      /* inp, outp */
#include <stdio.h>      /* printf */

#include "serialMux.h"
#include "timer.h"
#include "owb.h"
#include "error.h"
#include "globalDefinitions.h"
#include "debug.h"
#include "iniWrapper.h"

/* Globals */
/* Externs */
unsigned char esnDevicesFound;
unsigned char okEsnDevicesFound;
unsigned char koEsnDevicesFound;
unsigned char ESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE];
unsigned char okESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE];
unsigned char koESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE];


// Add description
int owbInit(void){

    /* A few local to deal with the initialization */
    CFG_STRUCT dataIn;
    unsigned char esnDevicesStored;
    unsigned char storedESNS[MAX_DEVICES_NUMBER][SERIAL_NUMBER_SIZE];
    unsigned char device;
    unsigned char loop;
    unsigned char swap;
    int TData[SEARCH_BYTES_LENGTH];
    int RData[SEARCH_BYTES_LENGTH];
    int timedOut=0; // A local to keep track of time out errors

    printf("Initializing One Wire Bus...\n");

    /* Reset the one wire master in the FPGA. The data sent is not important */
    printf(" - Reset the one wire master...");
    outp(MUX_OWB_RESET,
         0);
    printf("done!\n"); // Reset one wire master

    /* Select 10-12 MHz clock */
    printf(" - Set up clock rate...");
    outp(MUX_OWB_CLK_DIV,
         OWB_10_12MHZ);
    printf("done!\n"); // Setup clock

    /* Select Long Line Mode and Presence Pulse Masking Mode */
    printf(" - Set up LLM and PPM mode...");
    outp(MUX_OWB_CONTROL,
         OWB_LLM|OWB_PPM);
    printf("done!\n"); // Setup LLM+PPM

    /* Initialize the device discovery algorithm */
    printf(" - Initializing device search algorithm...");
    RecoverROM(NULL,
               TData,
               NULL);
    printf("done!\n"); // Initialize search algorithm

    /* Find available devices */
    printf(" - Searching devices...\n");
    for(device=0;
        device<MAX_DEVICES_NUMBER;
        device++){

        printf("   - Searching device %d...\n",
               device);

        /* Send the bus reset and check for presence pulse. */
        #ifdef DEBUG_OWB
            printf("     - Send bus reset...\n");
        #endif /* DEBUG_OWB */

        /* Set up for 10 seconds and start the asynchronous timer */
        if(startAsyncTimer(TIMER_OWB_RESET,
                           TIMER_TO_OWB_RESET)==ERROR){
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
            criticalError(ERR_OWB,
                          0x04); // Error 0x04 -> Timeout while waiting for the bus reset
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
        outp(MUX_OWB_COMMAND,
             ACC_SEARCH_MODE);
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
            #endif /* DEBUG_OWB */
            printf("     done!\n"); // Searching device No.x
            break;
        }
        #ifdef DEBUG_OWB
            printf("done!\n"); // Search tree state
        #endif /* DEBUG_OWB */

        printf("     done!\n"); // Searching device No.x

    }
    printf("   done!\n"); // Search devices

    /* If the maximum number of devices was reached, it is likely that there is
       a problem with the bus. Notify the system and set the number of found
       devices to 0. */
    if(device==MAX_DEVICES_NUMBER){
        storeError(ERR_OWB,
                   0x03); // Error 0x03 -> Maximum number of devices reached
        esnDevicesFound = 0;
    } else {
        /* If not, store the number of devices found. */
        esnDevicesFound = device+1;
    }

    printf("   - Devices found: %d\n",
           esnDevicesFound);

    /* Set the ESN of the FEMC at index 0 */
    for(device=1;
        device<esnDevicesFound;
        device++){
        if(ESNS[device][0]==0x10){
            for(loop=0;
                loop<SERIAL_NUMBER_SIZE;
                loop++){
                swap=ESNS[0][loop];
                ESNS[0][loop]=ESNS[device][loop];
                ESNS[device][loop]=swap;
            }
            break; // There should be only one 0x10 in the group, the one on the FEMC
        }
    }

    /* Print devices list */
    for(device=0;
        device<esnDevicesFound;
        device++){
        printf("     - ESN%d: %X %X %X %X %X %X %X %X\n",
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


    /* Check found list against stored list. */
    printf(" - Checking against stored list...\n");
    /* Load the number of stored devices from the file */
    dataIn.
     Name=ESNS_NO_KEY;
    dataIn.
     VarType=Cfg_Byte;
    dataIn.
     DataPtr=&esnDevicesStored;

    /* Access configuration file, if error, store in status register. */
    if(myReadCfg(ESNS_DATA_FILE,
                 ESNS_NO_SECTION,
                 &dataIn,
                 ESNS_NO_EXPECTED)==ERROR){
// update status register *******************************************************************
    } else {
        /* If no error accessing the file, process the file */
        printf("   - Number of stored devices: %d\n",
               esnDevicesStored);

        /* Load the Stored list in memory */
        dataIn.
         VarType=Cfg_HB_Array;

        for(device=0;
            device<esnDevicesStored;
            device++){
            dataIn.
             Name=ESNS_DEVICE_KEY(device);
            dataIn.
             DataPtr=storedESNS[device];

            /* Access configuration file, if error, store in status register and
               exit loop */
            if(myReadCfg(ESNS_DATA_FILE,
                         ESNS_DEVICE_SECTION,
                         &dataIn,
                         ESNS_DEVICE_EXPECTED)==ERROR){
// update status register *******************************************************************
                break;
            }
        }
        /* Print devices list */
        for(device=0;
            device<esnDevicesStored;
            device++){
            printf("     - ESN%d: %X %X %X %X %X %X %X %X\n",
                   device,
                   storedESNS[device][0],
                   storedESNS[device][1],
                   storedESNS[device][2],
                   storedESNS[device][3],
                   storedESNS[device][4],
                   storedESNS[device][5],
                   storedESNS[device][6],
                   storedESNS[device][7]);
        }



    }

    printf("   done!\n"); // Check against stored list


        /* Update stored list with new list. */
        printf(" - Updating stored list...\n");
        printf("   done!\n");



    printf("done!\n\n");

    return NO_ERROR;
}


/* Write to the one wire bus */
int writeOwb(int data){
    /* Write the data on the bus */
    outp(MUX_OWB_TXRX,
         data);

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
        storeError(ERR_OWB,
                   0x02); // Error 0x02 -> Presence pulse not detected
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
                       TIMER_TO_OWB_IRQ)==ERROR){
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
        storeError(ERR_OWB,
                   0x01); // Error 0x01 -> Timeout while waiting for the IRQ
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


