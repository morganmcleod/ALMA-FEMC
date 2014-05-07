/*! \file   console.c
    \brief  Console

    <b> File informations: </b><br>
    Created: 2007/05/22 11:31:31 by avaccari

    <b> CVS informations: </b><br>
    \$Id: console.c,v 1.8 2008/02/28 22:15:05 avaccari Exp $

    This files contains all the functions necessary to handle console accesses
    to the software. */


/* Includes */
#include <conio.h>      /* getch, kbhit */
#include <string.h>     /* strcmp */
#include <stdio.h>      /* printf */
#include <stdlib.h>     /* atol */
#include <math.h>       /* pow */

#include "console.h"
#include "can.h"
#include "main.h"
#include "globalDefinitions.h"
#include "debug.h"
#include "version.h"

/* Globals */
/* Externs */
#ifdef CONSOLE
    unsigned char consoleEnable=ENABLE;
#else
    unsigned char consoleEnable=DISABLE;
#endif /* DEVELOPMENT */

/* Static */
static unsigned char buffer[BUFFER_SIZE];
static unsigned char bufferIndex=0;


/* Console */
/*! This function handles the console inputs. */
void console(void){
    unsigned char key;

    /* Check if the console is enable */
    if(consoleEnable==DISABLE){
        return;
    }

    /* Echo data typed to console and store into circular buffer */
    if(kbhit()){
        key=getch();
        putch(key);
        if(key==DELETE_KEY){
            if(bufferIndex>0){
                bufferIndex--;
                buffer[bufferIndex]=NULL;
            }
            return;
        }
        buffer[bufferIndex]=key;
        buffer[bufferIndex+1]=NULL;
        bufferIndex++;

        if(key==ENTER_KEY){
            buffer[--bufferIndex]=NULL;
            parseBuffer();
            bufferIndex=0;
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
                        CAN_UINT = (unsigned int)atoi(token);
                        CAN_DATA(0)=convert.
                                     chr[3];
                        CAN_DATA(1)=convert.
                                     chr[2];
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
                        CAN_FLOAT = atof(token);
                        CAN_DATA(0)=convert.
                                     chr[3];
                        CAN_DATA(1)=convert.
                                     chr[2];
                        CAN_DATA(2)=convert.
                                     chr[1];
                        CAN_DATA(3)=convert.
                                     chr[0];
                        CAN_SIZE = CAN_FLOAT_SIZE;
                        // Disable interrupts (if necessary)
                        CANMessageHandler();
                        // Enable interrupts
                    }
                }
            }
        }
    } else { // Anything else print help
        printf("Console help\n");
        printf(" q -> quit\n");
        printf(" r -> restart\n");
        printf(" d -> disable console\n");
        printf(" i -> display version information\n");
        printf(" m RCA -> monitor the specified address\n");
        printf("          RCA is the Relative CAN Address.\n");
        printf("              It can be in decimal or exadecimal (0x...) format\n");
        printf("              For a list of the RCAs check:\n");
        printf("              - ALMA-40.00.00.00-75.35.25.00-X-ICD\n");
        printf(" c RCA q data -> control the specified address\n");
        printf("          RCA is the Relative CAN Address.\n");
        printf("              It can be in decimal or exadecimal (0x...) format\n");
        printf("              For a list of the RCAs check:\n");
        printf("              - ALMA-40.00.00.00-75.35.25.00-X-ICD\n");
        printf("          q is the qualifier for the payload:\n");
        printf("            b for a byte or a boolean\n");
        printf("            i for an unsigned integer\n");
        printf("            f for a float\n");
        printf("          data is the payload in the format specified by the qualifier\n");
        printf("              For a list of the RCAs check:\n");
        printf("              - ALMA-40.00.00.00-75.35.25.00-X-ICD\n");
    }
}

/* Alow to use exadecimal in the command line */
static long htol(char *hex){

    unsigned char len,bas,cnt,val;
	long longInt=0;

	if(strstr(hex,"0x")){
		len=strlen(hex);

		for(cnt=0;cnt<len;cnt++){

	    	val=hex[cnt];

		    if(((val>47)&&(val<58))||((val>64)&&(val<71))||((val>96)&&(val<103))){
				bas=val>64?(val>96?87:55):48;
				longInt+=(val-bas)*pow(16,len-cnt-1);
		    }
		}
	} else {
		longInt=atol(hex);
	}

	return longInt;
}

