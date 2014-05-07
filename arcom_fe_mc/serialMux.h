/*! \file       serialMux.h
    \brief      Serial multiplexing board header files

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: serialMux.h,v 1.14 2007/09/07 22:00:06 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the serial multiplexing board.
    See \ref serialMux for more information. */

/*! \defgroup   serialMux    Serial Multiplexing Board
    \ingroup    serialInterface
    \brief      Serial Multiplexing Board module
    \note       The \ref serialMux module doesn't include any submodule.

    For more information on this module see \ref serialMux.h */

#ifndef _SERIALMUX_H
    #define _SERIALMUX_H

    /* Defines */
    #define FRAME_DATA_LENGTH       3 // Maximum communication frame length in words
    #define FRAME_DATA_LENGTH_BYTES (2*FRAME_DATA_LENGTH) // Maximum communication frame length in bytes
    #define FRAME_DATA_UNIT_SIZE    8 // 8-bit per unit of data
    #define FRAME_DATA_BIT_SIZE     40// Maximum data payload in bits
    #define FRAME_DATA_MSW          2 // Most significant word of data
    #define FRAME_DATA_MDL          1 // Middle word of data
    #define FRAME_DATA_LSW          0 // Least significant word of data
    /* I/O register defines */
    #define FPGA_READY          0x5A5A          //!< FPGA ready data
    #define MUX_BASE            0x300           //!< I/O base address used by the multiplexing board
    #define SSC_BASE            MUX_BASE        //!< I/O base address used by the SSC
    #define FPGA_BASE           (MUX_BASE+0x10) //!< I/O base address used by the FPGA
    #define OWB_BASE            (MUX_BASE+0x20) //!< I/O base address used by the OWB

    #define MUX_PORT_ADD        SSC_BASE                //!< Port select register address
    #define MUX_DATA_ADD(Wo)    (2*Wo+SSC_BASE+0x02)    //!< Data words register address
    #define MUX_WLENGTH_ADD     (SSC_BASE+0x08)         //!< Write length register
    #define MUX_RLENGTH_ADD     (SSC_BASE+0x0A)         //!< Read length register
    #define MUX_COMMAND_ADD     (SSC_BASE+0x0C)         //!< Command word register
    #define MUX_BUSY_ADD        (SSC_BASE+0x0C)         //!< Busy status register

    #define MUX_FPGA_RDY_ADD    FPGA_BASE               //!< FPGA ready register (0x5A5A -> Ready)
    #define MUX_FPGA_VERSION    (FPGA_BASE+0x02)        //!< FPGA version info register
    #define MUX_BUSY_MASK       0x0001                  // Busy status register mask

    #define MUX_OWB_COMMAND     OWB_BASE                //!< OWB command register (R/W)
    #define MUX_OWB_TXRX        (OWB_BASE+0x01)         //!< OWB Transmit(W)/Receive(R) Buffer
    #define MUX_OWB_IRQ         (OWB_BASE+0x02)         //!< OWB Interrupt register (R)
    #define MUX_OWB_IRQ_EN      (OWB_BASE+0x03)         //!< OWB Interrupt enable regiaster (R/W)
    #define MUX_OWB_CLK_DIV     (OWB_BASE+0x04)         //!< OWB Clock divisor register (R/W)
    #define MUX_OWB_CONTROL     (OWB_BASE+0x05)         //!< OWB Control Register (R/W)
    #define MUX_OWB_RESET       (OWB_BASE+0x0F)         //!< Reset the one wire master in the FPGA

    /* Typedefs */
    //! Serial multiplexing board's frame
    /*! This structure contains all the informations necessary to create a
        communication frame to be sent to the mux board in order to deliver
        data to the required device.
        \param port         This is the port to be addressed.
        \param data[]       This is the data to be written/read. Even though
                            this parameter allows for 48-bit, the maximum size
                            of the data that can be transfer to/from the remote
                            hardware is 40-bit as speficified by
                            \ref FRAME_DATA_BIT_SIZE
        \param dataLength   This is the number of bits to be written/read.
        \param command      This specifies the command to be issued. These
                            commands are hardware dependent and the description
                            can be found in the hardware description documents.
        \param busy         This holds the current state of the serial mux
                            board.*/
    typedef struct {
        //! Port select
        /*! There are 24 ports + 1 spare that can be addressed via this field.
            The ports are assigned as follows:
                - 0x00 -> Cartridge0.lo
                - 0x01 -> Cartridge0.bias
                - 0x02 -> Cartridge1.lo
                - 0x03 -> Cartridge1.bias
                - 0x04 -> Cartridge2.lo
                - 0x05 -> Cartridge2.bias
                - 0x06 -> Cartridge3.lo
                - 0x07 -> Cartridge3.bias
                - 0x08 -> Cartridge4.lo
                - 0x09 -> Cartridge4.bias
                - 0x0A -> Cartridge5.lo
                - 0x0B -> Cartridge5.bias
                - 0x0C -> Cartridge6.lo
                - 0x0D -> Cartridge6.bias
                - 0x0E -> Cartridge7.lo
                - 0x0F -> Cartridge7.bias
                - 0x10 -> Cartridge8.lo
                - 0x11 -> Cartridge8.bias
                - 0x12 -> Cartridge9.lo
                - 0x13 -> Cartridge9.bias
                - 0x14 -> Cartridge Power Distribution System
                - 0x15 -> IF Switch Module
                - 0x16 -> Cryostat Module
                - 0x17 -> Photonic Switch Module
                - 0x18 -> Available */
        unsigned int port;
        //! Data
        /*! The largest trasmissible frame is 40-bit wide. The content is spread
            over the 3 unsigned int in the data array. It must be left justified
            and the data is distributed as follows:
                - Element 0 -> (bit 15-0) least significant word
                - Element 1 -> (bit 31-16) middle word
                - Element 2 -> (bit 39-32) most significant word */
        int data[FRAME_DATA_LENGTH];
        //! Data length
        /*! The ammount of bits to be written or read during an access cycle. */
        unsigned int dataLength;
        //! Command
        /*! This 5-bit field is module dependant and describes the operation that
            will be executed in the module once the serial message is received
            and decoded. */
        unsigned int command;
        //! Busy status
        /*! This 1-bit filed contain the current status of the multiplex board:
                - 0 -> Idle
                - 1 -> Busy */
        unsigned int busy;
    } FRAME;

    /* Globals */
    /* Externs */
    extern FRAME frame; //!< A global to create the frame for the mux board

    /* Prototypes */
    /* Statics */
    static int waitOnBusy(void); // Check the current state of the mux board
    /* Externs */
    extern int writeMux(void); //!< Serial Mux Board write
    extern int readMux(void); //!< Serial Mux Board read
    extern int readyMux(void); //!< Serial Mux Board ready

#endif // _SERIALMUX_H
