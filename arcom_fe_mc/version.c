/*! \file   version.c
    \brief  Version functions

    <b> File informations: </b><br>
    Created: 2007/08/14 14:36:58 by avaccari

    <b> CVS informations: </b><br>
    \$Id: version.c,v 1.2 2008/02/07 16:21:24 avaccari Exp $

    This files contains all the functions necessary to handle version
    information. */

/* Includes */
#include <stdio.h>      /* printf */

#include "version.h"

/* Display version information */
/*! This function will display the version information to the console. */
void displayVersion(void){
    printf("\nFront End Monitor and Control Firmware (%s)\n",
           PRODUCT_TREE);
    printf("%s\n",
           AUTHOR);
    printf("Revision: %d.%d.%d (%s)\n\n",
           VERSION_MAJOR,
           VERSION_MINOR,
           VERSION_PATCH,
           VERSION_DATE);
    printf("Bug report: %s\n\n",
           BUGZILLA);
}


