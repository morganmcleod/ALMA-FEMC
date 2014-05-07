/*! \file       biasSerialInterface.h
    \ingroup    serialInterface
    \brief      BIAS serial interface header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: biasSerialInterface.h,v 1.38 2008/02/28 22:15:05 avaccari Exp $

    This file contains all the informations necessary to define the
    characteristics and operate the BIAS serial interface. */

/*! \defgroup   biasSerialInterface  Cartridge BIAS serial interface
    \ingroup    serialInterface
    \brief      Cartridge BIAS System Serial Interface
    \note       The \ref biasSerialInterface module doesn't include any submodule.

    For more information on this module see \ref biasSerialInterface.h */

#ifndef _BIASSERIALINTERFACE_H
    #define _BIASSERIALINTERFACE_H

    /* Extra includes */
    /* CARTRIDGE defines */
    #ifndef _CARTRIDGE_H
        #include "cartridge.h"
    #endif  /* _CARTRIDGE_H */

    /* GLOBALDEFINITIONS defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */


    /* Defines */
    /* General */
    #define BIAS_AREG                   0
    #define BIAS_BREG                   1
    #define BIAS_DAC1                   0
    #define BIAS_DAC2                   1
    #define SIS_MIXER_BIAS_VOLTAGE      0
    #define SIS_MIXER_BIAS_CURRENT      1
    #define SIS_MIXER_BIAS_MODE_OPEN    1
    #define SIS_MIXER_BIAS_MODE_CLOSE   0
    #define SIS_MAGNET_BIAS_VOLTAGE     0
    #define SIS_MAGNET_BIAS_CURRENT     1
    #define LNA_BIAS_DISABLE            0
    #define LNA_BIAS_ENABLE             1
    #define LNA_STAGE_DRAIN_V           0
    #define LNA_STAGE_DRAIN_C           1
    #define LNA_STAGE_GATE_V            2
    #define LNA_LED_ENABLE              1
    #define LNA_LED_DISABLE             0
    #define SIS_HEATER_ENABLE           1
    #define SIS_HEATER_DISABLE          0
    #define DAC_RESET_STROBE            0
    #define DAC_CLEAR_STROBE            1
    #define CARTRIDGE_TEMP_CONV_ERR     FLOAT_ERROR
    #define CARTRIDGE_TEMP_TBL_SIZE     187
    #define CARTRIDGE_TEMP_READOUTS     2

    /* Command words:
       - Po is the polarization
       - Da is the DAC
       - Rg is the Register */
    #define BIAS_ADC_CONVERT_STROBE(Po)       (0x10*Po+0x03)
    #define BIAS_DAC_DATA_WRITE(Po,Da)        (0x10*Po+Da+0x04)
    #define BIAS_ADC_DATA_READ(Po)            (0x10*Po+0x06)
    #define BIAS_10MHZ_MODE(Po)               (0x10*Po+0x07)
    #define BIAS_PARALLEL_WRITE(Po,Rg)        (0x10*Po+Rg+0x08)
    #define BIAS_PARALLEL_READ(Po)            (0x10*Po+0x0A)
    #define BIAS_DAC_RESET_STROBE(Po,Da)      (0x10*Po+Da)
    #define BIAS_DAC_CLEAR_STROBE(Po)         (0x10*Po+0x02)
    // 0x0B-0x0F, 0x1B-0x1F -> not used


    /* --- AREG definitions (12-bit) --- */
    /* Write Only: 12-bit + 1 bit for latching.
       AREG is the register that defines which device has to be monitored. */
    #define BIAS_AREG_SIZE              13
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define BIAS_AREG_SHIFT_SIZE        1
    #define BIAS_AREG_SHIFT_DIR         SHIFT_LEFT
    // bit 11 -> not used
    /* bit 10-7 -> Monitor Points:
       - Sb is the sideband
       - Cu is the current (if monitoring current Cu=1) */
    #define BIAS_AREG_SIS_MIXER(Sb,Cu)      (2*Sb+Cu+0x01)
    #define BIAS_AREG_SIS_MAGNET(Sb,Cu)     (2*Sb+Cu+0x05)
    #define BIAS_AREG_SIS_HEATER            0x0009
    #define BIAS_AREG_CARTRIDGE_TEMP        0x000A
    /* bit 6-5 -> Temperature sensor:
        - Se is the sensor number */
    #define BIAS_AREG_TEMP_SENSOR(Se)       Se
    /* bit 4-2 -> LNA Stage Port:
        - Sb is the sideband
        - Pt is the port
           - 0 -> Drain Voltage
           - 1 -> Drain Current
           - 2 -> Gate Voltage */
    #define BIAS_AREG_LNA_STAGE_PORT(Sb,Pt) ((3*Sb+Pt+0x04)&0x07)
    /* bit 1-0 -> LNA Stage:
        - St is the stage */
    #define BIAS_AREG_LNA_STAGE(St)         St







    /* ---- BREG definitions (8-bit) ---- */
    /* Write Only: 8-bit + 1 bit for latching.
       BREG is a state register, it can only be written as a whole and every
       time is updated in the hardware, the state described by the different
       fields is update accordingly. */
    #define BIAS_BREG_SIZE              9
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define BIAS_BREG_SHIFT_SIZE        1
    #define BIAS_BREG_SHIFT_DIR         SHIFT_LEFT
    // bit 7-6 -> not used
    /* LNA mask (bit 5-4):
       - Sb is the sideband
       These are inverted respect to the numbering of the sidebands. To change
       the state of a band, the current register field is XORed with the result
       from this macro substitution:
       bit5 -> LNA1
       bit4 -> LNA2 */
    #define BIAS_BREG_LNA_ENABLE(Sb)    (2-Sb)
    /* SIS mixer mask (bit 3-2):
       - Sb is the sideband
       These are inverted respect to the numbering of the sidebands. To change
       the state of a band, the current register field is XORed with the result
       from this macro substitution:
       bit3 -> Sideband 1
       bit2 -> Sideband 2 */
    #define BIAS_BREG_SIS_MODE(Sb)      (2-Sb)
    /* HEMT led bias mask (bit 1) */
    /* Mixer heater mask (bit 0) */



    /* --- 10 MHz Mode command --- */
    #define BIAS_10MHZ_MODE_SIZE        0
    #define BIAS_10MHZ_MODE_SHIFT_SIZE  NO_SHIFT
    #define BIAS_10MHZ_MODE_SHIFT_DIR   NO_SHIFT



    /* --- Status register definitions (4-bit) --- */
    /* Read only: 4-bit */
    #define BIAS_STATUS_REG_SIZE        4
    #define BIAS_STATUS_REG_SHIFT_SIZE  NO_SHIFT
    #define BIAS_STATUS_REG_SHIFT_DIR   NO_SHIFT






    /* --- ADC definitions (16-bit) --- */
    #define BIAS_ADC_STROBE_SIZE        3           // Bit size of the BIAS ADC convert strobe
    #define BIAS_ADC_STROBE_SHIFT_SIZE  NO_SHIFT    // The strobe outgoing data is not shifted
    #define BIAS_ADC_STROBE_SHIFT_DIR   NO_SHIFT    // The strobe outgoing data is not shifted
    #define BIAS_ADC_RANGE              65536       // Full ADC range
    #define BIAS_ADC_DATA_SIZE          18          // Read only: 2 for internal use + 16-bit
    #define BIAS_ADC_DATA_SHIFT_SIZE    NO_SHIFT    // The incoming ADC data is not shifted
    #define BIAS_ADC_DATA_SHIFT_DIR     NO_SHIFT    // The incoming ADC data is not shifted
    #define BIAS_ADC_SIS_I_SCALE        20.0        // Scale factor for SIS current
    #define BIAS_ADC_SIS_V_SCALE        50.0        // Scale factor for SIS voltage
    #define BIAS_ADC_SIS_MAG_I_SCALE    250.0       // Scale factor for SIS magnet current
    #define BIAS_ADC_SIS_MAG_V_SCALE    10.0        // Scale factor for SIS magnet voltage
    #define BIAS_ADC_LNA_DRAIN_V_SCALE  10.0        // Scale factor for LNA drain voltage
    #define BIAS_ADC_LNA_DRAIN_C_SCALE  100.0       // Scale factor for LNA drain current
    #define BIAS_ADC_LNA_GATE_V_SCALE   10.0        // Scale factor for LNA gate voltage
    #define BIAS_ADC_SIS_HEATER_I_SCALE 425.0       // Scale factor for SIS heater current
    #define BIAS_ADC_CART_TEMP_V_SCALE  10.0        // Scale factor for the cartrdige temperature sensor
    #define BIAS_ADC_BUSY               0           // Busy state signal




    /* --- Common DAC definitions --- */
    #define BIAS_DAC_STROBE_SIZE        1           // Bit size of the BIAS DAC strobe commands
    #define BIAS_DAC_STROBE_SHIFT_SIZE  NO_SHIFT    // The strobe outgoing data is not shifted
    #define BIAS_DAC_STROBE_SHIFT_DIR   NO_SHIFT    // The strobe outgoing data is not shifted




    /* --- DAC1 definitions (24-bit) --- */
    #define BIAS_DAC1_WORD_SIZE                     2 // 2 ints necessary to contain the DAC1's message
    #define BIAS_DAC1_RANGE                         16384 // Full DAC1 range
    #define BIAS_DAC1_DATA_SIZE                     26 // Write only: 24-bit + 2 for internal use
    #define BIAS_DAC1_DATA_SHIFT_SIZE               2  // 2 extra bits to be sent
    #define BIAS_DAC1_DATA_SHIFT_DIR                SHIFT_LEFT // The data has to be shifted left
    #define BIAS_DAC1_TOGGLE_OFF                    0 // Toggle mode off
    #define BIAS_DAC1_READ_WRITE                    0 // Always write
    #define BIAS_DAC1_INPUT_DATA_REGISTER           3 // Input data register definition
    #define BIAS_DAC1_LNA_STAGE_PORT(Sb,Pt,St)      (0x08*Pt+0x04*Sb+St) // LNA sideband, stage and port register
    #define BIAS_DAC1_LNA_STAGE_DRAIN_V_SCALE(Vds)  (BIAS_DAC1_RANGE*(Vds/5)) // LNA stage drain voltage scale factor
    #define BIAS_DAC1_LNA_STAGE_DRAIN_C_SCALE(Ids)  (BIAS_DAC1_RANGE*(Ids/50)) // LNA stage drain current scale factor
    #define BIAS_DAC1_READY                         1 // Ready state signal
    #define BIAS_DAC1_BUSY                          0 // Busy state signal





    /* --- DAC2 definitions (24-bit) --- */
    #define BIAS_DAC2_WORD_SIZE                 2           // 2 ints necessary to contain the DAC2's message
    #define BIAS_DAC2_RANGE                     65536       // Full DAC2 range
    #define BIAS_DAC2_DATA_SIZE                 29          // Write only: 24-bit + 5 for internal use
    #define BIAS_DAC2_DATA_SHIFT_SIZE           5           // 5 extra bits to be sent
    #define BIAS_DAC2_DATA_SHIFT_DIR            SHIFT_LEFT  // The data has to be shifted left
    #define BIAS_DAC2_REGISTER(Rg)              Rg          // Select the DAC2 register
    #define BIAS_DAC2_SIS_MIXER_VOLTAGE(Sb)     Sb          // SIS mixer voltage DAC2 registers
    #define BIAS_DAC2_SIS_MAGNET_CURRENT(Sb)    (Sb+0x02)   // Sis mixer magnet DAC2 registers
    #define BIAS_DAC2_SIS_MIXER_V_SCALE(Vj)     (BIAS_DAC2_RANGE*(0.5+(Vj/50)))     // SIS mixer voltage scale factor
    #define BIAS_DAC2_SIS_MAGNET_C_SCALE(Im)    (BIAS_DAC2_RANGE*(0.5+(Im/250)))    // SIS magnet current scale factor





    /* Typedefs */
    /* BIAS Registers */
    //! BIAS AREG bitfield (12-bit+4 -> 16-bit)
    /*! The AREG bitfield defined which monitor point is currently selected.
        \param  lnaStage        an unsiged int     :2
        \param  lnaPoint        an unsigned int    :3
        \param  tempSensor      an unsigned int    :2
        \param  monitorPoint    an unsigned int    :4 */
    typedef struct {
        //!LNAstage selector (bit 1-0)
        /*! This field selects which stage of the desiredLNAis addressed:
                - 00 -> Stage 1
                - 01 -> Stage 2
                - 10 -> Stage 3 */
        unsigned int lnaStage       :2;
        //!LNAmonitor point (bit 4-2)
        /*! This field selects which unsigned characteristic of theLNAhas to be
            measured:
                - 100 -> Select LNA (SIDEBAND1) Drain Voltage Monitoring
                - 101 -> Select LNA (SIDEBAND1) Drain Current Monitoring
                - 110 -> Select LNA (SIDEBAND1) Gate Voltage Monitoring
                - 111 -> Select LNA (SIDEBAND2) Drain Voltage Monitoring
                - 000 -> Select LNA (SIDEBAND2) Drain Current Monitoring
                - 001 -> Select LNA (SIDEBAND2) Gate Voltage Monitoring */
        unsigned int lnaPoint       :3;
        //! Temperature sensor selector (bit 6-5)
        /*! This field select which temperature sensor is addressed:
                - 00 -> Select Temperature Sensor #1
                - 01 -> Select Temperature Sensor #2
                - 02 -> Select Temperature Sensor #3 */
        unsigned int tempSensor     :2;
        //! Monitor point selector (bit 10-7)
        /*! This field selects the desired monitor point:
                - 0000 -> SelectLNAVoltage/Current Monitoring
                - 0001 -> Select SIS Mixer (SIDEBAND1) Voltage Monitoring
                - 0010 -> Select SIS Mixer (SIDEBAND1) Current Monitoring
                - 0011 -> Select SIS Mixer (SIDEBAND2) Voltage Monitoring
                - 0100 -> Select SIS Mixer (SIDEBAND2) Current Monitoring
                - 0101 -> Select Magnet (SIDEBAND1) Voltage Monitoring
                - 0110 -> Select Magnet (SIDEBAND1) Current Monitoring
                - 0111 -> Select Magnet (SIDEBAND2) Voltage Monitoring
                - 1000 -> Select Magnet (SIDEBAND2) Current Monitoring
                - 1001 -> Select Heater Current Monitoring
                - 1010 -> Select Temperature Sensor Monitoring */
        unsigned int monitorPoint   :4;
        /* bit 11 of AREG is unused */
        unsigned int                :1;
        /* 4 extra bit to fill it up */
        unsigned int                :4;
    } BIAS_AREG_BITFIELD;





    //! BIAS AREG
    /*! The AREG union allows to address the AREG either as a single unsigned
        integer or as a bitfield structure.
        \param  integer     an int
        \param  bitField    a BIAS_AREG_BITFIELD */
    typedef union {
        //! AREG in integer format
        /*! The BIAS AREG in this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! AREG in bit fields format
        /*! The BIAS AREG in this format is very easy to update. */
        BIAS_AREG_BITFIELD  bitField;
    } BIAS_AREG_UNION;




    //! BIAS BREG bitfield  (8-bit+8 -> 16-bit)
    /*! The BREG defines the current state of  several submodules.
        \param  sisHeaterControl    an unsigned int    :1
        \param  lnaLedControl       an unsigned int    :1
        \param  sisBiasMode         an unsigned int    :2
        \param  lnaBiasEnable       an unsigned int    :2 */
    typedef struct {
        //! SIS heater control (bit 0)
        /*! This bit field defines the current state of the mixer heater:
                - 0 -> OFF
                - 1 -> ON */
        unsigned int sisHeaterControl   :1;
        //! LNA led control (bit 1)
        /*! This bit field defines the current state of theLNAled:
                - 0 -> OFF
                - 1 -> ON */
        unsigned int lnaLedControl      :1;
        //! SIS bias mode (bit 3-2)
        /*! This 2-bit field determines the state of the SIS bias mode:
                - bit0 -> SIS mixer in SIDEBAND2
                - bit1 -> SIS mixer in SIDEBAND1
            contain the mode of the SIS bias:
                - 0 -> Closed Loop
                - 1 -> Open Loop */
        unsigned int sisBiasMode        :2;
        //! LNA bias enable (bit 5-4)
        /*! This 2-bit field determines the state of the LNA bias:
                - bit0 -> LNA in SIDEBAND2
                - bit1 -> LNA in SIDEBAND1
            contain the state of theLNAbias:
                - 0 -> Disable/OFF
                - 1 -> Enable/ON */
        unsigned int lnaBiasEnable      :2;
        /* bit 7-6 of BREG are unused */
        unsigned int                    :2;
        /* 8 extra bit to fill it up */
        unsigned int                    :8;
    } BIAS_BREG_BITFIELD;

    //! BIAS BREG
    /*! The BREG union allows to address the BREG either as a single integer or
        as a bitfield structure.
        \param  integer     an int
        \param  bitField    a BIAS_BREG_BITFIELD */
    typedef union {
        //! BREG in integer format
        /*! The BIAS BREG in this format is ready to be sent as data to the
            destination device.  */
        int        integer;
        //! BREG in bit fields format
        /*! The BIAS BREG in this format is very easy to update. */
        BIAS_BREG_BITFIELD  bitField;
    } BIAS_BREG_UNION;






    //! BIAS status register bitfield (4-bit+12 -> 16-bit)
    /*! The BIAS status register defines the current state of DAC and ADCs:
        \param dac1Ready    an unsigned int     :1
        \param adcReady     an unsigned int     :1 */
    typedef struct {
        /* bit 1-0 of the BIAS status register are unused */
        unsigned int            :2;
        //! DAC1 Ready bit
        /*! This 1-bit field return the current status of the DAC1:
                - 0 -> DAC1 Busy
                - 1 -> DAC1 Ready */
        unsigned int dac1Ready  :1;
        //! ADC Ready bit
        /*! This 1-bit field returns the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady   :1;
        /* 12 extra bit to fill it up */
        unsigned int            :12;
    } BIAS_STATUS_REG_BITFIELD;

    //! BIAS status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param  integer     an int
        \param  bitField    a BIAS_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The BIAS status register in this format is ready to be read as data
            from the addressed destination device.  */
        int                         integer;
        //! Status register in bit fields format
        /*! The BIAS status register in this format is very easy to update. */
        BIAS_STATUS_REG_BITFIELD    bitField;
    } BIAS_STATUS_REG_UNION;






    //! BIAS DAC1 register bitfield (24-bit+8 -> 32-bit)
    /*! The BIAS DAC1 register contains the current word to be sent to the DAC1:
        \param  data            an unsigned long    :14
        \param  inputRegister   an unsigned long    :2
        \param  channel         an unsigned long    :4
        \param  readWrite       an unsigned long    :1
        \param  toggleABSelect  an unsigned long    :1 */
    typedef struct {
        //! Data filed
        /*! This 14-bit fields contains the actual data in unsigned format
            (unsigned int). */
        unsigned long   data            :14;
        //! Register selector
        /*! This 2-bit fields is used to select the internal register of the
            addressed DAC. There are 4 different registers:
                - 0 -> Special function register
                - 1 -> Gain register
                - 2 -> Offset register
                - 3 -> Input data register */
        unsigned long   inputRegister   :2;
        //! Channel selector
        /*! This 4-bit field is useed to address the desired channel. */
        unsigned long   channel         :4;
        /* The following two bits are rewuired by hardware. */
        unsigned long                   :2;
        //! Read/Write mode
        /*! This 1-bit field toggles the read/_write mode. Only the write mode
            is implemented in the hardware. The field is left for information
            purposes. */
        unsigned long   readWrite       :1;
        //! Toggle mode _A/B register select
        /*! This 1-bit field is used when toggle mode is enabled in the
            addressed device. */
        unsigned long   toggleABSelect  :1;
        /* 8 extra bit to to a fill it up */
        unsigned long                   :8;
    } BIAS_DAC1_REG_BITFIELD;

    //! BIAS DAC1 register
    /*! The BIAS DAC1 register union allows to address the status register either
        as an array of 2 ints or as a bitfield structure.
        \param  integer[]   an int
        \param  bitField    a BIAS_DAC1_REG_BITFIELD */
    typedef union {
        //! Status register in array of integer format
        /*! The BIAS status register in this format is ready to be read as data
            from the addressed destination device. */
        int                         integer[BIAS_DAC1_WORD_SIZE];
        //! Status register in bit fields format
        /*! The BIAS status register in this format is very easy to update. */
        BIAS_DAC1_REG_BITFIELD      bitField;
    } BIAS_DAC1_REG_UNION;






    //! BIAS DAC2 register bitfield (24-bit+8 -> 32-bit)
    /*! The BIAS DAC2 register contains the current word to be sent to the DAC2:
        \param  data            an unsigned long    :16
        \param  quickLoad       an unsigned long    :1
        \param  inputRegister   an unsigned long    :2 */
    typedef struct {
        //! Data field
        /*! This 16-bit field contains the actual data in signed format
            (2's complement). */
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
        //! Register selector
        /*! This 2-bit field determins which register in the DAC2 is addressed:
                - 0 -> Register A
                - 1 -> Register B
                - 2 -> Register C
                - 3 -> Register D */
        unsigned long   inputRegister   :2;
        /* 8 extra bit to fill it up */
        unsigned long                   :8;
    } BIAS_DAC2_REG_BITFIELD;

    //! BIAS DAC2 register
    /*! The BIAS DAC2 register union allows to address the status register either
        as an array of 2 ints or as a bitfield structure.
        \param  integer[]   an int
        \param  bitField    a BIAS_DAC2_REG_BITFIELD */
    typedef union {
        //! DAC2 register in array of integer format
        /*! The BIAS DAC2 register in this format is ready to be read as data
            from the addressed destination device. */
        int                         integer[BIAS_DAC2_WORD_SIZE];
        //! DAC2 register in bit fields format
        /*! The BIAS DAC register in this format is very easy to update. */
        BIAS_DAC2_REG_BITFIELD      bitField;
    } BIAS_DAC2_REG_UNION;





    //! BIAS registers
    /*! This structure contains the current state of the BIAS registers.
        \param aReg     a BIAS_AREG_UNION
        \param bReg     a BIAS_BREG_UNION
        \param stateReg a BIAS_STATUS_REG_UNION
        \param dac1Reg  a BIAS_DAC1_REG_UNION
        \param dac2Reg  a BIAS_DAC2_REG_UNION
        \param adcData  an int */
    typedef struct {
        //! AREG
        /*! The AREG defines which monitor point is currently selected (Write
            only register). */
        BIAS_AREG_UNION         aReg;
        //! BREG
        /*! The BREG defines the current state of  several submodules (Write
            only register). */
        BIAS_BREG_UNION         bReg;
        //! Status Register
        /*! The status register contains the current state of DACs and ADCs
            (Read only register). */
        BIAS_STATUS_REG_UNION   statusReg;
        //! DAC1 register
        /*! The DAC1 register contains the message to be sent to the DAC1 (Write
            only register). */
        BIAS_DAC1_REG_UNION     dac1Reg;
        //! DAC2 register
        /*! The DAC2 register contains the message to be sent to the DAC2 (Write
            only register). */
        BIAS_DAC2_REG_UNION     dac2Reg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. */
        int                     adcData;
    } BIAS_REGISTERS;


    /* Globals */
    /* Externs */
    extern BIAS_REGISTERS biasRegisters[CARTRIDGES_NUMBER]; //!< Bias Registers

    /* Prototypes */
    /* Statics */
    static int getBiasAnalogMonitor(void); // Perform core analog monitor functions
    static float temperatureConversion(float voltage); // Perform voltage to temperature conversion

    /* Externs */
    extern int getSisMixerBias(unsigned char current); //!< This function monitors the SIS mixer bias
    extern int setSisMixerBias(void); //!< This function control the SIS mixer bias
    extern int setSisMixerLoop(unsigned char biasMode); //!< This function sets the SIS mixer bias mode
    extern int getSisMagnetBias(unsigned char current); //!< This function monitors the SIS magnet bias
    extern int setSisMagnetBias(void); //!< This function control the SIS magnet bias
    extern int setLnaBiasEnable(unsigned char enable); //!< This function enables/disables the LNA bias
    extern int getLnaStage(void); //!< This function monitors the LNA stage conditions
    extern int setLnaStage(void); //!< This function controls the LNA stage conditions
    extern int setLnaLedEnable(unsigned char enable); //!< This function enables/disable the LNA led
    extern int setSisHeaterEnable(unsigned char enable); //!< This function enables/disable the SIS mixers heater
    extern int getSisHeater(void); //!< This function monitors the SIS heater bias
    extern int setBiasDacStrobe(void); //!< This function sends the desired strobe to the DACs
    extern int getTemp(unsigned char polarization,
                       unsigned char sensor); //!< This function monitors the selected temperature sensor
#endif /* _BIASSERIALINTERFACE_H */
