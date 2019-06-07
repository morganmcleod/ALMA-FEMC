/*! \file   serialInterface.c
    \brief  Serial Interface functions

    <b> File information: </b><br>
    Created: 2004/08/24 16:24:39 by avaccari

    This file contains all the functions necessary to communcate with the
    serial mux board drivers.

    This modules builds the frame containing the information necessary for the
    hardware to perfrom one access to the synchronous serial bus.

    The scope of this module is to provide a layer between all the different
    modules that have to access the hardware and the pure hardware interface
    drivers.
    */

/* Includes */
#include <string.h>     /* memcpy */

#include "error.h"
#include "serialInterface.h"
#include "serialMux.h"
#include "frontend.h"

/* Globals */
/* Externs */
/* Statics */

/* Serial Access */
/*! This function perform all the data manipulation necessary to fill up the
    \ref FRAME that will be utilized by the low level driver to communicate with
    the multiplexing board.

    The data can be shifted of a certain amount left or right:
        - \ref SHIFT_LEFT   -> left
        - \ref SHIFT_RIGHT  -> right
    The shifting is performed \em before writing to the hardware during a write
    operation and \em after receiving the data from the hardware during a read
    operation.

    The function performs both read and write operations depending on the
    content of the variable write:
        - \ref SERIAL_READ  -> read opeation
        - \ref SERIAL_WRITE -> write operation

    \param command      This is the command word that we want to send out to the
                        serial mux board. This is dependent on the receiving
                        hardware and it will perform a well determined opeation.
    \param reg          If it is a write operation it will contain the data we
                        want to send to the remote hardware.\n
                        If it is a read operation it will contain the data read
                        back from the remote hardware.
    \param regSize      Is the size (in bit) of the data to be sent or read.
    \param shiftAmount  This is the shift amount in bits to apply to the data.
    \param shiftDir     This is the direction of the shift.
    \param write        This specify if it is read or a write operation.

    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int serialAccess(unsigned int command,
                 int *reg,
                 unsigned char regSize,
                 unsigned char shiftAmount,
                 unsigned char shiftDir,
                 unsigned char write){

    /* Intermediate data buffer
       An intermediate buffer variable is defined. This is going to be used
       to implement all the shifting needed before writing the data and after
       receiving the response from the hardware. */
    long long intermediateBuffer;

    /* Check that the command word size is ok */
    if(command > COMMAND_WORD_SIZE){
        storeError(ERR_SERIAL_INTERFACE, ERC_MODULE_RANGE); //Command out of range
        return ERROR;
    }

    /* Figure out the port on the serial mux board.
       Every cartridge has two port, the bias and the lo. If a cartridge is
       addressed then the port is given by the cartrdige number multiplied by a
       factor of two plus the index to the cartridge subsystem (1 -> Bias,
       0 -> Lo).
       On the other end if any other module is addressed, the port is offsetted
       respect to the addressed module by the number of cartridges given their
       doublefolded nature. */
    if(currentModule < CARTRIDGES_NUMBER){
        frame.
         port = 2*currentModule+(1-currentCartridgeSubsystem);
    } else {
        frame.
         port = CARTRIDGES_NUMBER+currentModule;
    }


    /* Store the command in the outgoing frame */
    frame.
     command=command;

    /* Store the size of the register */
    frame.
     dataLength=regSize;

    /* Perform differently if read or write */
    if(write==SERIAL_WRITE){ // If it's a WRITE operation
        /* Copy the data to the intermediate buffer. */
        memcpy(&intermediateBuffer,
               reg,
               sizeof(intermediateBuffer));

        /* If some shifting was required, it is performed before writing the
           data to the hardware. */
        if(shiftAmount){
            if(shiftDir==SHIFT_RIGHT){ // 0 -> Left, 1 -> Right
                intermediateBuffer = intermediateBuffer>>shiftAmount;
            } else {
                intermediateBuffer = intermediateBuffer<<shiftAmount;
            }
        }

        /* Store the register in the frame. Store 3 words, even if the register
           is smaller, it doesn't matter since the variable regSize is going to
           take care of the actual size. */
        memcpy(frame.
                data,
               &intermediateBuffer,
               FRAME_DATA_LENGTH_BYTES);

        /* Call the hardware writing function */
        if(writeMux()==ERROR){
            return ERROR;
        }
    } else { // If it's a READ operation
        /* Call the hardware reading funtion */
        if(readMux()==ERROR){
            return ERROR;
        }
        /* Copy the data to the intermediate buffer. */
        memcpy(&intermediateBuffer,
               frame.
                data,
               sizeof(intermediateBuffer));

        /* If some shifting was required, it is performed after reading the
           data from the hardware. */
        if(shiftAmount){
            if(shiftDir==SHIFT_RIGHT){ // 0 -> Left, 1 -> Right
                intermediateBuffer = intermediateBuffer>>shiftAmount;
            } else {
                intermediateBuffer = intermediateBuffer<<shiftAmount;
            }
        }
        /* Store the data from the frame into the register. This time the size
           does matter since the register has a well define size. */
        memcpy(reg,
               &intermediateBuffer,
               1+(unsigned char)(regSize/FRAME_DATA_UNIT_SIZE));
    }

    return NO_ERROR;
}

