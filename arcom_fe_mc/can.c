/*! \file   can.c
    \brief  CAN functions
    \todo
                - Find a more clever way to handle the monitor messages on
                  control addresses. The static array is too big.
                - Should the firmware send back a well define message in case
                  of error while in the monitor rountines?
                - Make sure that the message is either cleared or set to a known
                  value (since is a global) otherwise you'll end up sending back
                  the previous payload with the wrong size if anything doesn't
                  work out as it should!
                - Find a clever way to store the last control request. The
                  current way needs a huge variable.
                - Drop the ADD-2 part of the FPGA revision numbering once all
                  the FPGA versions in the field are consisten

    <b> File informations: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: can.c,v 1.46 2009/09/22 14:46:10 avaccari Exp $

    This file contains the functions necessary to deal with the received CAN
    messages. */

/* Includes */
#include <conio.h>      /* inpw */
#include <string.h>     /* memcpy */
#include <stdio.h>      /* printf */

#include "ppComm.h"
#include "version.h"
#include "can.h"
#include "error.h"
#include "pegasus.h"
#include "frontend.h"
#include "main.h"
#include "debug.h"
#include "serialMux.h"
#include "console.h"
#include "owb.h"
#include "globalOperations.h"
#include "globalDefinitions.h"

/* Globals */
/* Externs */
volatile unsigned char newCANMsg=0; /*!< This variable is a semaphore which will
                                         notify the entire program of the arrival
                                         of a new CAN message. It is cleared
                                         once the message has been dealt with. */
CAN_MESSAGE CANMessage;             /*!< This variable hold the latest message
                                         received. */
unsigned char    currentModule=0;   /*!< This variable stores the current module
                                         information. This is the front end item
                                         the request are currently directed to. */
unsigned char    currentClass=0;    /*!< This variable stores the current class
                                         information. This is a specifier of the
                                         type of message: monitor, control or
                                         special that has been received. */

/* Statics */
/* During initialization only special messages are allowed. To minimize the
   time spend to check if the software is initializing, we use a trick where the
   function pointer is shifted by 3 during initialization. This saves time
   during normal execution. */
static HANDLER      classesHandler[CLASSES_NUMBER]={standardRCAsHandler,
                                                    standardRCAsHandler,
                                                    specialRCAsHandler}; // The classes handler array is initialized.
static HANDLER      modulesHandler[MODULES_NUMBER]={cartridgeHandler,   // Cartridge 0 -> Band 1
                                                    cartridgeHandler,   // Cartridge 1 -> Band 2
                                                    cartridgeHandler,   // Cartridge 2 -> Band 3
                                                    cartridgeHandler,   // Cartridge 3 -> Band 4
                                                    cartridgeHandler,   // Cartridge 4 -> Band 5
                                                    cartridgeHandler,   // Cartridge 5 -> Band 6
                                                    cartridgeHandler,   // Cartridge 6 -> Band 7
                                                    cartridgeHandler,   // Cartridge 7 -> Band 8
                                                    cartridgeHandler,   // Cartridge 8 -> Band 9
                                                    cartridgeHandler,   // Cartridge 9 -> Band 10
                                                    powerDistributionHandler,
                                                    ifSwitchHandler,
                                                    cryostatHandler,
                                                    lprHandler}; // The modules handler array is initialized





/*! This function handles the incoming CAN messages.

    The message is recomposed in the \ref CANMessage variable using the data
    in the \ref PPRxBuffer which contains all the relevant informations about
    the newly received message.

    After this is done, the function will select the appropriate action to
    perform according to the RCA and the direction (monitor/control) of the
    message.

    If it is a monitor message then after the message has been dealt with, the
    \ref PPTxBuffer will be compose using information now available in the
    \ref CANMessage variable and then send back to the AMBSI1 board calling the
    \ref PPWrite function. */
