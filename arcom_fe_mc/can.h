/*! \file   can.h
    \brief  CAN header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the Controlled Area Network interface included
    in the AMBSI1 board. */

#ifndef _CAN_H
    #define _CAN_H

    #include <string.h> /* memcpy */

    /* Defines */
    /* General */
    #define CAN_MESSAGE_PAYLOAD_SIZE        0x08    // Max size of the CAN message payload
    #define CAN_RX_HEADER_SIZE              0x05    // Size of the the message header (RCA + Payload size)
    #define CAN_RX_MESSAGE_SIZE             0x0D    // Maximum size of the can message transmitted by the AMBSI1 on the parallel port
    #define CAN_RX_MAX_PAYLOAD_SIZE         (CAN_MESSAGE_PAYLOAD_SIZE)  // Max payload size to receive from parallel port
    #define CAN_TX_MAX_PAYLOAD_SIZE         (CAN_MESSAGE_PAYLOAD_SIZE)  // Maximum size of the can message to be transmitted to the AMBSI1 on the parallel port
    #define CAN_MONITOR                     0x00    // Size of a monitor message
    #define CAN_FULL_SIZE                   (CAN_MESSAGE_PAYLOAD_SIZE)  // Size of a full payload
    #define CAN_FLOAT_SIZE                  0x04    // Size of a float payload
    #define CAN_REV_SIZE                    0x03    // Size of a revision level message
    #define CAN_INT_SIZE                    0x02    // Size of an int payload
    #define CAN_BYTE_SIZE                   0x01    // Siza of a char payload
    #define CAN_BOOLEAN_SIZE                0x01    // Size of a enable/disable state payload
    #define CAN_LAST_CONTROL_MESSAGE_SIZE   (CAN_MESSAGE_PAYLOAD_SIZE+2) // Size of the last control message
    /* Type substitution macros for CAN data import/export */
    #define CAN_MSG             CANMessage
    #define CAN_DATA_ADD        CANMessage.data
    #define CAN_DATA(idx)       CANMessage.data[idx]
    #define CAN_BYTE            CAN_DATA(0)
    #define CAN_SIZE            CANMessage.size
    #define CAN_ADDRESS         CANMessage.address
    #define CAN_STATUS          CANMessage.status


    /* Classes definition */
    #define CLASSES_NUMBER      3           // See the list below
    #define CLASSES_RCA_MASK    0x30000L    /* Mask to extract the different classes:
                                               0 -> Monitor Messages
                                               1 -> Control Messages
                                               2 -> Special Messages
                                               3 -> AMBSI Reserved (Should not receive) */
    #define CLASSES_MASK_SHIFT  16          // Bits right shift for the classes mask
    #define CONTROL_CLASS       1

    /* Modules definition */
    #define MODULES_NUMBER      15          // See the list below
    #define BAND1_MODULE        0x00
    #define BAND2_MODULE        0x01
    #define BAND3_MODULE        0x02
    #define BAND4_MODULE        0x03
    #define BAND5_MODULE        0x04
    #define BAND6_MODULE        0x05
    #define BAND7_MODULE        0x06
    #define BAND8_MODULE        0x07
    #define BAND9_MODULE        0x08
    #define BAND10_MODULE       0x09
    #define POWER_DIST_MODULE   0x0A
    #define IF_SWITCH_MODULE    0x0B
    #define CRYO_MODULE         0x0C
    #define LPR_MODULE          0x0D
    #define LPR2_MODULE         0x0E    // When configured for 2 LPRs this masks the FETIM RCAs
    #define FETIM_MODULE        0x0E
    #define MODULES_RCA_MASK    0x0F000     /* Mask to extract the module number:
                                               0-9 -> Cartridge (10 modules -> 1 handler)
                                               A   -> Power distribution
                                               B   -> IF switch
                                               C   -> Cryostat
                                               D   -> LPR
                                               E   -> FETIM
                                               F   -> Available for more modules */
    #define MODULES_MASK_SHIFT  12          // Bits right shift for the modules mask



    /* Standard RCAs */
    /* Monitor */
    //! \b 0x00001 -> Base address for the monitor RCAs
    /*! This is the starting relative CAN address for the monitor requests
        available in the firmware. Address 0x00000 is reserved within the
        AMBSI1 to return the ID. */
    #define BASE_MONITOR_RCA    0x00001L
    #define LAST_MONITOR_RCA    (BASE_MONITOR_RCA + 0x0FFFF - 1) // Last possible monitor RCA
    /* Control */
    //! \b 0x10000 -> Base address for the control RCAs
    /*! This is the starting relative CAN address for the control requests
        available in the firmware. */
    #define BASE_CONTROL_RCA    0x10000L
    #define LAST_CONTROL_RCA    (BASE_CONTROL_RCA + 0x0FFFF) // Last possible control RCA


    /* Special RCAs */
    /* Monitor */
    //! \b 0x20000 -> Base address for the special monitor RCAs
    /*! This is the starting relative CAN address for the special monitor
        requests available in the firmware.
        \warning    This is the only RCA that has to be specified in \b both
                    firmwares to allow the system to work. If it is necessary to
                    change this address then a change \b has to be made in the
                    AMBSI1 firmware to reflect the new value and vice-versa.
                    The AMBSI1 uses this address to get information about its
                    firmware version so this request should never be received by
                    this software. */
    #define BASE_SPECIAL_MONITOR_RCA    0x20000L
    #define GET_AMBSI1_VERSION_INFO     0x20000L    //!< \b BASE+0x00 -> AMBSI1 dedicated message to get firmware version. It should never be received from this software
    #define GET_SETUP_INFO              0x20001L    //!< \b BASE+0x01 -> AMBSI1 dedicated message to get setup info. It should never be received from this software
    #define GET_ARCOM_VERSION_INFO      0x20002L    //!< \b BASE+0x02 -> Information about the ARCOM Pegasus firware version. This is the first addressable special RCA (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_SPECIAL_MONITOR_RCAS    0x20003L    //!< \b BASE+0x03 -> Information about the special monitor RCA range (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_SPECIAL_CONTROL_RCAS    0x20004L    //!< \b BASE+0x04 -> Information about the special control RCA range (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_MONITOR_RCAS            0x20005L    //!< \b BASE+0x05 -> Information about the monitor RCA range (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_CONTROL_RCAS            0x20006L    //!< \b BASE+0x06 -> Information about the control RCA range (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_PPCOMM_TIME             0x20007L    //!< \b BASE+0x07 -> Returns 8 bytes message allowing to evaluate the longest communication time on the parallel port (Don't change the offset of this RCA, since it is register with the same number in the AMBSI1)
    #define GET_FPGA_VERSION_INFO       0x20008L    //!< \b BASE+0x08 -> Information about the FPGA firmware version.
    #define GET_CONSOLE_ENABLE          0x20009L    //!< \b BASE+0x09 -> Returns the current state of the console (0->disabled 1->enabled)
    #define GET_ESNS_FOUND              0x2000AL    //!< \b BASE+0x0A -> Returns the number of ESNs found
    #define GET_ESNS                    0x2000BL    //!< \b BASE+0x0B -> Returns the list of the found ESNs
    #define GET_ERRORS_NUMBER           0x2000CL    //!< \b BASE+0x0C -> Returns the number unread errors in the error buffer
    #define GET_NEXT_ERROR              0x2000DL    //!< \b BASE+0x0D -> Returns the next unread error
    #define GET_FE_MODE                 0x2000EL    //!< \b BASE+0x0E -> Returns the current FE operating mode
    #define GET_TCPIP_ADDRESS           0x2000FL    //!< \b BASE+0x0E -> Returns the IP address of the FEMC module ethernet port
    #define GET_LO_PA_LIMITS_TABLE_ESN  0x20010L    //!< \b BASE+0x10 through 0x19 return the PA LIMITS table ESN for band 1-10
    #define LAST_SPECIAL_MONITOR_RCA    (BASE_SPECIAL_MONITOR_RCA+0x00FFF)  // Last possible special monitor RCA
    /* Control */
    //! \b 0x21000 -> Base address for the special control RCAs
    /*! This is the starting relative CAN address for the special control
        requests available in the firmware. */

    #define BASE_SPECIAL_CONTROL_RCA    0x21000L
    #define SET_EXIT_PROGRAM            0x21000L    //!< \b BASE+0x00 -> Ends the execution of the main program
    #define SET_REBOOT                  0x21001L    //!< \b BASE+0x01 -> Reboots the ARCOM board
    #define SET_PPCOMM_BYTES            0x21007L    //!< \b BASE+0x07 -> Bytes to return from GET_PPCOMM_TIME.  Defaults to 8 0xFF.
    #define SET_CONSOLE_ENABLE          0x21009L    //!< \b BASE+0x09 -> Enables/Disables the console
    #define SET_WRITE_NV_MEMORY         0x2100DL    //!< \b BASE+0x0D -> Writes cold head hours to the flash disk
    #define SET_FE_MODE                 0x2100EL    //!< \b BASE+0x0E -> Changes the current FE operating mode
    #define SET_READ_ESN                0x2100FL    //!< \b BASE+0x0F -> Forces the firmware to read again the ESN available on the OWB
    #define SET_LO_CLEAR_PA_LIMITS      0x21020L    //!< \b BASE+0x20 through 0x29 clear the PA LIMITS table for band 1-10
    #define SET_LO_SET_PA_LIMITS_ENTRY  0x21030L    //!< \b BASE+0x30 through 0x39 upload a PA LIMITS table entry for band 1-10
    #define LAST_SPECIAL_CONTROL_RCA    (BASE_SPECIAL_CONTROL_RCA+0x00FFF)  // Last possible special monitor RCA


    /* Typedefs */
    //! CAN message format
    /*! This structure defines the CAN message format.
        \param  address an unsigned long
        \param  size    an unsigned char
        \param  data[]  an unsigned char
        \param  status  an unsigned char */
    typedef struct {
        //! Relative CAN Address (RCA)
        /*! This is the RCA of the handled message. Different functions are
            addressed depending on the RCA of the message. */
        unsigned long   address;
        //! CAN Message size
        /*! This is the size of the CAN message payload.
            \note   The monitor messages have \p size = 0. This is the way
                    the software distinguishes between a monitor and a control
                    message. */
        unsigned char   size;
        //! CAN Message payload
        /*! This is the CAN message payload. */
        unsigned char   data[CAN_MESSAGE_PAYLOAD_SIZE];
        //! CAN message status
        /*! This contains the error state of the current CAN operation. This
            status will be stored as the first byte after the payload for
            standard CAN monitor messages:
                - general
                    - \ref NO_ERROR         -> the operation was succesful
                    - \ref ERROR            -> the operation was \em not succesful (error while communicating with hardware)
                    - \ref HARDW_RNG_ERR    -> the addressed hardware is not installed
                    - \ref HARDW_BLKD_ERR   -> the addressed hardware is locked
                - if monitor message on a monitor point:
                    - \ref MON_CAN_RNG      -> the monitor message RCA is out of allowed range
                - if monitor message on a control point:
                    - \ref CON_ERROR_RNG    -> the set value in the last control operation is outside the allowed range */
        unsigned char   status;
    } CAN_MESSAGE;

    //! CAN last control message format
    /*! This structure defines the CAN last control message format. This is
        used to store the last issued control command for a particular RCA.
        This is needed because in case a monitor request is received on a
        control RCA, the returned message should contain the last issued
        control message.
        \param  size    an unsigned char
        \param  data[]  an unsigned char
        \param  status  an unsigned char */
    typedef struct {
        //! CAN last control message size
        /*! This is the size of the CAN message payload. */
        unsigned char   size;
        //! CAN Message payload
        /*! This is the CAN message payload. */
        unsigned char   data[CAN_MESSAGE_PAYLOAD_SIZE];
        //! Last control operation status
        /*! This contains an code resulting from the last control operation:
                - \ref NO_ERROR         -> the operation was succesful and the control data was within the allowed range
                - \ref ERROR            -> the operation was nor succesful (error while communicating with the hardware)
                - \ref CON_ERROR_RNG    -> the operation was succesful but the control data was outside the allowed range
                - \ref HARDW_BLKD_ERR   -> the addressed hardware is locked */
        unsigned char   status;
    } LAST_CONTROL_MESSAGE;

    //! A macro to save the incoming control message into a LAST_CONTROL_MESSAGE struct
    //!  then reset its status to NO_ERROR prior to command processing.
    //!  This pattern is repeated all over the code!
    #define SAVE_LAST_CONTROL_MESSAGE(target) { \
        memcpy(&target, &CAN_SIZE, CAN_LAST_CONTROL_MESSAGE_SIZE); \
        target.status = NO_ERROR; }

    //! A macro to return the last control message and status.
    //!  This pattern is repeated all over the code!
    #define RETURN_LAST_CONTROL_MESSAGE(source) { \
        memcpy(&CAN_SIZE, &source, CAN_LAST_CONTROL_MESSAGE_SIZE); }

    /* Globals */
    /* Externs */
    extern volatile unsigned char newCANMsg;    //!< Notifier to the main program that a new CAN message has arrived
    extern CAN_MESSAGE CANMessage;              //!< A global to deal with the received message
    extern unsigned char currentClass;          //!< A global to store the current RCA class
    extern unsigned char currentModule;         //!< A global to store the current module info

    /* Prototypes */
    /* Statics */
    /* A function to build CANMessage with the incoming data */
    static void receiveCANMessage(void);
    static void sendCANMessage(int appendStatusByte);
    /* All the handlers for the different messages */
    /* Classes */
    static void standardRCAsHandler(void);
    static void specialRCAsHandler(void);
    /* Externs */
    extern void CANMessageHandler(void); //!< This function deals with the incoming can message
    extern void configureLPR2(void);     //!< for BELO test set, there is a 2nd LPR instead of the FETIM

#endif /* _CAN_H */
