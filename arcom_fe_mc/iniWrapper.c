/*! \file   iniWrapper.c
    \brief  INI file wrapper functions

    <b> File informations: </b><br>
    Created: 2006/06/29 16:43:33 by avaccari

    <b> CVS informations: </b><br>
    \$Id: iniWrapper.c,v 1.3 2007/08/27 21:08:41 avaccari Exp $

    This files contains all the functions necessary to handle the wrapper to
    the 3rd party INI file reader library. */

/* Includes */
#include <stdio.h>      /* printf */
#include <errno.h>      /* errno */

#include "iniWrapper.h"
#include "error.h"
#include "debug.h"

/* Read info from the configuration file */
/*! This function will read informations from the selected configuration file.
    \param  *fileName       This is the name of the configuration file to access
    \param  *sectionName    This is the name of the section to look for without
                            the bracket "[" or "]"
    \param  *searchVar      This is the variable describing the value to search
                            for
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something wrong happened */
int myReadCfg(const char *fileName,
              char *sectionName,
              CFG_STRUCT *searchVar,
              unsigned char expectedItems){

    /* A variable to deal with the return value */
    int returnValue=NO_ERROR;

    #ifdef DEBUG_INI
        printf("\n     Opening file: %s\n     Serching for section: %s\n     Key: %s\n",
               fileName,
               sectionName,
               searchVar->Name);
    #endif /* DEBUG_INI */



    /* Call the original function */
    returnValue=ReadCfg(fileName,
                        sectionName,
                        searchVar);

    /* Deal with the return value */
    switch(returnValue){
        case DATA_NOT_FOUND:
            storeError(ERR_INI,
                       0x01); // Error 0x01 -> The data wasn't found in the configuration file
            return ERROR;
            break;
        case FILE_OPEN_ERROR:
            storeError(ERR_INI,
                       0x02); // Error 0x02 -> Error opening the required file
            return ERROR;
            break;
        case FILE_ERROR:
            storeError(ERR_INI,
                       0x04); // Error 0x04 -> Error handling the required file
            return ERROR;
            break;
        case FILE_CLOSE_ERROR:
            storeError(ERR_INI,
                       0x05); // Error 0x05 -> Error closing the required file
            return ERROR;
            break;
        default:
            /* If no errors, check if the number of returned items is the same
               as the requested ones. */
            if(returnValue!=expectedItems){
                storeError(ERR_INI,
                           0x03); // Error 0x03 -> Number of returned items different from expected
                return ERROR;
            }
            return NO_ERROR;
            break;
    }

    return NO_ERROR;
}