void CANMessageHandler(void){

    receiveCANMessage(); // Build the CAN message from the incoming data

    /* Redirect to the correct class handler depending on the RCA */
    currentClass=(CAN_ADDRESS&CLASSES_RCA_MASK)>>CLASSES_MASK_SHIFT;
    /* Check if the addressed class exist */
    if(currentClass>=CLASSES_NUMBER){
        storeError(ERR_CAN,
                   0x01); // Error 0x01 -> Required class outside allowed range
        newCANMsg=0; // Clear the new message flag
        PPClear(); // Clear the interrupt
        return;
    }
    /* If in range call the function and let the handler figure out if the
       receiver is outfitted with the particular device addressed.
       Adding the initializing status variable allows to use different pointers
       while in intialization mode respect to the standard operation. */
    (classesHandler[currentClass])(); // Call the appropriate handler

    /* Clear the new message flag */
    newCANMsg=0;

    /* After handling the message, the firmware is ready to receive more
       messages. The parallel port IRQ is cleared. */
    PPClear();

    return;


}

/* Standard message handler. */
static void standardRCAsHandler(void){

    if(CAN_SIZE == CAN_MONITOR){ // If it is a monitor message
        #ifdef DEBUG_CAN
            if(currentClass){
                printf("Monitor message on control message RCA (currentClass: %d)\n",
                        currentClass);
            } else {
                printf("Monitor message on monitor message RCA (currentClass: %d)\n",
                        currentClass);
            }
        #endif /* DEBUG_CAN */

        /* Store the NO_ERROR default status */
        CAN_STATUS = NO_ERROR;

        /* Check if the addressed module exist */
        currentModule=(CAN_ADDRESS&MODULES_RCA_MASK)>>MODULES_MASK_SHIFT;

        /* If it doesn't exist, return the error, otherwise call the correct
           handler */
        if(currentModule>=MODULES_NUMBER){
            storeError(ERR_CAN,
                       0x02); // Error 0x02 -> Required module outside allowed range
            CAN_STATUS = HARDW_RNG_ERR; // Notify incoming CAN message of the error
        } else {
            /* Redirect to the correct module handler depending on the RCA */
            (modulesHandler[currentModule])(); // Call the appropriate module handler
        }

        /* Since it was a monitor request, then the reply message was assembled
           by the previous call, send the message! */
        sendCANMessage();

        return;
    }

    /* If it is a control message... */
    /* There is no way to store an error to a control message addressed to a
       non existing RCA. */
    #ifdef DEBUG_CAN
        if(currentClass){
            printf("Control message on control message RCA (currentClass: %d)\n",
                    currentClass);
        } else {
            printf("Control message on monitor message RCA (currentClass: %d)\n",
                    currentClass);
        }
    #endif /* DEBUG_CAN */

    if(currentClass == 0){ // If it is on a monitor RCA
        storeError(ERR_CAN,
                   0x04); // Error 0x04 -> Control RCA out of range
        return;
    }

    /* Check if the addressed module exist */
    currentModule=(CAN_ADDRESS&MODULES_RCA_MASK)>>MODULES_MASK_SHIFT;
    if(currentModule>=MODULES_NUMBER){
        storeError(ERR_CAN,
                   0x02); // Error 0x02 -> Required module outside allowed range
        /* Since the main module is in error, all the following submodule
           addressing is considered in error as well. Because of this it is not
           possible to write an error in the status byte of the last control
           message received. */
        return;
    }

    /* Redirect to the correct module handler depending on the RCA. Any possible
       error happening after this point will be stored in the last control
       message status byte. This is not true for message directed towards non
       existing harware because we don't know with what hardware to associate
       the error. Since nothing is returned from a control message, we cannot
       return the error state as well. */
    (modulesHandler[currentModule])(); // Call the appropriate module handler

    /* It's a control message, so we're done. */
    return;
}




