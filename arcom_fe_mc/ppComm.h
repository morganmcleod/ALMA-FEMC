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

    /* Parallel Port Defines */
    #define PP_DEFAULT_IRQ_NO       0x07    //!< IRQ to be assigned to parallel port
    #define PP_IRQ_ENA              0x10    // IRQ enable line in the control port (bit 4 = IRQE)
    #define PP_DATA_DIR             0x20    // Data direction line in the control port (bit 5 = PCD: 0 = Write, 1 = Read)
    #define SPP_STATUS_SELECT       0x10    // Select pin on the SPP status port. Used to check the status of the AMBSI: 0 -> Ready
    #define SPP_CONTROL_INIT        0x04    // nInit pin on the SPP control port. Used to report the status to the AMBSI: 0 -> Ready
    #define WAIT_ON_AMBSI_NOT_READY 100     // Number of milliseconds to wait before checking AMBSI status at initialization
    #define ARCOM_READY             0x5A    // Data to be sent to the AMBSI1 to indicate that the ARCOM is ready

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
    extern void PPWrite(unsigned char length);      //!< Write to parallel port
    extern void PPClear(void);                      //!< Clear the parallel port IRQ
    extern void PPIrqCtrl(unsigned char enable);    //!< Controls the PP interrupt enable state
    extern void PicPPIrqCtrl(unsigned char enable); //!< Controls the PIC PP interrupt enable state

#endif /* _PPCOMM_H */
