/*! \file       pdSerialInterface.h
    \ingroup    serialInterface
    \brief      Cartridge power distribution serial interface header file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the cartridge power distribution serial
    interface. */

/*! \defgroup   pdSerialInterface  Cartridge Power Distribution serial interface
    \ingroup    serialInterface
    \brief      Cartridge Power Distribution System Serial Interface
    \note       The \ref pdSerialInterface module doesn't include any submodule.

    For more information on this module see \ref pdSerialInterface.h */

#ifndef _PDSERIALINTERFACE_H
    #define _PDSERIALINTERFACE_H

    /* Defines */
    /* General */
    #define PD_AREG             0
    #define PD_BREG             1

    #define PD_MODULE_ENABLE    1
    #define PD_MODULE_DISABLE   0

    #define PD_CHANNEL_CURRENT  0
    #define PD_CHANNEL_VOLTAGE  1

    /* Command words:
       - Rg is the Register */
    // 0x00-0x02 -> not used
    #define PD_ADC_CONVERT_STROBE  0x03
    // 0x04-0x05 -> not used
    #define PD_ADC_DATA_READ       0x06
    // 0x07 -> not used
    #define PD_PARALLEL_WRITE(Rg)  (0x08+Rg)
    #define PD_PARALLEL_READ       0x0A
    // 0x0B-0x1F -> not used




    /* --- AREG definitions (10-bit) --- */
    /* Write Only: 10-bit + 1 bit for latching.
       AREG is the register that defines the current enable state for the power
       distribution modules. */
    #define PD_AREG_SIZE            11
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define PD_AREG_SHIFT_SIZE      1
    #define PD_AREG_SHIFT_DIR       SHIFT_LEFT
    // bit 9 -> Band 10 power enable (0 -> OFF, 1 -> ON
    // bit 8 -> Band 9 power enable (0 -> OFF, 1 -> ON
    // bit 7 -> Band 8 power enable (0 -> OFF, 1 -> ON
    // bit 6 -> Band 7 power enable (0 -> OFF, 1 -> ON
    // bit 5 -> Band 6 power enable (0 -> OFF, 1 -> ON
    // bit 4 -> Band 5 power enable (0 -> OFF, 1 -> ON
    // bit 3 -> Band 4 power enable (0 -> OFF, 1 -> ON
    // bit 2 -> Band 3 power enable (0 -> OFF, 1 -> ON
    // bit 1 -> Band 2 power enable (0 -> OFF, 1 -> ON
    // bit 0 -> Band 1 power enable (0 -> OFF, 1 -> ON
    /* Bit 9-0 -> Power register:
       - Md is the module */
    #define SET_PD_MODULE_ENABLE(Md)    pdRegisters.aReg|(0x0001<<Md)
    #define SET_PD_MODULE_DISABLE(Md)   pdRegisters.aReg&(~(0x0001<<Md))




    /* --- BREG definitions (8-bit) --- */
    /* Write Only: 8-bit + 1 bit for latching.
       BREG defines which monitor point is selected. */
    #define PD_BREG_SIZE                    9
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define PD_BREG_SHIFT_SIZE              1
    #define PD_BREG_SHIFT_DIR               SHIFT_LEFT
    /* bit 7-4 -> band select:
       - Ca is the cartrdiges */
    #define PD_BREG_SELECT_BAND(Ca)         (Ca)
    /* bit 3-0 -> monitor point select:
       - Md is the current module
           - 0 -> +6 Current
           - 1 -> -6 Current
           - 2 -> +15 Current
           - 3 -> -15 Current
           - 4 -> +24 Current
           - 5 -> +8 Current
           - 8 -> +6 Voltage
           - 9 -> -6 Voltage
           - 10 -> +15 Voltage
           - 11 -> -15 Voltage
           - 12 -> +24 Voltage
           - 13 -> +8 Voltage
       - Pt is the port:
           - 0 -> Current
           - 1 -> Voltage */
    #define PD_BREG_SELECT_MONITOR(Md,Pt)   (0x08*Pt+Md)







    /* --- Status register definitions (4-bit) --- */
    /* Read only: 4-bit */
    #define PD_STATUS_REG_SIZE          4
    #define PD_STATUS_REG_SHIFT_SIZE    NO_SHIFT
    #define PD_STATUS_REG_SHIFT_DIR     NO_SHIFT






    /* --- ADC definitions (16-bit) --- */
    #define PD_ADC_STROBE_SIZE          3           // Bit size of the PD ADC convert strobe
    #define PD_ADC_STROBE_SHIFT_SIZE    NO_SHIFT    // The strobe outgoing data is not shifted
    #define PD_ADC_STROBE_SHIFT_DIR     NO_SHIFT    // The strobe outgoing data is not shifted
    #define PD_ADC_RANGE                65536       // Full ADC range
    #define PD_ADC_DATA_SIZE            18          // Read only: 2 for internal use + 16-bit
    #define PD_ADC_DATA_SHIFT_SIZE      NO_SHIFT    // The incoming ADC data is not shifted
    #define PD_ADC_DATA_SHIFT_DIR       NO_SHIFT    // The incoming ADC data is not shifted
    #define PD_ADC_PLUS_6I_SCALE        10.0        // Scale factor for the +6 current
    #define PD_ADC_MINUS_6I_SCALE       1.0         // Scale factor for the -6 current
    #define PD_ADC_PLUS_15I_SCALE       4.0         // Scale factor for the +15 current
    #define PD_ADC_MINUS_15I_SCALE      2.0         // Scale factor for the -15 current
    #define PD_ADC_PLUS_24I_SCALE       2.0         // Scale factor for the +24 current
    #define PD_ADC_PLUS_8I_SCALE        2.0         // Scale factor for the +8 current
    #define PD_ADC_PLUS_6V_SCALE        8.0         // Scale factor for the +6 voltage
    #define PD_ADC_MINUS_6V_SCALE       (-8.0)      // Scale factor for the -6 voltage
    #define PD_ADC_PLUS_15V_SCALE       20.0        // Scale factor for the +15 voltage
    #define PD_ADC_MINUS_15V_SCALE      (-20.0)     // Scale factor for the -15 voltage
    #define PD_ADC_PLUS_24V_SCALE       30.0        // Scale factor for the +24 voltage
    #define PD_ADC_PLUS_8V_SCALE        10.0        // Scale factor for the +8 voltage
    #define PD_ADC_BUSY                 0           // Busy state signal




    /* Typedefs */
    /* Power Distribution registers */
    //! Power Distribution BREG bitfield (8-bit+8 -> 16-bit)
    /*! The BREG defines the currently selected monitor point.
        \param monitorPoint     an unsigned char    :4
        \param monitorBand      an unsigned char    :4 */
    typedef struct {
        //! Monitor point (bit 3-0)
        /*! This bit field contains the currently selected monitor point in the
            power distribution system:
                - 0000 -> +6 Current
                - 0001 -> -6 Current
                - 0010 -> +15 Current
                - 0011 -> -15 Current
                - 0100 -> +24 Current
                - 0101 -> +8 Current
                - 1000 -> +6 Voltage
                - 1001 -> -6 Voltage
                - 1010 -> +15 Voltage
                - 1011 -> -15 Voltage
                - 1100 -> +24 Voltage
                - 1101 -> +8 Voltage */
        unsigned int monitorPoint   :4;
        //! Band select (bit 7-4)
        /*! This bit field contains the currently selected monitor band in the
            power distribution system:
                - 0000 -> Band 1
                - 0001 -> Band 2
                - 0010 -> Band 3
                - 0011 -> Band 4
                - 0100 -> Band 5
                - 0101 -> Band 6
                - 0110 -> Band 7
                - 0111 -> Band 8
                - 1000 -> Band 9
                - 1001 -> Band 10 */
        unsigned int monitorBand    :4;
    } PD_BREG_BITFIELD;



    //! Power distribution status register bitfield (4-bit+12 -> 16-bit)
    /*! The power distribution status register defines the current state of
        several hardware within the power distribution:
        \param adcReady     an unsigned int     :1 */
    typedef struct {
        /* bit 2-0 of the power distribution are unused */
        unsigned int                :3;
        //! ADC Ready bit
        /*! This 1-bit field return the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady       :1;
        /* 12 extra bit to fill it up */
        unsigned int                :12;
    } PD_STATUS_REG_BITFIELD;


    //! Power Distribution status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param integer      an int
        \param bitField     a PD_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The power distribution status register in this format is ready to be
            read as data from the addressed destination device. */
        int                         integer;
        //! Status register in bit fields format
        /*! The power distribution status register in this format is very easy
            to update. */
        PD_STATUS_REG_BITFIELD      bitField;
    } PD_STATUS_REG_UNION;




    //! Power Distribution BREG
    /*! The BREG union allows to address the BREG either as a single integer or
        as a bitfield structure.
        \param integer      an int
        \param bitField     a PD_BREG_BITFIELD */
    typedef union {
        //! BREG in integer format
        /*! The poer distribution BREG in this format is ready to be sent as
            data to the destination device. */
        int                 integer;
        //! BREG in bit fields format
        /*! The power distribution BREG in this format is very easy to
            update. */
        PD_BREG_BITFIELD    bitField;
    } PD_BREG_UNION;




    //! Power Distribution Registers
    /*! This structure contains the current state of the power distribution
        module.
        \param aReg         an int
        \param bReg         a PD_BREG_UNION
        \param statusReg    a PD_STATUS_REG_UNION
        \param adcData      an unsigned int */
    typedef struct {
        //! AREG
        /*! The AREG defined the current power state of the modules controlling
            the cartrdiges (Write only register). */
        int                     aReg;
        //! BREG
        /*! The BREG defines the currently selected monitor point in the power
            distribution system (Write only register). */
        PD_BREG_UNION           bReg;
        //! Status Registers
        /*! The status register contains the current state of several hardware
            subsystems of the power distribution */
        PD_STATUS_REG_UNION     statusReg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. Due to hardware configuration (unipolar)
            this variable is an unsigned int. */
        unsigned int            adcData;
    } PD_REGISTERS;



    /* Globals */
    /* Externs */
    extern PD_REGISTERS pdRegisters; //!< Power Distribution Registers

    /* Prototypes */
    /* Statics */
    static int getPdAnalogMonitor(void); // Perform core analog monitor functions

    /* Externs */
    extern int setPdModuleEnable(unsigned char enable); //!< This function enables/disables the selected power distribution module
    extern int getPdChannel(void); //!< This function monitors the selected power distribution channel
#endif /* _PDSERIALINTERFACE_H */