/* Special messages handler */
static void specialRCAsHandler(void){

    /* A static to take care of the ESNs monitoring */
    static unsigned char device=0;

    #ifdef DEBUG_CAN
        printf("Special message:\n");
    #endif /* DEBUG_CAN */

    /* Set the status to the default */
    CAN_STATUS=NO_ERROR;

    switch(CAN_SIZE){ // If size = 0 -> message = monitor, everyting else -> message = control
        case CAN_MONITOR: // If special message is monitor
            #ifdef DEBUG_CAN
                printf(" Monitor\n");
            #endif /* DEBUG_CAN */
            switch(CAN_ADDRESS){
                case GET_ARCOM_VERSION_INFO:  // 0x20002 -> Return info about the version of the firmware
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_ARCOM_VERSION_INFO\n\n",
                               GET_ARCOM_VERSION_INFO);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(0)=VERSION_MAJOR;
                    CAN_DATA(1)=VERSION_MINOR;
                    CAN_DATA(2)=VERSION_PATCH;
                    CAN_SIZE=3;
                    break;
                case GET_SPECIAL_MONITOR_RCAS: // 0x20003 -> Return the special monitor RCAs informations
                    /* Since this is a special case, the base address returned
                       to the AMBSI1 is not the actual base address but is the
                       first addressable CAN monitor message that will return
                       informations about the ARCOM board.

                       Addresses 0x20000 and 0x20001 are already registered as
                       functions callbacks in the AMBSI1 firmware.
                       If we allow them to be registered again this would cause
                       problems at run time. */
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_SPECIAL_MONITOR_RCAS\n\n",
                               GET_SPECIAL_MONITOR_RCAS);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(7)=(unsigned char)(LAST_SPECIAL_MONITOR_RCA>>24);
                    CAN_DATA(6)=(unsigned char)(LAST_SPECIAL_MONITOR_RCA>>16);
                    CAN_DATA(5)=(unsigned char)(LAST_SPECIAL_MONITOR_RCA>>8);
                    CAN_DATA(4)=(unsigned char)(LAST_SPECIAL_MONITOR_RCA);
                    CAN_DATA(3)=(unsigned char)(GET_ARCOM_VERSION_INFO>>24);
                    CAN_DATA(2)=(unsigned char)(GET_ARCOM_VERSION_INFO>>16);
                    CAN_DATA(1)=(unsigned char)(GET_ARCOM_VERSION_INFO>>8);
                    CAN_DATA(0)=(unsigned char)(GET_ARCOM_VERSION_INFO);
                    CAN_SIZE=CAN_FULL_SIZE;
                    break;
                case GET_SPECIAL_CONTROL_RCAS: // 0x20004 -> Return the special control RCAs informations
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_SPECIAL_CONTROL_RCAS\n\n",
                               GET_SPECIAL_CONTROL_RCAS);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(7)=(unsigned char)(LAST_SPECIAL_CONTROL_RCA>>24);
                    CAN_DATA(6)=(unsigned char)(LAST_SPECIAL_CONTROL_RCA>>16);
                    CAN_DATA(5)=(unsigned char)(LAST_SPECIAL_CONTROL_RCA>>8);
                    CAN_DATA(4)=(unsigned char)(LAST_SPECIAL_CONTROL_RCA);
                    CAN_DATA(3)=(unsigned char)((BASE_SPECIAL_CONTROL_RCA)>>24);
                    CAN_DATA(2)=(unsigned char)((BASE_SPECIAL_CONTROL_RCA)>>16);
                    CAN_DATA(1)=(unsigned char)((BASE_SPECIAL_CONTROL_RCA)>>8);
                    CAN_DATA(0)=(unsigned char)(BASE_SPECIAL_CONTROL_RCA);
                    CAN_SIZE=CAN_FULL_SIZE;
                    break;
                case GET_MONITOR_RCAS: // 0x20005 -> Return the monitor RCAs informations
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_MONITOR_RCAS\n\n",
                               GET_MONITOR_RCAS);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(7)=(unsigned char)(LAST_MONITOR_RCA>>24);
                    CAN_DATA(6)=(unsigned char)(LAST_MONITOR_RCA>>16);
                    CAN_DATA(5)=(unsigned char)(LAST_MONITOR_RCA>>8);
                    CAN_DATA(4)=(unsigned char)(LAST_MONITOR_RCA);
                    CAN_DATA(3)=(unsigned char)((BASE_MONITOR_RCA)>>24);
                    CAN_DATA(2)=(unsigned char)((BASE_MONITOR_RCA)>>16);
                    CAN_DATA(1)=(unsigned char)((BASE_MONITOR_RCA)>>8);
                    CAN_DATA(0)=(unsigned char)(BASE_MONITOR_RCA);
                    CAN_SIZE=CAN_FULL_SIZE;
                    break;
                case GET_CONTROL_RCAS: // 0x20006 -> Return the control RCAs informations
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_CONTROL_RCAS\n\n",
                               GET_CONTROL_RCAS);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(7)=(unsigned char)(LAST_CONTROL_RCA>>24);
                    CAN_DATA(6)=(unsigned char)(LAST_CONTROL_RCA>>16);
                    CAN_DATA(5)=(unsigned char)(LAST_CONTROL_RCA>>8);
                    CAN_DATA(4)=(unsigned char)(LAST_CONTROL_RCA);
                    CAN_DATA(3)=(unsigned char)((BASE_CONTROL_RCA)>>24);
                    CAN_DATA(2)=(unsigned char)((BASE_CONTROL_RCA)>>16);
                    CAN_DATA(1)=(unsigned char)((BASE_CONTROL_RCA)>>8);
                    CAN_DATA(0)=(unsigned char)(BASE_CONTROL_RCA);
                    CAN_SIZE=CAN_FULL_SIZE;
                    break;
                case GET_PPCOMM_TIME: // 0x20007 -> Get parallel port communication time
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_PPCOMM_TIME\n\n",
                               GET_PPCOMM_TIME);
                    #endif /* DEBUG_CAN */
                    /* This message doesn't perform any operation. It is intended for debug purposes only.
                       When called it will fill up a message payload with 8 0xFF and return. This will give
                       an estimate on the longest time necessary to acknowledge and respond to the largest
                       monitor request without performing any operation: it is a measure of the longest
                       communication time between the ARCOM and the AMBSI1 board */
                    CAN_DATA(7)=0x00;
                    CAN_DATA(6)=0x00;
                    CAN_DATA(5)=0x00;
                    CAN_DATA(4)=0x00;
                    CAN_DATA(3)=0x00;
                    CAN_DATA(2)=0x00;
                    CAN_DATA(1)=0x00;
                    CAN_DATA(0)=0x00;
                    CAN_SIZE=CAN_FULL_SIZE;
                    break;
                case GET_FPGA_VERSION_INFO: // 0x20008 -> Get FPGA firmware info
                    {
                        /* A union to deal with the FPGA version */
                        union {
                            unsigned int        integer;
                            struct {
                                unsigned int patch  :8;
                                unsigned int minor  :4;
                                unsigned int major  :4;
                            }                   bitField;
                        } fpgaVersion;

                        #ifdef DEBUG_CAN
                            printf("  0x%lX->GET_FPGA_VERSION_INFO\n\n",
                                   GET_FPGA_VERSION_INFO);
                        #endif /* DEBUG_CAN */

                        /* The ADD-4 part can be dropped once all the FPGA versions
                           are consistent. */
                        if(inpw(MUX_FPGA_RDY_ADD)==FPGA_READY){
                            fpgaVersion.
                             integer=inpw(MUX_FPGA_VERSION);
                        } else {
                            fpgaVersion.
                             integer=inpw(MUX_FPGA_VERSION-2);
                        }

                        CAN_DATA(0)=fpgaVersion.
                                     bitField.
                                      major;
                        CAN_DATA(1)=fpgaVersion.
                                     bitField.
                                      minor;
                        CAN_DATA(2)=fpgaVersion.
                                     bitField.
                                      patch;
                        CAN_SIZE=CAN_REV_SIZE;
                    }
                    break;
                case GET_CONSOLE_ENABLE: // 0x20009 -> Enables/Disables the console
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_CONSOLE_ENABLE\n\n",
                               GET_CONSOLE_ENABLE);
                    #endif /* DEBUG_CAN */
                    CAN_BYTE=consoleEnable;
                    CAN_SIZE=CAN_BOOLEAN_SIZE;
                    break;
                case GET_ESNS_FOUND: // 0x2000A -> Returns the number of ESNs found
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_ESNS_FOUND\n\n",
                               GET_ESNS_FOUND);
                    #endif /* DEBUG_CAN */
                    device=0; // Reset the device index to the beginning of the list
                    CAN_BYTE=esnDevicesFound;
                    CAN_SIZE=CAN_BOOLEAN_SIZE;
                    break;
                case GET_ESNS: // 0x2000B -> Return the list of ESNs found
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_ESNS\n\n",
                               GET_ESNS);
                    #endif /* DEBUG_CAN */
                    /* If no devices were found return error */
                    if(esnDevicesFound==0){
                        CAN_DATA(7)=0xFF;
                        CAN_DATA(6)=0xFF;
                        CAN_DATA(5)=0xFF;
                        CAN_DATA(4)=0xFF;
                        CAN_DATA(3)=0xFF;
                        CAN_DATA(2)=0xFF;
                        CAN_DATA(1)=0xFF;
                        CAN_DATA(0)=0xFF;
                        CAN_SIZE=CAN_FULL_SIZE;
                        break;
                    }

                    /* If last found device was already reported, return zero
                       and reset the count. */
                    if(device==esnDevicesFound){
                        CAN_DATA(7)=0;
                        CAN_DATA(6)=0;
                        CAN_DATA(5)=0;
                        CAN_DATA(4)=0;
                        CAN_DATA(3)=0;
                        CAN_DATA(2)=0;
                        CAN_DATA(1)=0;
                        CAN_DATA(0)=0;
                        CAN_SIZE=CAN_FULL_SIZE;
                        device=0;
                        break;
                    }

                    /* Return the next available ESN */
                    CAN_DATA(7)=ESNS[device][7];
                    CAN_DATA(6)=ESNS[device][6];
                    CAN_DATA(5)=ESNS[device][5];
                    CAN_DATA(4)=ESNS[device][4];
                    CAN_DATA(3)=ESNS[device][3];
                    CAN_DATA(2)=ESNS[device][2];
                    CAN_DATA(1)=ESNS[device][1];
                    CAN_DATA(0)=ESNS[device][0];
                    CAN_SIZE=CAN_FULL_SIZE;
                    device++;
                    break;
                case GET_ERRORS_NUMBER: // 0x2000C -> Returns the number of unread errors
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_ERROR_NUMBER\n\n",
                               GET_ERRORS_NUMBER);
                    #endif // DEBUG_CAN
                    CAN_SIZE=CAN_BYTE_SIZE;
                    CAN_BYTE=(errorNewest>=errorOldest)?errorNewest-errorOldest:
                                                        ERROR_HISTORY_LENGTH-(errorNewest-errorOldest);
                    break;
                case GET_NEXT_ERROR: // 0x2000D -> Returns the next unread error
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_NEXT_ERROR\n\n",
                               GET_NEXT_ERROR);
                    #endif // DEBUG_CAN
                    CAN_SIZE=CAN_INT_SIZE;
                    if(errorNewest==errorOldest){
                        CONV_UINT(0)=0xFFFF;
                    } else {
                        CONV_UINT(0)=errorHistory[errorOldest];
                        errorOldest++;
                    }
                    CAN_DATA(0)=CONV_CHR(1);
                    CAN_DATA(1)=CONV_CHR(0);
                    break;
                case GET_FE_MODE: // 0x2000E -> Returns the FE operating mode
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->GET_FE_MODE\n\n",
                               GET_FE_MODE);
                    #endif /* DEBUG_CAN */
                    CAN_DATA(0)=frontend.
                                 mode[CURRENT_VALUE];
                    CAN_SIZE=CAN_BYTE;
                    break;
                default: // This will take care also of all the monitor request on special CAN control RCAs
                    #ifdef DEBUG_CAN
                        printf("  Out of Range!\n\n");
                    #endif /* DEBUG_CAN */
                    storeError(ERR_CAN,
                               0x05); // Error 0x05 -> Special Monitor RCA out of range
                    CAN_STATUS = MON_CAN_RNG; // Message out of range
                    break;
            }
            sendCANMessage();
            break;
        default: // If special message is control
            #ifdef DEBUG_CAN
                printf(" Control\n");
            #endif /* DEBUG_CAN */
            switch(CAN_ADDRESS){
                case SET_EXIT_PROGRAM: // 0x21000 -> Cause the entire program to come to a "graceful" end
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->SET_EXIT_PROGRAM\n\n",
                               SET_EXIT_PROGRAM);
                    #endif /* DEBUG_CAN */
                    stop = 1;
                    break;
                case SET_REBOOT: // 0x21001 -> Reboots the ARCOM board
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->SET_REBOOT\n\n",
                               SET_REBOOT);
                    #endif /* DEBUG_CAN */
                    reboot();
                    break;
                case SET_CONSOLE_ENABLE: // 0x21009 -> Enables/Disables the console
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->SET_CONSOLE\n\n",
                               SET_CONSOLE_ENABLE);
                    #endif /* DEBUG_CAN */
                    consoleEnable=(CAN_BYTE==DISABLE)?DISABLE:
                                                      ENABLE;
                    break;
