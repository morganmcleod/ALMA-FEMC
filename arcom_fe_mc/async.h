/*! \file   async.h
    \brief  Async operations header file

    <b> File informations: </b><br>
    Created: 2009/03/25 18:20:44 by avaccari

    <b> CVS informations: </b><br>
    \$Id: async.h,v 1.1 2009/04/09 02:09:55 avaccari Exp $

    This file contains the information necessary to define the characteristics
    of the functions that take care of executing operation while the cpu is idle
    between incoming CAN messages. */

#ifndef _ASYNC_H
    #define _ASYNC_H

    /* Defines */

    /* Typedefs */



    /* Globals */
    /* Externs */
    extern unsigned char asyncRequired; //!< This notifies that some async operations are required
    /* Statics */


    /* Prototypes */
    /* Externs */
    extern void async(void); //!< This function takes care of the async operations

#endif /* _ASYNC_H */
