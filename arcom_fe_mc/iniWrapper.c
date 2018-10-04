/*! \file   iniWrapper.c
    \brief  INI file wrapper functions

    <b> File informations: </b><br>
    Created: 2006/06/29 16:43:33 by avaccari

    <b> CVS informations: </b><br>
    \$Id: iniWrapper.c,v 1.4 2009/03/25 14:45:06 avaccari Exp $

    This files contains all the functions necessary to handle the wrapper to
    the 3rd party INI file reader library. */

/* Includes */
#include <stdio.h>      /* printf */
#include <errno.h>      /* errno */

#include "iniWrapper.h"
#include "error.h"
#include "debug.h"

/* Write info to the configuration file */
/*! This function will write informations to the selected configuration file.
        - If the section doesn't yet exist in the file, it will be added
        - If the variable doesn't yet exist in the file, it will be added
        - New variables are created at the end of existing sections, or on the
          last line before the first section name in the case where the section
          name is ""
        - New sections are created at the end of the file.
    \param *fileName        This is the name of the configuration file to access
    \param *sectionName     This is the name of the section to look for without
                            the bracket "[" or "]"
    \param *varWanted       This is a pointer to the name of the key to update
    \param *newData         This is a pointer to the new value to be stored
    \return
        - \ref NO_ERROR -> if no error occurred
        - \ref ERROR    -> if something went wrong */
extern int myWriteCfg(const char *fileName,
                      char *sectionName,
                      char *varWanted,
                      char *newData){
    #ifdef DEBUG_INI
        printf("\n     Opening file: %s\n     Serching for section: %s\n     Key: %s\n",
               fileName,
               sectionName,
               varWanted);
    #endif // DEBUG_INI

    if(UpdateCfg(fileName,
                 sectionName,
                 varWanted,
                 newData)==ERROR){
        storeError(ERR_INI, ERC_FLASH_ERROR); //Error updating the configuration file
        return ERROR;
    }

    return NO_ERROR;
}




/* Read info from the configuration file */
/*! This function will read informations from the selected configuration file.
    \param  *fileName       This is the name of the configuration file to access
    \param  *sectionName    This is the name of the section to look for without
                            the bracket "[" or "]"
    \param  *searchVar      This is the variable describing the value to search
                            for
    \return
        - \ref NO_ERROR         -> if no error occurred
        - \ref ERROR            -> if something wrong happened
        - \ref DATA_NOT_FOUND   -> if the sought item was not found
        - \ref FILE_OPEN_ERROR  -> if there was an error opening the file
        - \ref FILE_ERROR       -> if there was an error accessing the file
        - \ref FILE_CLOSE_ERROR -> if there was an error closing the file
        - \ref ITEMS_NO_ERROR   -> if the returned number of items found is not
                                   in agreement with the specified number */
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
            storeError(ERR_INI, ERC_FLASH_ERROR); //The data wasn't found in the configuration file
            return DATA_NOT_FOUND;
            break;
        case FILE_OPEN_ERROR:
            storeError(ERR_INI, ERC_FLASH_ERROR); //Error 0x02 -> Error opening the required file
            return FILE_OPEN_ERROR;
            break;
        case FILE_ERROR:
            storeError(ERR_INI, ERC_FLASH_ERROR); //Error handling the required file
            return FILE_ERROR;
            break;
        case FILE_CLOSE_ERROR:
            storeError(ERR_INI, ERC_FLASH_ERROR); //Error closing the required file
            return FILE_CLOSE_ERROR;
            break;
        default:
            /* If no errors, check if the number of returned items is the same
               as the requested ones. */
            if (expectedItems && returnValue!=expectedItems) {
                storeError(ERR_INI, ERC_FLASH_ERROR); //Number of returned items different from expected
                return ITEMS_NO_ERROR;
            }
            return NO_ERROR;
            break;
    }

    return NO_ERROR;
}