/* This should be implemented right now, only operation mode is allowed
                case SET_FE_MODE: // 0x2100E -> Set the FE operating mode
                    #ifdef DEBUG_CAN
                        printf("  0x%lX->SET_FE_MODE\n\n",
                               SET_FE_MODE);
                    #endif // DEBUG_CAN
// The range should be checked according to the usual scheme before setting.
                    frontend.
                     mode[CURRENT_VALUE]=CAN_DATA(0);
                    break;
*/
                default:
                    #ifdef DEBUG_CAN
                        printf("  Out of Range!\n\n");
                    #endif /* DEBUG_CAN */
                    storeError(ERR_CAN,
                               0x06); // Error 0x06 -> Special Control RCA out of range
                    break;
            }
            break;
        return;
    }
}



/* A function to build CANMessage with the incoming data */
static void receiveCANMessage(void){
    #ifdef DEBUG_CAN
        unsigned char cnt;
    #endif /* DEBUG_CAN */

    if(newCANMsg==TRUE){
        #ifdef DEBUG_CAN
            printf("Input PP Buffer: ");
            for(cnt=0;
                cnt<CAN_RX_MESSAGE_SIZE;
                cnt++){
                printf("%#x ",
                       PPRxBuffer[cnt]);
            }
            printf("\n");
        #endif /* DEBUG_CAN */

        memcpy(&CAN_MSG,
               PPRxBuffer,
               CAN_RX_HEADER_SIZE+PPRxBuffer[4]);
    }

    #ifdef DEBUG_CAN_FAST
        printf("\n%#lx",
               CAN_ADDRESS);
    #endif /* DEBUG_CAN_FAST */

    #ifdef DEBUG_CAN
        printf("RCA: %#lx\n",
               CAN_ADDRESS);
        printf("size: %d\nData: ",
               CAN_SIZE);
        for(cnt=0;
            cnt<CAN_SIZE;
            cnt++){
            printf("%#x ",
                   CAN_DATA(cnt));
        }
        printf("\n");
    #endif /* DEBUG_CAN */
}



