/*! \file   database.h
    \brief  Debug handling header file

    <b> File informations: </b><br>
    Created: 2004/08/31 18:21:05 by avaccari

    <b> CVS informations: </b><br>
    \$Id: database.h,v 1.19 2007/10/02 22:04:58 avaccari Exp $

    This files contains all the informations necessary to interface with the
    configuration database.

    This files right now contains only few defines used to enable, disable
    sections of code requiring access to the configuration database. */

#ifndef _DATABASE_H
    #define _DATABASE_H

    /* Defines */
    /* Extra includes */
    /* DEBUG */
    #ifndef _DEBUG_H
        #include "debug.h"
    #endif /* _DEBUG_H */

    #define DATABASE_HARDW         // HW existance checking controlled by INI files.

#endif /* _DATABASE_H */
