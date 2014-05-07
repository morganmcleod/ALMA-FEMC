/*! \file   console.c
    \brief  Console

    <b> File informations: </b><br>
    Created: 2007/05/22 11:31:31 by avaccari

    <b> CVS informations: </b><br>
    \$Id: console.c,v 1.14 2009/10/13 15:01:49 avaccari Exp $

    This files contains all the functions necessary to handle console accesses
    to the software. */


/* Includes */
#include <conio.h>      /* getch, kbhit, putch */
#include <string.h>     /* strcmp, memcpy, strcpy, strtok */
#include <stdio.h>      /* printf, flushall */
#include <stdlib.h>     /* atoi, atof */

#include "console.h"
#include "can.h"
#include "main.h"
#include "globalDefinitions.h"
#include "debug.h"
#include "version.h"
#include "async.h"

/* Globals */
/* Externs */
#ifdef CONSOLE
    unsigned char consoleEnable=ENABLE;
#else
    unsigned char consoleEnable=DISABLE;
#endif /* DEVELOPMENT */

/* Static */
static unsigned char buffer[BUFFER_SIZE];
static unsigned char lastBuffer[BUFFER_SIZE];
static unsigned char bufferIndex=0;
static unsigned char lastBufferIndex=0;


/* Console */
/*! This function handles the console inputs. */
void console(void){
    unsigned char key;
    unsigned char counter;

    /* Echo data typed to console and store into circular buffer */
    if(kbhit()){
        key=getch();
        putch(key);

        #ifdef DEBUG_CONSOLE
            printf("(%d@%d)",
                   key,
                   bufferIndex);
            flushall();
        #endif /* DEBUG_CONSOLE */

        switch(key){
            case DELETE_KEY:
                if(bufferIndex>0){
                    bufferIndex--;
                    buffer[bufferIndex]=NULL;
                }
                break;
            case APOSTROPHE_KEY:
                bufferIndex++;
                memcpy(buffer,
                       lastBuffer,
                       BUFFER_SIZE);
                for(counter=0;
                    counter<bufferIndex;
                    counter++){
                    putch(DELETE_KEY);
                }
                bufferIndex=lastBufferIndex;
                printf("%s",
                       buffer);
                flushall();
                break;
            case QUOTE_KEY:
                bufferIndex++;
                memcpy(buffer,
                       lastBuffer,
                       BUFFER_SIZE);
                for(counter=0;
                    counter<bufferIndex;
                    counter++){
                    putch(DELETE_KEY);
                }
                printf("%s",
                       buffer);
                parseBuffer();
                bufferIndex=0;
                buffer[0]=NULL;
                break;
            case ENTER_KEY:
                memcpy(lastBuffer,
                       buffer,
                       BUFFER_SIZE);
                lastBufferIndex=bufferIndex;
                parseBuffer();
                bufferIndex=0;
                buffer[0]=NULL;
                break;
            default:
                buffer[bufferIndex]=key;
                buffer[bufferIndex+1]=NULL;
                bufferIndex++;
                break;
        }
    }
}

