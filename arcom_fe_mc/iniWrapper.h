/*! \file       iniWrapper.h
    \brief      Header for wrapper of 3rd party INI file reader

    <b> File informations: </b><br>
    Created: 2007/06/29 16:47:04 by avaccari

    <b> CVS informations: </b><br>
    \$Id: iniWrapper.h,v 1.2 2007/08/16 14:09:38 avaccari Exp $

    This file is a wrapper to interface with a 3rd party library to access
    configuration files written in INI format. */



#ifndef _INIWRAPPER_H
    #define INI_WRAPPER_H

    /* Extra includes */
    #ifndef INI_H_
        #include "3rdParty/ini.h"
    #endif /* INI_H_ */

    /* Defines */
    #define DATA_NOT_FOUND      (-1)
    #define FILE_OPEN_ERROR     (-2)
    #define FILE_ERROR          (-3)
    #define FILE_CLOSE_ERROR    (-4)


    /* Prototypes */
    /* Statics */
    /* Externs */
    extern int myReadCfg(const char *fileName,
                         char *sectionName,
                         CFG_STRUCT *searchVar,
                         unsigned char expectedItems); //!< This function will read the specified data from the specified file
#endif /* _INIWRAPPER_H */
