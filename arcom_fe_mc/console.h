/*! \file   console.h
    \brief  Console handling header file

    <b> File informations: </b><br>
    Created: 2007/05/22 11:31:57 by avaccari

    <b> CVS informations: </b><br>
    \$Id: console.h,v 1.3 2007/09/07 22:00:06 avaccari Exp $

    This files contains all the informations necessary to handle the console
    accesses to the software. */

#ifndef _CONSOLE_H
    #define _CONSOLE_H

    /* Defines */
    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    #define BUFFER_SIZE 255 //!< Size of the console buffer
    #define ENTER_KEY   13  //!< Key to press to evaluate buffer
    #define DELETE_KEY  8   //!< Delete key code

    /* Globals */
    /* Externs */
    extern unsigned char consoleEnable; //!< Controls the console

    /* Prototypes */
    /* Statics */
    static void parseBuffer(void);
    static long htol(char *hex);
    /* Externs */
    extern void console(void); //!< This function handles the console inputs

#endif /* _CONSOLE_H */
