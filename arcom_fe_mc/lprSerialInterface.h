/*! \file       lprSerialInterface.h
    \ingroup    serialInterface
    \brief      LPR serial interface header file

    <b> File information: </b><br>
    Created: 2007/06/02 10:38:01 by avaccari

    This file contains all the information necessary to define the
    characteristics and operate the LPR serial interface. */

/*! \defgroup   lprSerialInterface  LPR serial interface
    \ingroup    serialInterface
    \brief      LPR serial interface
    \note       The \ref lprSerialInterface module doesn't include any submodule.

    For more information on this module see \ref lprSerialInterface.h */

#ifndef _LPRSERIALINTERFACE_H
    #define _LPRSERIALINTERFACE_H

    /* Defines */
    /* General */
    #define SHUTTER_DISABLE             0
    #define SHUTTER_ENABLE              1
    #define PORT_SHUTTERED              0xFF
    #define MODULATION_INPUT_ENABLE     1
    #define MODULATION_INPUT_DISABLE    0
    #define LPR_AREG                    0
    #define LPR_BREG                    1
    #define OPTICAL_SWITCH_IDLE         0
    #define OPTICAL_SWITCH_BUSY         1
    #define STANDARD                    0
    #define FORCED                      1
    #define LPR_DAC_A_REG               0


    /* Command words:
       - Rg is the register */
    // 0x00 -> not used
    #define LPR_DAC_RESET_STROBE        0x01
    #define LPR_OPTICAL_SWITCH_STROBE   0x02
    #define LPR_ADC_CONVERT_STROBE      0x03
    // 0x04 -> not used
    #define LPR_DAC_DATA_WRITE          0x05
    #define LPR_ADC_DATA_READ           0x06
    #define LPR_10MHZ_MODE              0x07
    #define LPR_PARALLEL_WRITE(Rg)      (0x08+Rg)
    #define LPR_PARALLEL_READ           0x0A
    // 0x0B-0x1F -> not used

    /* --- AREG definitions (8-bit) --- */
    /* Write Only: 8-bit + 1 bit for latching.
       AREG is the register that contains the optical switch port selection and
       the EDFA modulation input enable. */
    #define LPR_AREG_SIZE                   9
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define LPR_AREG_SHIFT_SIZE             1
    #define LPR_AREG_SHIFT_DIR              SHIFT_LEFT
    /* bit 4-0 -> selected optical switch port:
       - Pt is the port number */
    #define LPR_AREG_SWITCH_PORT(Pt)        (0x10+Pt)
    #define LPR_AREG_SWITCH_SHUTTER         0x00
    /* bit 6-5 -> not used */
    /* bit 7 -> EDFA modulation input control */


    /* --- BREG definitions (3-bit) --- */
    /* Write Only: 3-bit + 1 bit for latching
       BREG is the register that defines which device has to be monitored. */
    #define LPR_BREG_SIZE                   4
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define LPR_BREG_SHIFT_SIZE             1
    #define LPR_BREG_SHIFT_DIR              SHIFT_LEFT
    /* bit 2-0 -> Monitor points:
       - Se is the temperature sensor number */
    #define LPR_BREG_LASER_PD_CURRENT       0x00
    #define LPR_BREG_EDFA_PD_CURRENT        0x01
    #define LPR_BREG_EDFA_PD_POWER          LPR_BREG_EDFA_PD_CURRENT
    #define LPR_BREG_TEMPERATURE(Se)        (0x02+Se)
    #define LPR_BREG_LASER_DRIVE_CURRENT    0x04
    #define LPR_BREG_PUMP_TEMP              0x05




    /* --- 10 MHz Mode command --- */
    #define LPR_10MHZ_MODE_SIZE        0
    #define LPR_10MHZ_MODE_SHIFT_SIZE  NO_SHIFT
    #define LPR_10MHZ_MODE_SHIFT_DIR   NO_SHIFT





    /* --- Status register definitions (4-bit) --- */
    /* Read only: 4-bit */
    #define LPR_STATUS_REG_SIZE        4
    #define LPR_STATUS_REG_SHIFT_SIZE  NO_SHIFT
    #define LPR_STATUS_REG_SHIFT_DIR   NO_SHIFT




    /* --- ADC definitions (16-bit) --- */
    #define LPR_ADC_STROBE_SIZE                 3           // Bit size of the LPR ADC convert strobe
    #define LPR_ADC_STROBE_SHIFT_SIZE           NO_SHIFT    // The strobe outgoing data is not shifted
    #define LPR_ADC_STROBE_SHIFT_DIR            NO_SHIFT    // The strobe outgoing data is not shifted
    #define LPR_ADC_RANGE                       65536       // Full ADC range
    #define LPR_ADC_DATA_SIZE                   18          // Read only: 2 for internal use + 16-bit
    #define LPR_ADC_DATA_SHIFT_SIZE             NO_SHIFT    // The incoming ADC data is not shifted
    #define LPR_ADC_DATA_SHIFT_DIR              NO_SHIFT    // The incoming ADC data is not shifted
    #define LPR_ADC_BUSY                        0           // Busy state signal
    #define LPR_ADC_TEMP_SCALE                  978.4736    // Scale factor for the LPR temperature sensors
    #define LPR_ADC_DRIVE_CURRENT_SCALE         800.0       // Scale factor for the laser drive current
    #define LPR_ADC_LASER_PD_CURRENT_SCALE      50.0        // Scale factor for the laser photo detector current
    #define LPR_ADC_LASER_PD_CURRRENT_OFFSET    25.0        // Offset value for the laser photo detector current
    #define LPR_ADC_EDFA_PD_CURRENT_SCALE       500.0       // Scale factor for the EDFA photo detector current
    #define LPR_ADC_EDFA_PD_POWER_COEFF_DFLT    28.0        // Default conversion coefficient for the EDFA photo detector
    #define LPR_ADC_EDFA_PD_POWER_COEFF_DFLT_OLD 18.6       // Original default coeff (early LPR hw)
    #define LPR_ADC_VOLTAGE_IN_SCALE            5.0         // Scale factor for the standard voltage monitoring



    /* --- DAC definitions --- */
    #define LPR_DAC_STROBE_SIZE         1           // Bit size of the LPR DAC strobe command
    #define LPR_DAC_STROBE_SHIFT_SIZE   NO_SHIFT    // The strobe outgoing data is not shifted
    #define LPR_DAC_STROBE_SHIFT_DIR    NO_SHIFT    // The strobe outgoing data is not shifted
    #define LPR_DAC_WORD_SIZE           2           // 2 ints necessary to contain the DAC message
    #define LPR_DAC_RANGE               65536       // Full DAC range
    #define LPR_DAC_DATA_SIZE           29          // Write only: 24-bit + 5 for internal use
    #define LPR_DAC_DATA_SHIFT_SIZE     5           // 5 extra bits to be sent
    #define LPR_DAC_DATA_SHIFT_DIR      SHIFT_LEFT  // The data has to be shifter left
    #define LPR_DAC_REGISTER(Rg)        Rg          // Select the DAC register
    #define LPR_DAC_MOD_INPUT_SCALE(Vo) (LPR_DAC_RANGE*0.2*Vo)  // Modulation input scale factor




    /* --- Optical Switch Strobe definitions --- */
    #define LPR_OPTICAL_SWITCH_STROBE_SIZE         10          // Bit size of the Optical Switch strobe command
    #define LPR_OPTICAL_SWITCH_STROBE_SHIFT_SIZE   NO_SHIFT    // The strobe outgoing data is not shifted
    #define LPR_OPTICAL_SWITCH_STROBE_SHIFT_DIR    NO_SHIFT    // The strobe outgoing data is not shifted


    /* Typedefs */
    /* LPR registers */
    //! LPR AREG bitfield (8-bit+8 -> 16-bit)
    /*! The AREG bitfield define the state of several LPR hardware components
        \param port             an unsigned int     :5 */
    typedef struct {
        //! Optical switch port
        /*! This is the currently selected port in the optical switch.
            Port 0 correspond to the shuttered mode. */
        unsigned int    port        :5;
        /* 3 unused bits */
        unsigned int                :3;
        /* 8 extra bits to fill up */
        unsigned int                :8;
    } LPR_AREG_BITFIELD;

    //! LPR AREG
    /*! The AREG union allows to address the AREG either as a single unsigned
        integer of as a bitfield structure.
        \param integer      an int
        \param bitField     a LPR_AREG_BITFIELD */
    typedef union {
        //! AREG in integer format
        /*! The LPR AREG in this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! AREG in bitfield format.
        /*! The LPR AREG in this format is very easy to update. */
        LPR_AREG_BITFIELD   bitField;
    } LPR_AREG_UNION;






    //! LPR BREG bitfield (3-bit+13 -> 16-bit)
    /*! The BREG defines which monitor point is currently selected.
        \param  monitorPoint        an unsigned int     :3 */
    typedef struct {
        //! Selected monitor point (bit 3-0)
        /*! This bitfield defines the currently selected monitor point:
                - 000   Laser Output PD Monitoring
                - 001   EDFA Output PD Monitoring
                - 010   Temperature Sensor #1
                - 011   Temperature Sensor #2
                - 100   Laser Drive Current Monitoring
                - 101   Pump Temperature Monitoring */
        unsigned int monitorPoint       :3;
        /* Extra 13 bit to fill it up */
        unsigned int                    :13;
    } LPR_BREG_BITFIELD;

    //! LPR BREG
    /*! The BREG union allows to address the BREG either as a single integer or
        as a bitfield structure.
        \param  integer     an int
        \param  bitField    a LPR_BREG_BITFIELD */
    typedef union {
        //! BREG in integer format
        /*! The LPR BREG in this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! BREG in bit fileds format
        /*! The LPR BREG in this format is very easy to update. */
        LPR_BREG_BITFIELD   bitField;
    } LPR_BREG_UNION;



    //! LPR status register bitfield (4-bit+12 -> 16-bit)
    /*! The LPR status register defined the current state of ADC and optical
        switch:
        \param edfaDriverState      an unsigned int     :1
        \param opticalSwitchState   an unsigned int     :1
        \param opticalSwitchError   an unsigned int     :1
        \param adcReady             an unsigned int     :1 */
    typedef struct {
        //! EDFA Error status
        /*! This 1-bit field return the current state of the EDFA driver:
                - 0 -> OK
                - 1 -> Error */
        unsigned int edfaDriverState    :1;
        //! Optical switch state
        /*! This 1-bit field return the current state of the optical switch:
                - 0 -> Idle
                - 1 -> Busy (Switching) */
        unsigned int opticalSwitchState :1;
        //! Optical switch error
        /*! This 1-bit field return the current error status of the optical
            switch:
                - 0 -> OK
                - 1 -> Error detected */
        unsigned int opticalSwitchError :1;
        //! ADC Ready bit
        /*! This 1-bit field return the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady           :1;
        /* 12 extra bits to fill it up */
        unsigned int                    :12;
    } LPR_STATUS_REG_BITFIELD;

    //! LPR status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param  integer     an int
        \param  bitField    a LPR_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The LPR status register in this format is ready to be read as data
            from the addressed destination device. */
        int                         integer;
        //! Status register in bit field format
        /*! The LPR status register in this format is very easy to update. */
        LPR_STATUS_REG_BITFIELD     bitField;
    } LPR_STATUS_REG_UNION;





    //! LPR DAC register bitfield (24-bit+8 -> 32-bit)
    /*! The LPR DAC register contains the current word to be sent to the DAC:
        \param data         an unsigned long: 16
        \param quickLoad    an unsigned long: 1
        \param dacSelect    an unsigned long: 1 */
    typedef struct {
        //! Data field
        /*! This 16-bit field contains the actual data in unsigned format */
        unsigned long   data            :16;
        /* a 5-bit field is required by the hardware. The value doesn't matter. */
        unsigned long                   :5;
        //! Quick load
        /*! This 1-bit field determins the behavior of the DAC2 when uploading
            the internal registers with the received data:
                - 0 -> The content of the shift register is loaded only to the
                       DAC input register that is addressed.
                - 1 -> The content of the shift register is loaded into ALL DAC's
                       input registers. */
        unsigned long   quickLoad       :1;
        /* a 1-bit field is required by the hardware. The value doesn't matter. */
        unsigned long                   :1;
        //! Register selector
        /*! This 1-bit field determins which register in the DAC is addressed:
                - 0 -> Register A
                - 1 -> Register B */
        unsigned long   dacSelect       :1;
        /* 8 extra bit to fill it up */
        unsigned long                   :8;
    } LPR_DAC_REG_BITFIELD;

    //! LPR DAC register
    /*! The LPR DAC register union allows to address the status register either
        as an array of 2 ints or as a bitfield structure.
        \param  integer[]   an int
        \param  bitField    a LPR_DAC_REG_BITFIELD */
    typedef union {
        //! DAC register in array of integer format
        /*! The LPR DAC register in this format is ready to be read as data
            from the addressed destination device. */
        int                       integer[LPR_DAC_WORD_SIZE];
        //! DAC register in bit fields format
        /*! The LPR DAC register in this format is very easy to update. */
        LPR_DAC_REG_BITFIELD      bitField;
    } LPR_DAC_REG_UNION;





    //! LPR registers
    /*! This structure contains the current state of the LPR registers.
        \param aReg         a LPR_AREG_UNION
        \param bReg         a LPR_BREG_UNION
        \param statusReg    a LPR_STATUS_REG_UNION
        \param dacReg       a LPR_DAC_REG_UNION
        \param adcData      an unsigned int */
    typedef struct {
        //! AREG
        /*! The AREG defines the state of several hardware modules in the LPR
            (Write only register). */
        LPR_AREG_UNION          aReg;
        //! BREG
        /*! The BREG defines which monitor point is currently selected (Write
            only register). */
        LPR_BREG_UNION          bReg;
        //! Status register
        /*! The status register contains the current state of several hardware
            subsystems of the LPR */
        LPR_STATUS_REG_UNION    statusReg;
        //! DAC register
        /*! The DAC register contains the message to be sent to the DAC (Write
            only register). */
        LPR_DAC_REG_UNION       dacReg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. Due to hardware configuration (unipolar)
            this variable is an unsigned int. */
        unsigned int            adcData;

    } LPR_REGISTERS;

    /* Globals */
    /* Externs */
    extern LPR_REGISTERS lprRegisters; //!< Local Oscillator Photonic Reference registers

    /* Prototypes */
    /* Statics */
    static int getLprAnalogMonitor(void); // Perform core analog monitor functions

    /* Externs */
    extern int getLprTemp(void); //!< This function monitors the LPR temperature sensors
    extern int setOpticalSwitchPort(void); //!< This function controls the port selection for the optical switch
    extern int setOpticalSwitchShutter(unsigned char mode); //!< This function enables the LPR optical switch shutter
    extern int getLprStates(void); //!< This function monitors the states of several LPR hardware
    extern int getLaserPumpTemperature(void); //!< This function monitors the temperature of the laser pump
    extern int setLaserDriveCurrent(void); //!< This function controls the EDFA laser drive current
    extern int getLaserDriveCurrent(void); //!< This function monitors the EDFA laser drive current
    extern int getLaserPhotoDetectCurrent(void); //!< This function monitors the EDFA laser photo detector current
    extern int getPhotoDetectorCurrent(void); //!< This fucntion monitors the EDFA photodetector current
    extern int getPhotoDetectorPower(void); //!< This function monitors the EDFA photodetector power
    extern int setModulationInputEnable(unsigned char enable); //!< This function controls the EDFA modulation input port enable state
    extern int setModulationInputValue(void); //!< This function control the EDFA modulation input value
    extern int setLprDacStrobe(void);  //!< This function sends the desired strobe to the DAC
#endif /* _LPRSERIALINTERFACE_H */