/* Parse buffer */
static void parseBuffer(void){
    unsigned char *token;
    unsigned char localBuf[BUFFER_SIZE];

    /* Print a new line */
    printf("\n");

    /* Copy the buffer to a local buffer */
    strcpy(localBuf,
           buffer);

    /* Extract the first token */
    token=strtok(localBuf,
                 " \0");

    /* Check the first token for known commands */
    if(strcmp(token,
              "q")==0){         // 'q' -> Quit
        stop = 1;
    } else if (strcmp(token,
                      "r")==0){ // 'r' -> Restarts
        stop = 1;
        restart = 1;
    } else if (strcmp(token,
                      "a")==0){ // 'a' -> toggles async process on/off
        if(asyncState!=ASYNC_OFF){
            asyncState=ASYNC_OFF;
        } else {
            asyncState=ASYNC_ON;
        }
    } else if (strcmp(token,
                      "d")==0){ // 'd' -> Disables the console
        printf("Console disabled!\n");
        printf("To enable, send a 1 to RCA 0x%lX\n",
               SET_CONSOLE_ENABLE);
        consoleEnable = 0;
    } else if (strcmp(token,
                      "i")==0){ // 'i' -> Display version info
        displayVersion();
    } else if (strcmp(token,
                      "m")==0){ // 'm' -> Monitor
        CAN_SIZE=0;

        /* Get next token */
        token = strtok(NULL,
                       " \0");

        /* Extract the RCA from the token */
        if(token != NULL){
            CAN_ADDRESS = (unsigned long)htol(token);
            // Disable interrupts (if necessary)
            CANMessageHandler();
            // Enable interrupts
        }
    } else if(strcmp(token,
                     "c")==0){  // 'c' -> Control
        /* Get next token */
        token = strtok(NULL,
                       " \0");

        /* Extract the RCA from the token */
        if(token != NULL){
            CAN_ADDRESS = (unsigned long)htol(token);
            /* Get payload qualifier token */
            token = strtok(NULL,
                           " \0");

            /* Extract qualifier from the token */
            if(token != NULL){
                if(strcmp(token,
                          "b")==0){
                    /* Get next token */
                    token = strtok(NULL,
                                   " \0");

                    /* Extract the payload from the token */
                    if(token != NULL){
                        CAN_DATA(0) = (unsigned char)atoi(token);
                        CAN_SIZE = CAN_BYTE_SIZE;
                        // Disable interrupts (if necessary)
                        CANMessageHandler();
                        // Enable interrupts
                    }
                } else if(strcmp(token,
                                 "i")==0){
                    /* Get next token */
                    token = strtok(NULL,
                                   " \0");

                    /* Extract the payload from the token */
                    if(token != NULL){
                        CONV_UINT(0) = (unsigned int)atoi(token);
                        CAN_DATA(0)=CONV_CHR(1);
                        CAN_DATA(1)=CONV_CHR(0);
                        CAN_SIZE = CAN_INT_SIZE;
                        // Disable interrupts (if necessary)
                        CANMessageHandler();
                        // Enable interrupts
                    }
                } else if(strcmp(token,
                                 "f")==0){
                    /* Get next token */
                    token = strtok(NULL,
                                   " \0");

                    /* Extract the payload from the token */
                    if(token != NULL){
                        CONV_FLOAT = atof(token);
                        CAN_DATA(0)=CONV_CHR(3);
                        CAN_DATA(1)=CONV_CHR(2);
                        CAN_DATA(2)=CONV_CHR(1);
                        CAN_DATA(3)=CONV_CHR(0);
                        CAN_SIZE = CAN_FLOAT_SIZE;
                        // Disable interrupts (if necessary)
                        CANMessageHandler();
                        // Enable interrupts
                    }
                }
            }
        }
    } else { // Anything else print help
        displayVersion();
        printf("Console help\n");
        printf(" ' -> retypes last command\n");
        printf(" \" -> repeats last command\n");
        printf(" q<CR> -> quit\n");
        printf(" r<CR> -> restart\n");
        printf(" a<CR> -> enables/disables the async process (DEBUG only)\n");
        printf(" d<CR> -> disable console\n");
        printf(" i<CR> -> display version information\n");
        printf(" m RCA<CR> -> monitor the specified address\n");
        printf("           RCA is the Relative CAN Address.\n");
        printf("               It can be in decimal or exadecimal (0x...) format\n");
        printf("               For a list of the RCAs check:\n");
        printf("               - ALMA-40.00.00.00-75.35.25.00-X-ICD\n");
        printf("               - ALMA-40.04.03.03-002-X-DSN\n");
        printf(" c RCA q data <CR> -> control the specified address\n");
        printf("                   RCA is the Relative CAN Address.\n");
        printf("                       It can be in decimal or exadecimal (0x...) format\n");
        printf("                       For a list of the RCAs check:\n");
        printf("                       - ALMA-40.00.00.00-75.35.25.00-X-ICD\n");
        printf("                       - ALMA-40.04.03.03-002-X-DSN\n");
        printf("                   q is the qualifier for the payload:\n");
        printf("                     b for a byte or a boolean\n");
        printf("                     i for an unsigned integer\n");
        printf("                     f for a float\n");
        printf("                   data is the payload in the format specified by the qualifier\n");
    }
}


