/*! \file   globalDefinitions.c
    \brief  Global functions

    <b> File informations: </b><br>
    Created: 2004/08/24 16:16:14 by avaccari

    <b> CVS informations: </b><br>
    \$Id: globalDefinitions.c,v 1.18 2010/11/02 14:36:29 avaccari Exp $

    This file contains general use functions and variables. */

/* Includes */
#define __STDC_WANT_LIB_EXT1__  1   // Use the Safer C Library extension
#include <stdio.h>                  /* sprintf_s */
#include <stdlib.h>                 /* atol */
#include <string.h>                 /* strstr, strlen */
#include <math.h>                   /* pow */

#include "globalDefinitions.h"
#include "can.h"
#include "error.h"
#include "debug.h"

/* Globals */
/* Externs */
CONVERSION convert;   /*!< This union allows to perform quick and easy
                           conversion from the incoming CAN payload (unsigned
                           unsigned char[4]) to a float to be used in the program. */

/* Check range */
/*! This function checks if the parameter \p test is within the provided ranges.
    The check is done in the following range: [low,high] (inclusive for not-math
    majors).
    \param  low     a float
    \param  test    a float
    \param  high    a float
    \return
        - \ref ABOVE_RANGE  -> Above range
        - \ref IN_RANGE     -> In range
        - \ref BELOW_RANGE  -> Below Range */
unsigned char checkRange(float low,
                         float test,
                         float high){
    if(test<low){
        return BELOW_RANGE;
    }
    if(test>high){
        return ABOVE_RANGE;
    }
    return IN_RANGE;
}

/* Change endianicity for floats and longs */
/*! This function inverts the order of the 4 bytes IEEE float representation
    pointed at by \p *source and stores it into the location pointed at by
    \p *destination
    \param  destination a *unsigned char
    \param  source      a *unsigned char */
void changeEndian(unsigned char *destination,
                  unsigned char *source){
    destination[3]=source[0];
    destination[2]=source[1];
    destination[1]=source[2];
    destination[0]=source[3];
}

/* Change endianicity for unsigned ints */
/*! This function inverts the order of each of the two bytes in the array of
    ints pointed at by \p *source and stores it into the location pointed at by
    \p *destination
    \param  destination a *unsigned char
    \param  source      a *unsigned char */
void changeEndianInt(unsigned char *destination,
                     unsigned char *source){
    destination[3]=source[2];
    destination[2]=source[3];
    destination[1]=source[0];
    destination[0]=source[1];
}

/* Build string */
/*! This function builds a file name based on the parameters provided.
    \param  prefix      If provided will be prepended as first string
                        if NULL is passed, then no prefix will be added
    \param  number      A number to append to the prefix
    \param  suffix      If provided it will be appended after the string
                        If NULL is passed, then no suffix will be added

    \note   This function doesn't perform any check on the provided data to
            speed up the execution. */
char *buildString(unsigned char *prefix,
                  unsigned char number,
                  unsigned char *suffix){
    static unsigned char string[MAX_STRING_SIZE];

    if(suffix==NULL){
        if(prefix==NULL){
            sprintf_s(string,
                      sizeof(string),
                      "%d",
                      number);
        } else {
            sprintf_s(string,
                      sizeof(string),
                      "%s%d",
                      prefix,
                      number);
        }
    } else {
        if(prefix==NULL){
            sprintf_s(string,
                      sizeof(string),
                      "%d%s",
                      number,
                      suffix);
        } else {
            sprintf_s(string,
                      sizeof(string),
                      "%s%d%s",
                      prefix,
                      number,
                      suffix);
        }
    }

    return string;
}

/* Bogofunction */
/*! This function is used when an already established monitor/control point is
    removed from the hardware. It is used as a placeholder in case something
    new will be connected in the hardware. */
void bogoFunction(void) {

    #ifdef DEBUG
        printf("Bogo Function!\n");
    #endif /* DEBUG */

    /* If control message, ignore it. */
    if(CAN_SIZE){ // If control (size!=0)
        return;
    }

    /* If monitor */
    CAN_SIZE=0;
    CAN_STATUS=HARDW_UPD_WARN;

    return;
}

/* htol */
/*! This function allow conversion from hexadecimal string to long int. */
long htol(char *hex){

    unsigned char len,bas,cnt,val;
	long longInt=0;

	if(strstr(hex,
              "0x")){
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