/* A function to build the outgoing message from CANMessage */
static void sendCANMessage(void){
    unsigned char cnt;

    /* If there is space in the message then store the status byte as first
       after the payload and increase the size of the message by 1 */
    if(CAN_SIZE<CAN_TX_MAX_PAYLOAD_SIZE){
        CAN_DATA(CAN_SIZE++)=CAN_STATUS;
    }


    #ifdef DEBUG_CAN
        printf("\nSending...\n");
        printf("RCA: %#lx\n",
               CAN_ADDRESS);
        printf("size: %d\nData: ",
               CAN_SIZE);
        for(cnt=0;
            cnt<CAN_SIZE;
            cnt++){
            printf("%#x ",
                   CAN_DATA(cnt));
        }
        printf("\n");
    #endif /* DEBUG_CAN */

    memcpy(PPTxBuffer,
           CAN_DATA_ADD,
           CAN_SIZE);

    #ifdef DEBUG_CAN_FAST
        printf("m");
    #endif /* DEBUG_CAN_FAST */

    #ifdef DEBUG_CAN
        printf("Output PP Buffer: ");
        for(cnt=0;
            cnt<CAN_TX_MAX_PAYLOAD_SIZE;
            cnt++){
            printf("%#x ",
                   PPTxBuffer[cnt]);
        }
        printf("\n\n");
    #endif /* DEBUG_CAN */

    /* If it was a request coming from the CAN bus, send back the data. */
    if(newCANMsg==TRUE){
        PPWrite(CAN_SIZE);
        return;
    }

    /* If it was coming from console, deal with the console */
    switch(CAN_SIZE){
        case CAN_FLOAT_SIZE+1:
            if(currentClass==CONTROL_CLASS){ // If it was a monitor on a control RCA
                changeEndian(CONV_CHR_ADD,
                             CAN_DATA_ADD);
            }
            printf("%f",
                   CONV_FLOAT);
            break;
        case CAN_INT_SIZE+1:
            if(currentClass==CONTROL_CLASS){ // If it was a monitor on a control RCA
                changeEndianInt(CONV_CHR_ADD,
                                CAN_DATA_ADD);
            }
            printf("%d",
                   CONV_UINT(0));
            break;
        case CAN_BYTE_SIZE+1:
            printf("%d",
                   CAN_BYTE);
            break;
        default:
            for(cnt=0;
                cnt<CAN_SIZE;
                cnt++){
                printf("%#x ",
                       CAN_DATA(cnt));
            }
            break;
    }

    printf(" (status: %#x)\n",
           CAN_STATUS);



}





