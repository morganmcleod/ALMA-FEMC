/*! \file   serialMux.c
    \brief  Serial multiplexing board functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to control the serial
    multiplexing board.

    The functions in this module provide the lowest level communication with
    the hardware.
    This module should contain the hardware depended part of the code.
    No check is actually performed to insure that the data is transmitted to the
    serial mux board through ISA commands.
    The only way to do this would be to actually perform a read right after the
    write to insure that the written data is correct. This check is available
    only for few of the addresses due to the FPGA configuration. */

/* Includes */
#include <conio.h>      /* inpw, outpw */
#include <stdio.h>      /* printf */

#include "error.h"
#include "serialMux.h"
#include "timer.h"
#include "debug.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
FRAME frame; /*! This variable is used to create the serial frame to be
                 handled by the multiplexing board. */

int LATCH_DEBUG_SERIAL_WRITE;

/* Write the data through the Mux board */
/*! This function will trasmit the current courrent \ref frame content to the
    selected device.

    This function performs the following operations:
        -# Check the busy status to verify the synchronous serial bus is ready
           to begin a new cycle
        -# Select the desired port/device
            - by writing to the port select register
        -# Load the data register with the data output word(s). Bits must be
           left-justified and they start from bit 15 of the most significant
           word in the data register. The data is most significant bit first.
        -# Write the desired data word length to be transmitted into the lenght
           register
        -# Write the command register with the desired command. This will
           initiate the serial transfer

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int writeMux(void){
    /* Check if the lenght is within the hardware limit (40 bits) */
    if(frame.
        dataLength>FRAME_DATA_BIT_SIZE){
        storeError(ERR_SERIAL_MUX, ERC_COMMAND_VAL); //Data length out of range
        return ERROR;
    }

    /* 1 - Wait on busy status */
    if(waitOnBusy()==ERROR){
        return ERROR;
    }

    /* 2 - Select the desired port/device. The check on the availability of the
           device on the selected port should have been done by the CAN message
           handlers. At the point of this call the software should already have
           returned if the addressed device is not available. */
    outpw(MUX_PORT_ADD,
          frame.
           port);

    /* 3 - Load the data registers. */
    outpw(MUX_DATA_ADD(FRAME_DATA_LSW),
          frame.
           data[FRAME_DATA_LSW]); // Least significant word
    outpw(MUX_DATA_ADD(FRAME_DATA_MDL),
          frame.
           data[FRAME_DATA_MDL]); // Middle word
    outpw(MUX_DATA_ADD(FRAME_DATA_MSW),
          frame.
           data[FRAME_DATA_MSW]); // Most significant word

    /* 4 - Write the outgoing data lenght register with the number of bits to be
           sent. */
    outpw(MUX_WLENGTH_ADD,
          frame.
           dataLength);

    /* 5 - Write the command register. This will initiate the transmission of
           data. */
    outpw(MUX_COMMAND_ADD,
          frame.
           command);

    #ifdef DEBUG_SERIAL_WRITE
        if (LATCH_DEBUG_SERIAL_WRITE) {
            LATCH_DEBUG_SERIAL_WRITE = 0;
            printf("            (0x%04X) <- Frame.port: 0x%04X\n",
                   MUX_PORT_ADD,
                   frame.
                    port);
            printf("            (0x%04X) <- Frame.data[LSW]: 0x%04X\n",
                   MUX_DATA_ADD(FRAME_DATA_LSW),
                    frame.
                     data[FRAME_DATA_LSW]);
            printf("            (0x%04X) <- Frame.data[MDL]: 0x%04X\n",
                   MUX_DATA_ADD(FRAME_DATA_MDL),
                   frame.
                    data[FRAME_DATA_MDL]);
            printf("            (0x%04X) <- Frame.data[MSW]: 0x%04X\n",
                   MUX_DATA_ADD(FRAME_DATA_MSW),
                   frame.
                    data[FRAME_DATA_MSW]);
            printf("            (0x%04X) <- Frame.dataLength: 0x%04X\n",
                   MUX_WLENGTH_ADD,
                   frame.
                    dataLength);
            printf("            (0x%04X) <- Frame.command: 0x%04X\n",
                   MUX_COMMAND_ADD,
                   frame.
                    command);
        }
    #endif /* DEBUG_SERIAL_WRITE */

    return NO_ERROR;

}

