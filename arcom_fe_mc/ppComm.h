/*! \file   ppComm.h
    \brief  Parallel Port communication interface header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the parallel port included in the ARCOM Pegasus
    board. */

#ifndef _PPCOMM_H

    #define _PPCOMM_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Parallel port control register */
    #define EPPC_NWRITE             0x01    //!< nWrite inverted output: 0 = write, 1 = read
    #define EPPC_NDSTROBE           0x02    //!< nDataStrobe inverted output
    #define SPPC_INIT               0x04    //<! Initialize. Used to report the status to the AMBSI: 0 -> Ready
    #define SPPC_NSELECT            0x08    //<! nSelect inverted output
    #define SPPC_IRQENA             0x10    //<! Enable IRQ Via Ack/Interrupt line
    #define SPPC_DATADIR            0x20    //<! Data direction: 0 = output, 1 = input
    /* Parallel port status register */
    #define EPPS_TIMEOUT            0x01    //<! EPP timeout
    #define SPPS_NIRQIN             0x04    //<! nIRQ in
    #define SPPS_NERRORIN           0x08    //<! nError in
    #define SPPS_SELECTIN           0x10    //<! Select in. Used to check the status of the AMBSI: 0 -> Ready
    #define SPPS_PAPEROUT           0x20    //<! Paper-Out
    #define EPPS_INTERRUPT          0x40    //<! Interrupt in
    #define EPPS_NWAIT              0x80    //<! nWait inverted input

    #define PP_DEFAULT_IRQ_NO       0x07    //!< IRQ to be assigned to parallel port

    /* Globals */
    /* Externs */
    extern unsigned char PPRxBuffer[CAN_RX_MESSAGE_SIZE];   //!< Parallel port received message buffer
    extern unsigned char PPTxBuffer[CAN_TX_MAX_PAYLOAD_SIZE];   //!< Parallel port transmitted message buffer

    /* Prototypes */
    /* Statics */
    static void interrupt far PPIntHandler(void); // Interrupt function to handle incoming messages
    /* Externs */
    extern int PPOpen(void);                        //!< Configure parallel port
    extern int PPClose(void);                       //!< Close parallel port
    extern int PPStart(void);                       //!< Enable connection with AMBSI1
    extern int PPClearTimeout(void);                //!< Test and clear the timeout bit
    extern void PPWrite(unsigned char length);      //!< Write to parallel port
    extern void PPClear(void);                      //!< Clear the parallel port IRQ
    extern void PPIrqCtrl(unsigned char enable);    //!< Controls the PP interrupt enable state
    extern void PicPPIrqCtrl(unsigned char enable); //!< Controls the PIC PP interrupt enable state
    extern void PPStatusReport(void);               //!< Display PP status and buffers

#endif /* _PPCOMM_H */
