/*! \file       serialInterface.h
    \brief      Serial Interface header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the entire serial interface system.
    See \ref serialInterface for more information. */

/*! \defgroup   serialInterface    Serial Interface
    \brief      Serial Interface module

    This group includes all the different \ref serialInterface submodules. For
    more information on the \ref serialInterface module see \ref serialInterface.h */

#ifndef _SERIALINTERFACE_H
    #define _SERIALINTERFACE_H

    /* Defines */
    #define COMMAND_WORD_SIZE       0x1F //!< Maximum size of the command word (5-bit)
    #define SERIAL_READ             0    //!< Serial read
    #define SERIAL_WRITE            1    //!< Serial write

    /* Prototypes */
    /* Externs */
    extern int serialAccess(unsigned int command,
                            int *reg,
                            unsigned char regSize,
                            unsigned char shiftAmount,
                            unsigned char shiftDir,
                            unsigned char write); //!< Serial Access funtion

#endif // _SERIALINTERFACE_H