/* Reads the data through the Mux board */
/*! This function will read the required data from the selected device into the
    current \ref frame.

    This function performs the following operations:
        -# Check the busy status to verify the synchronous serial bus is ready
           to begin a new cycle
        -# Select the desiref port/device
            - by writing to the port select register
        -# Write the desired data word length to be read into the lenght
           register
        -# Write the command register with the desired command. This will
           initiate the serial transfer
        -# Check the busy status to verify the read cycle is complete
        -# Load the frame input word(s) with the data register. Bits will be
           right-justified and they end with bit 0 of the least significant
           word in the data register. The data is most significant bit first.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int readMux(void){
    /* Check if the lenght is within the hardware limit (40 bits) */
    if(frame.
        dataLength>FRAME_DATA_BIT_SIZE){
        storeError(ERR_SERIAL_MUX, ERC_COMMAND_VAL); //Data length out of range
        return ERROR;
    }

    /* 1 - Wait on busy status */
    if(waitOnBusy()==ERROR){
        return ERROR;
    }

    /* 2 - Select the desired port/device. The check on the availability of the
           device on the selected port should have been done by the CAN message
           handlers. At the point of this call the software should already have
           returned if the addressed device is not available. */
    outpw(MUX_PORT_ADD,
          frame.
           port);

    /* 3 - Write the incoming data lenght register with the number of bits to be
           received. */
    outpw(MUX_RLENGTH_ADD,
          frame.
           dataLength);

    /* 4 - Write the command register. This will initiate the transmission of
           data. */
    outpw(MUX_COMMAND_ADD,
          frame.
           command);

    #ifdef DEBUG_SERIAL_READ
        printf("            (0x%04X) <- Frame.port: 0x%04X\n",
               MUX_PORT_ADD,
               frame.
                port);
        printf("            (0x%04X) <- Frame.dataLength: 0x%04X\n",
               MUX_RLENGTH_ADD,
               frame.
                dataLength);
        printf("            (0x%04X) <- Frame.command: 0x%04X\n",
               MUX_COMMAND_ADD,
               frame.
                command);
    #endif /* DEBUG_SERIAL_READ */

    /* 5 - Wait on busy status */
    if(waitOnBusy()==ERROR){
        return ERROR;
    }

    /* 6 - Load the data registers */
    frame.
     data[FRAME_DATA_MSW]=inpw(MUX_DATA_ADD(FRAME_DATA_MSW)); // Most significant word
    frame.
     data[FRAME_DATA_MDL]=inpw(MUX_DATA_ADD(FRAME_DATA_MDL)); // Middle word
    frame.
     data[FRAME_DATA_LSW]=inpw(MUX_DATA_ADD(FRAME_DATA_LSW)); // Least significant word

    #ifdef DEBUG_SERIAL_READ
        printf("            (0x%04X) -> Frame.data[LSW]: 0x%04X\n",
               MUX_DATA_ADD(FRAME_DATA_LSW),
               frame.
                data[FRAME_DATA_LSW]);
        printf("            (0x%04X) -> Frame.data[MDL]: 0x%04X\n",
               MUX_DATA_ADD(FRAME_DATA_MDL),
               frame.
                data[FRAME_DATA_MDL]);
        printf("            (0x%04X) -> Frame.data[MSW]: 0x%04X\n",
               MUX_DATA_ADD(FRAME_DATA_MSW),
               frame.
                data[FRAME_DATA_MSW]);
    #endif /* DEBUG_SERIAL_READ */

    return NO_ERROR;
}

/* Wait on busy state of the mux board */
/* This function check if the serial mux board is still busy sending dealing
   with a previously received message.
   If after a certain time the serial mux board is still busy then an value of
   ERROR is returned to the calling function. */
static int waitOnBusy(void){

    int timedOut=0; // A local to keep track of time out errors

    /* Setup for 1 seconds and start the asynchronous timer */
    if(startAsyncTimer(TIMER_SERIAL_MUX,
                       TIMER_TO_SERIAL_MUX,
                       FALSE)==ERROR){
        return ERROR;
    }

    /* Wait for the mux board to be ready or for the timer to expire. */
    do {
        #ifdef DEBUG_SERIAL_WAIT
            printf("             Waiting on mux board to get ready...\n");
        #endif /* DEBUG_SERIAL_WAIT */
        timedOut=queryAsyncTimer(TIMER_SERIAL_MUX);
        if(timedOut==ERROR){
            return ERROR;
        }
    } while((inpw(MUX_BUSY_ADD)&MUX_BUSY_MASK)&&!timedOut);


    /* If the timer has expired signal the error */
    if(timedOut==TIMER_EXPIRED){
        storeError(ERR_SERIAL_MUX, ERC_HARDWARE_TIMEOUT); //Timeout while waiting for the mux board to become ready
        return ERROR;
    }
    /* In case of no error, clear the asynchronous timer. */
    if(stopAsyncTimer(TIMER_SERIAL_MUX)==ERROR){
        return ERROR;
    }

    return NO_ERROR;
}

/* Serial Mux Board initialization status */
/*! This function will check for the ready state of the serial mux board.
    Once the FPGA in the board is ready to receive commands then a read access
    performed on \ref MUX_FPGA_RDY_ADD should return \ref FPGA_READY.
    If communication with the FPGA cannot be established a critcal error will be
    returned and the program will terminate.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int serialMuxInit(void){

    #ifdef DEBUG_STARTUP
        printf("Initializing Serial Multiplexer board...\n");
    #endif

    /* Check if the FPGA is ready. The check for ADD-2 can be removed once the
       FPGA has been fixed and all the versions are consistent. */
    if((inpw(MUX_FPGA_RDY_ADD)!=FPGA_READY)&&(inpw(MUX_FPGA_RDY_ADD-2)!=FPGA_READY)){

        printf("\n\nserialMux: CRITICAL ERROR - The FPGA is not ready\n\n");
        criticalError(ERR_SERIAL_MUX, ERC_FPGA_NOT_READY);
        return ERROR;
    }

    #ifdef DEBUG_STARTUP
        printf("done!\n\n");
    #endif

    return NO_ERROR;
}

