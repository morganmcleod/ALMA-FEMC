/*! \file   pegasus.h
    \brief  ARCOM Pegasus header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: pegasus.h,v 1.9 2009/03/23 20:43:04 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the ARCOM Pegasus board. */

#ifndef _PEGASUS_H

    #define _PEGASUS_H

    /* Super I/O (FDC37B72x) */
    /* General */
    #define SIO_CONFIG_PORT         0x0370              //!< SuperIO Config Port
    #define SIO_INDEX_PORT          SIO_CONFIG_PORT     //!< SuperIO Index Port
    #define SIO_DATA_PORT           (SIO_INDEX_PORT+1)  //!< SuperIO Data Port
    #define SIO_CONFIG_MODE_KEY     0x55                //!< Code to set SuperIO in Configure Mode
    #define SIO_RUN_MODE_KEY        0xAA                //!< Code to set SuperIO in Run Mode
    /* Global Configuration Registers */
    #define GC_INDEX_ADDRESSES      0x03    //!< Code to modify Index Addresses
    #define GC_RUN_ADDRESS_MASK     0x03    //!< Mask for selecting the run address configuration bits
    #define GC_RUN_ADDRESS_ENABLE   0x80    //!< Mask for selecting the run address enable bits
    #define GC_DEVICE_NO            0x07    //!< Code to select Logic Device
    /* Logical Device Configuration Register */
    /* LD 3 -> Parallel Port */
    #define LD3_DEVICE_NO       0x03    //!< 0x03 -> Parallel Port
    #define LD3_ACTIVATE_INDEX  0x30    //!< 0x30 -> Activate
    #define LD3_ACTIVATE_KEY    0x01    //!< Code to activate the selected device
    #define LD3_ADDRESS_INDEX1  0x60    //!< 0x60 -> Primary Base IO (Byte1)
    #define LD3_ADDRESS_INDEX2  0x61    //!< 0x60 -> Primary Base IO (Byte1)
    #define LD3_PRIMARY_INT     0x70    //!< 0x70 -> Primary Interrupt
    #define LD3_MODE_REGISTER1  0xF0    //!< 0xF0 -> Parallel Port Mode Register
    #define LD3_MODE_REGISTER2  0xF1    //!< 0xF1 -> Parallel Port Mode Register 2
    #define LD3_EPP_MODE_KEY    0x05    //!< Code to set parallel port mode to EPP 1.7

    /* AMD ELAN SC520 */
    /* Programmable Interrupt Controller (PIC) */
    #define PIC_ADDR1       0x20    //!< Base address of the PIC
    #define PIC_ADDR2       0xA0    //!< Base address of secondary PIC
    #define PIC_INT_CLR     0x20    //!< Data to clear the interrupt request

    /* Typedefs */
    typedef struct PACKETIN {
        unsigned int        Info_Level;
        unsigned long int   Serial;
        unsigned char       Label[11];
        unsigned char       File_Sys_Type[8];
    };

    /* Globals */
    /* Externs */

    /* Prototypes */
    /* Externs */
    extern void reboot(void);   //!< Perform a reboot
    extern long int getVolSerial(void); //! Returns the serial number of the current drive

#endif /* _PEGASUS_H */
