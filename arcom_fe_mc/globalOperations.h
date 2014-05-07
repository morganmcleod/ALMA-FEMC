/*! \file   globalOperations.h
    \brief  Global operations handling header file

    <b> File informations: </b><br>
    Created: 2006/10/24 11:53:36 by avaccari

    <b> CVS informations: </b><br>
    \$Id: globalOperations.h,v 1.7 2008/03/10 22:15:43 avaccari Exp $

    This files contains all the informations necessary to handle the global
    frontend operations in the running software. */

#ifndef _GLOBALOPERATIONS_H
    #define _GLOBALOPERATIONS_H

    /* Globals */
    /* Externs */
    extern unsigned char initializing; //!< This semaphore signals to the software that the initialization is happening
    extern unsigned char initializeFrontend; //!< This global controls the initialization of the frontend

    /* Prototypes */
    /* Externs */
    extern int initialization(void); //!< This function performs a global initialization of the system
    extern int shutDown(void); //!< This function performs a global shutdown of the system

#endif /* _GLOBALOPERATION_H */
