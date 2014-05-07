/*! \file       ifSerialInterface.h
    \ingroup    serialInterface
    \brief      IF switch serial interface header file

    <b> File informations: </b><br>
    Created: 2006/11/30 16:40:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: ifSerialInterface.h,v 1.8 2010/03/03 15:43:18 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the IF switch serial interface. */

/*! \defgroup   ifSerialInterface  IF Switch serial interface
    \ingroup    serialInterface
    \brief      IF Switch System Serial Interface
    \note       The \ref ifSerialInterface module doesn't include any submodule.

    For more information on this module see \ref ifSerialInterface.h */
#ifndef _IFSERIALINTERFACE_H
    #define _IFSERIALINTERFACE_H

    /* Defines */
    /* General */
    #define IF_AREG                 0
    #define IF_BREG                 1
    #define IF_CREG                 2
    #define IF_DREG                 3
    #define IF_EREG                 4
    #define IF_FREG                 5
    #define IF_GREG                 6

    #define IF_TEMP_SERVO_ENABLE    1
    #define IF_TEMP_SERVO_DISABLE   0

    /* Command words:
       - Rg is the Register */
    #define IF_PARALLEL_WRITE(Rg)   Rg
    // 0x07 -> not used
    #define IF_ADC_CONVERT_STROBE   0x08
    #define IF_ADC_DATA_READ        0x09
    #define IF_PARALLEL_READ        0x0A


    /* --- AREG difinitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       AREG is the register that defines which way is selected by the IF
       siwtch. */
    #define IF_AREG_SIZE         5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_AREG_SHIFT_SIZE   1
    #define IF_AREG_SHIFT_DIR    SHIFT_LEFT
    /* bit 3-0 channel select:
       - Id is the index
           - 0 -> Way 0
           - 1 -> Way 1
           - 2 -> Way 2
           - 3 -> Way 3
           - 4 -> Way 4
           - 5 -> Way 5
           - 6 -> Way 6
           - 7 -> Way 7
           - 8 -> Way 8
           - 9 -> Way 9 */
    #define IF_AREG_SELECT_WAY(Id)  (currentIfSwitchWay[Id])



    /* --- BREG definitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       BREG is the register that defines the current attenuation for IF
       channel 0. */
    #define IF_BREG_SIZE        5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_BREG_SHIFT_SIZE  1
    #define IF_BREG_SHIFT_DIR   SHIFT_LEFT
    /* bit 3-0 -> Attenuation setting in dB */



    /* --- CREG definitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       CREG is the register that defines the current attenuation for IF
       channel 1. */
    #define IF_CREG_SIZE        5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_CREG_SHIFT_SIZE  1
    #define IF_CREG_SHIFT_DIR   SHIFT_LEFT
    /* bit 3-0 -> Attenuation setting in dB */



    /* --- DREG definitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       DREG is the register that defines the current attenuation for IF
       channel 2. */
    #define IF_DREG_SIZE        5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_DREG_SHIFT_SIZE  1
    #define IF_DREG_SHIFT_DIR   SHIFT_LEFT
    /* bit 3-0 -> Attenuation setting in dB */



    /* --- EREG definitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       EREG is the register that defines the current attenuation for IF
       channel 3. */
    #define IF_EREG_SIZE        5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_EREG_SHIFT_SIZE  1
    #define IF_EREG_SHIFT_DIR   SHIFT_LEFT
    /* bit 3-0 -> Attenuation setting in dB */





    /* --- FREG definitions (4-bit) --- */
    /* Write Only: 4-bit + 1 bit for latching.
       FREG is the register that defines the current enable state of the IF
       switch temperature servo. */
    #define IF_FREG_SIZE         5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_FREG_SHIFT_SIZE   1
    #define IF_FREG_SHIFT_DIR    SHIFT_LEFT
    // bit 3 -> Temperature controller 4 enable (0 -> OFF, 1 -> ON)
    // bit 2 -> Temperature controller 3 enable (0 -> OFF, 1 -> ON)
    // bit 1 -> Temperature controller 2 enable (0 -> OFF, 1 -> ON)
    // bit 0 -> Temperature controller 3 enable (0 -> OFF, 1 -> ON)
    /* Bit 3-0 -> Temperature servo register:
       - Ch is the channel */
    #define SET_IF_TEMP_SERVO_ENABLE(Ch)     ifRegisters.fReg|(0x0001<<Ch)
    #define SET_IF_TEMP_SERVO_DISABLE(Ch)    ifRegisters.fReg&(~(0x0001<<Ch))





    /* --- GREG definitions (3-bit) --- */
    /* Write Only: 3-bit + 1 bit for latching.
       BREG is the register used to select the voltages to monitor in order to
       calculate the IF switch channels' temperature. */
    #define IF_GREG_SIZE        4
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware needs to latch the data. */
    #define IF_GREG_SHIFT_SIZE  1
    #define IF_GREG_SHIFT_DIR   SHIFT_LEFT
    /* Bit 2-0 -> Voltage select:
         - Ch is the channel:
           - 0 -> Voltage 1 IF channel 1
           - 1 -> Voltage 2 IF channel 1
           - 2 -> Voltage 1 IF channel 2
           - 3 -> Voltage 2 IF channel 2
           - 4 -> Voltage 1 IF channel 3
           - 5 -> Voltage 2 IF channel 3
           - 6 -> Voltage 1 IF channel 4
           - 7 -> Voltage 2 IF channel 4 */
    #define IF_GREG_SELECT_TEMP_MONITOR_V1(Ch)          (2*Ch)
    #define IF_GREG_SELECT_TEMP_MONITOR_V2(Ch)          (2*Ch+1)
    #define IF_GREG_SELECT_TEMP_MONITOR_V_NEW_HARDW(Ch) (Ch)



    /* --- Status register definitions (4-bit) --- */
    /* Read only: 4-bit */
    #define IF_STATUS_REG_SIZE          4
    #define IF_STATUS_REG_SHIFT_SIZE    NO_SHIFT
    #define IF_STATUS_REG_SHIFT_DIR     NO_SHIFT


    /* --- ADC definitions (16-bit) --- */
    #define IF_ADC_STROBE_SIZE          3           // Bit size of the IF switch ADC convert strobe
    #define IF_ADC_STROBE_SHIFT_SIZE    NO_SHIFT    // The strobe outgoing data is not shifted
    #define IF_ADC_STROBE_SHIFT_DIR     NO_SHIFT    // The strobe outgoing data is not shifted
    #define IF_ADC_RANGE                65536       // Full ADC range
    #define IF_ADC_DATA_SIZE            18          // Read only: 2 for internal use + 16-bit
    #define IF_ADC_DATA_SHIFT_SIZE      NO_SHIFT    // The incoming ADC data is not shifted
    #define IF_ADC_DATA_SHIFT_DIR       NO_SHIFT    // The incoming ADC data is not shifted
    #define IF_ADC_TEMP_V_SCALE         5.0         // Scale factor for the Voltage of the temperature monitor
    #define IF_ADC_BUSY                 0           // Busy state signal




    /* Typedefs */
    /* If switch registers */
    //! IF Siwtch status register bitfield (4-bit+12 -> 16-bit)
    /*! The IF switch status register defines the current state of several
        hardware within the power distribution:
        \param hardwRev     an unsigned int     :2
        \param adcReady     an unsigned int     :1 */
    typedef struct {
        //! Hardware revision level
        /*! This 2-bits return the hardware revision level of the
            IF switch M&C board */
        unsigned int hardwRev       :2;
        /* bit 2 of thae power distribution are unused */
        unsigned int                :1;
        //! ADC Ready bit
        /*! This 1-bit field return the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady       :1;
        /* 12 extra bit to fill it up */
        unsigned int                :12;
    } IF_STATUS_REG_BITFIELD;


    //! IF Switch status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param integer      an int
        \param bitField     a IF_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The IF Switch status register in this format is ready to be read as
            data from the addressed destination device. */
        int                         integer;
        //! Status register in bit fields format
        /*! The IF Switch status register in this format is very easy to
            update. */
        IF_STATUS_REG_BITFIELD      bitField;
    } IF_STATUS_REG_UNION;



    //! IF Switch Registers
    /*! This structure contains the current state of the IF Switch module.
        \param aReg         an int
        \param bReg         an int
        \param cReg         an int
        \param dReg         an int
        \param eReg         an int
        \param fReg         an int
        \param gReg         an int
        \param statusReg    a IF_STATUS_REG_UNION
        \param adcData      an int */
    typedef struct {
        //! AREG
        /*! The AREG defines the currently selected way in the IF switch.
            (Write only register) */
        int                     aReg;
        //! BREG
        /*! The BREG defines the attenuation for channel 0 of the IF switch.
            (Write only register) */
        int                     bReg;
        //! CREG
        /*! The CREG defines the attenuation for channel 1 of the IF switch.
            (Write only register) */
        int                     cReg;
        //! DREG
        /*! The DREG defines the attenuation for channel 2 of the IF switch.
            (Write only register) */
        int                     dReg;
        //! EREG
        /*! The EREG defines the attenuation for channel 3 of the IF switch.
            (Write only register) */
        int                     eReg;
        //! FREG
        /*! The FREG defines the current enable state of the temperature servos
            in the IF switch. (Write only register) */
        int                     fReg;
        //! GREG
        /*! The GREG defines which voltage to monitor in order to calculate the
            current temperature of the currently selected IF channel. (Write
            only register) */
        int                     gReg;
        //! Status Registers
        /*! The status register contains the current state of several hardware
            subsystems of the IF Switch */
        IF_STATUS_REG_UNION     statusReg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. Due to hardware configuration (unipolar)
            this variable is an unsigned int. */
        unsigned int            adcData;
    } IF_REGISTERS;



    /* Prototypes */
    /* Statics */
    static int getIfAnalogMonitor(void); // Perform core analog monitor functions

    /* Externs */
    extern int setIfTempServoEnable(unsigned char enable); //!< This function enables/disables the IF switch temperature servo
    extern int getIfChannelTemp(void); //!< This function monitors the IF channel temperature
    extern int setIfChannelAttenuation(void); //!< This function controls the IF channel attenuation
    extern int setIfSwitchBandSelect(void); //!< This function controls the IF switch band selection
    extern int getIfSwitchHardwRevision(void); //!< This function return the IF switch M&C board hardware revision level

#endif /* _IFSERIALINTERFACE_H */
