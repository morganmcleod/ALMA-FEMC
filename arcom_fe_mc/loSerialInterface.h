/*! \file       loSerialInterface.h
    \ingroup    serialInterface
    \brief      LO serial interface header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: loSerialInterface.h,v 1.33 2010/03/03 15:43:18 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the LO serial interface. */

/*! \defgroup   loSerialInterface  Local Oscillator serial interface
    \ingroup    serialInterface
    \brief      Local Oscillator System Serial Interface
    \note       The \ref loSerialInterface module doesn't include any submodule.

    For more information on this module see \ref loSerialInterface.h */

#ifndef _LOSERIALINTERFACE_H
    #define _LOSERIALINTERFACE_H

    /* Extra includes */
    /* CARTRIDGE defines */
    #ifndef _CARTRIDGE_H
        #include "cartridge.h"
    #endif  /* _CARTRIDGE_H */

    /* Defines */
    /* General */
    #define LO_AREG                             0
    #define LO_BREG                             1

    #define PHOTOMIXER_ENABLE                   1
    #define PHOTOMIXER_DISABLE                  0
    #define PHOTOMIXER_BIAS_V                   0
    #define PHOTOMIXER_BIAS_C                   1

    #define PLL_LOCK_DETECT_VOLTAGE             0
    #define PLL_CORRECTION_VOLTAGE              1
    #define PLL_ASSEMBLY_TEMP                   2
    #define PLL_YIG_HEATER_CURRENT              3
    #define PLL_REF_TOTAL_POWER                 4
    #define PLL_IF_TOTAL_POWER                  5
    #define PLL_UNLOCK_DETECT_LATCH_OPERATE     1
    #define PLL_UNLOCK_DETECT_LATCH_CLEAR       0
    #define PLL_LOOP_BANDWIDTH_ALTERNATE        1
    #define PLL_LOOP_BANDWIDTH_DEFAULT          0
    #define PLL_LOOP_BANDWIDTH_UNDEFINED        9
    #define PLL_SIDEBAND_LOCK_POLARITY_USB      1
    #define PLL_SIDEBAND_LOCK_POLARITY_LSB      0
    #define PLL_NULL_LOOP_INTEGRATOR_NULL       1
    #define PLL_NULL_LOOP_INTEGRATOR_OPERATE    0

    #define AMC_GATE_A_VOLTAGE                  0
    #define AMC_DRAIN_A_VOLTAGE                 1
    #define AMC_DRAIN_A_CURRENT                 2
    #define AMC_GATE_B_VOLTAGE                  3
    #define AMC_DRAIN_B_VOLTAGE                 4
    #define AMC_DRAIN_B_CURRENT                 5
    #define AMC_5V_SUPPLY_VOLTAGE               6
    #define AMC_MULTIPLIER_D_CURRENT            7
    #define AMC_MULTIPLIER_D_VOLTAGE            7
    #define AMC_GATE_E_VOLTAGE                  8
    #define AMC_DRAIN_E_VOLTAGE                 9
    #define AMC_DRAIN_E_CURRENT                 10

    #define PA_3V_SUPPLY_VOLTAGE                0
    #define PA_5V_SUPPLY_VOLTAGE                1

    #define PA_CHANNEL_GATE_VOLTAGE             0
    #define PA_CHANNEL_DRAIN_VOLTAGE            1
    #define PA_CHANNEL_DRAIN_CURRENT            2





    /* Command words:
       - Rg is the register */
    // 0x00-0x02 -> unused
    #define LO_ADC_CONVERT_STROBE  0x03
    #define LO_PA_DATA_WRITE       0x04
    #define LO_AMC_DATA_WRITE      0x05
    #define LO_ADC_DATA_READ       0x06
    #define LO_10MHZ_MODE          0x07
    #define LO_PARALLEL_WRITE(Rg)  (Rg+0x08)
    #define LO_PARALLEL_READ       0x0A
    // 0x0B-0x1F -> unused




    /* --- AREG definition (12-bit) --- */
    /* Write only: 12-bit + 3 bits for latching.
       AREG is the register that contains the YTO coarse tune. */
    #define LO_AREG_SIZE            15
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define LO_AREG_SHIFT_SIZE      3
    #define LO_AREG_SHIFT_DIR       SHIFT_LEFT
    /* YTO Coarse tune (bit 11-0) */





    /* --- BREG definitions (12-bit) --- */
    /* Write Only: 12-bit + 2 bits for latching.
       BREG is the register that contains the currently selected monitor point
       and the state of several LO subsystems. */
    #define LO_BREG_SIZE            14
    /* This register has to be shifted left before being sent out for alignment.
       This is because of extra bits the hardware needs to latch the data. */
    #define LO_BREG_SHIFT_SIZE      2
    #define LO_BREG_SHIFT_DIR       SHIFT_LEFT
    // bit 11 -> Photomixer bias
    // bit 10-9 -> Unused
    // bit 8 -> Clear LO unlock detect latch (LULOCK)
    // bit 7 -> PLL loop bandwidth select (BWSEL)
    // bit 6 -> Sideband lock polarity select (POL)
    // bit 5 -> Null PLL loop integrator (ZERO)
    /* bit 4-0 -> Monitor Points:
    - Cu is the current/voltage selector:
        - 0 -> Voltage
        - 1 -> Current
    - PLL_Mp is the PLL monitor point:
        - 0 -> Lock detect voltage
        - 1 -> Correction voltage
        - 2 -> Assembly temperature
        - 3 -> Supply voltage
        - 4 -> REF total power
        - 5 -> IF total power
    - AMC_Mp is the AMC monitor point:
        - 0 -> A Gate Voltage
        - 1 -> A Drain Voltage
        - 2 -> A Drain Current
        - 3 -> B Gate Voltage
        - 4 -> B Drain Voltage
        - 5 -> B Drain Current
        - 6 -> 5V Supply Voltage
        - 7 -> D Multiplier Current
        - 8 -> E Gate Voltage
        - 9 -> E Drain Voltage
        - 10 -> E Drain Current
        - 14 -> 3V Supply Voltage
    - PA_Mp is the PA monitor point
        - 0 -> 3V Supply Voltage
        - 1 -> 5V Supply Voltage
    - Pt is the port
        - 0 -> Gate Voltage
        - 1 -> Drain Voltage
        - 2 -> Drain Current */
    #define LO_BREG_PHOTOMIXER(Cu)      (Cu)
    #define LO_BREG_PLL(PLL_Mp)         (PLL_Mp+0x02)
    #define LO_BREG_AMC(AMC_Mp)         (AMC_Mp+0x10)
    #define LO_BREG_PA(PA_Mp)           (0x0C*PA_Mp+0x0F)
    #define LO_BREG_PA_CHANNEL(Pt)      (0x03*currentPaChannel()+Pt+0x08)








    /* --- 10 MHz Mode command --- */
    #define LO_10MHZ_MODE_SIZE          0
    #define LO_10MHZ_MODE_SHIFT_SIZE    NO_SHIFT
    #define LO_10MHZ_MODE_SHIFT_DIR     NO_SHIFT



    /* --- Status register definitions (4-bit) --- */
    /* Read only: 4-bit */
    #define LO_STATUS_REG_SIZE          4
    #define LO_STATUS_REG_SHIFT_SIZE    NO_SHIFT
    #define LO_STATUS_REG_SHIFT_DIR     NO_SHIFT




    /* --- ADC definitions (16-bit) --- */
    #define LO_ADC_STROBE_SIZE              3           // Bit size of the LO ADC convert strobe
    #define LO_ADC_STROBE_SHIFT_SIZE        NO_SHIFT    // The strobe outgoing data is not shifted
    #define LO_ADC_STROBE_SHIFT_DIR         NO_SHIFT    // The strobe outgoing data is not shifted
    #define LO_ADC_RANGE                    65536       // Full ADC range
    #define LO_ADC_DATA_SIZE                18          // Read only: 2 for internal use + 16-bit
    #define LO_ADC_DATA_SHIFT_SIZE          NO_SHIFT    // The incoming ADC data is not shifted
    #define LO_ADC_DATA_SHIFT_DIR           NO_SHIFT    // The incoming ADC data is not shifted
    #define LO_ADC_PHOTOMIXER_BIAS_V_SCALE  (-10.0)     // Scale factor for PHOTOMIXER voltage
    #define LO_ADC_PHOTOMIXER_BIAS_C_SCALE  (-10.0)     // Scale factor for PHOTOMIXER current
    #define LO_GATE_DRAIN_V_SCALE           10.0        // Scale factor for any gate or drain voltage
    #define LO_DRAIN_C_SCALE                1000.0      // Scale factor for any drain current
    #define LO_PLL_ASSEMBLY_TEMP_SCALE      1000.0      // Scale factor for the PLL assembly temperature
    #define LO_PLL_TTL_PWR_SCALE            10.0        // Scale factor for the PLL any total power
    #define LO_LOCK_DETECT_V_SCALE          (210.0 / 11.0) // Scale factor for lock detect voltage
    #define LO_PLL_CORRECTION_V_SCALE       (260.0 / 11.0) // Scale factor for pll correction voltage
    #define LO_AMC_MULTIPLIER_I_SCALE       100.0       // Scale factor for AMC multiplier currents
    #define LO_SUPPLY_V_SCALE               20.0        // Scale factor for LO supply voltages
    #define LO_YIG_HEATER_I_SCALE           400.0       // Scale factor for PLL YIG heater current
    #define LO_YIG_HEATER_I_OFFSET          150.0       // Scale factor for PLL YIG heater current
    #define LO_ADC_BUSY                     0           // Busy state signal

    /* --- Common POT definitions (64-bit) --- */
    #define LO_POT_WORD_SIZE                4   // 4 ints necessary to contain the POT message
    #define POT0                            0
    #define POT1                            1
    #define POT2                            2
    #define POT3                            3


    /* --- AMC POT definitions */
    #define LO_AMC_POT_STACK_BIT_0              0   // Stack bit configuration determineed by hardware design
    #define LO_AMC_POT_DATA_SIZE                35  // Write only: 34-bit + 1 for internal use
    #define LO_AMC_POT_DATA_SHIFT_SIZE          1   // 1 extra bit to be sent
    #define LO_AMC_POT_DATA_SHIFT_DIR           SHIFT_LEFT // The data has to be shifted left
    #define LO_AMC_POT_DRAIN_B_V_SCALE(v)       (unsigned char)(51.0*v) // Scale factor for AMC drain B voltage
    #define LO_AMC_POT_GATE_E_V_SCALE(v)        (unsigned char)(0.5+(v==0.15?0.0:((127.5*(20.0*v+17.0-5.0*sqrt(80.0*v*v+8.0*v+13.0)))/(20.0*v-3.0)))) // Scale factor for AMC gate E voltage
    #define LO_AMC_POT_DRAIN_E_V_SCALE(v)       (unsigned char)(102.0*v) // Scale factor for AMC drain E voltage


    /* --- PA POT definitions */
    #define LO_PA_POT_STACK_BIT_0               0   // Stack bit configuration determineed by hardware design
    #define LO_PA_POT_GATE_V_SCALE(v)           (unsigned char)(0.5+(v==0.15?0.0:((127.5*(20.0*v+17.0-5.0*sqrt(80.0*v*v+8.0*v+13.0)))/(20.0*v-3.0)))) // Scale factor for PA_CHANNEL gate voltage
    #define LO_PA_POT_DRAIN_V_SCALE(v)          (unsigned char)(102.0*v) // Scale factor for PA_CHANNEL drain voltage
    #define LO_PA_CURRENT_POT(Ch)               (2*currentPaChannel()+Ch)
    #define LO_PA_POT_DATA_SIZE                 35  // Write only: 34-bit + 1 for internal use
    #define LO_PA_POT_DATA_SHIFT_SIZE           1   // 1 extra bit to be sent
    #define LO_PA_POT_DATA_SHIFT_DIR            SHIFT_LEFT // The data has to be shifted left



    /* Typedefs */
    /* LO Registers */
    //! LO AREG bitfield (12-bit+4 -> 16-bit)
    /*! The AREG bitfield defined the YTO coarse tuning.
        \param ytoCoarseTune   an unsigned int :12 */
    typedef struct {
        //! YTO coarse tune
        /*! This is the coarse tune word for the YTO. The range is from 0 to
            4095 */
        unsigned int ytoCoarseTune :12;
        /* 4 extra bit to fill it up */
        unsigned int               :4;
    } LO_AREG_BITFIELD;

    //! LO AREG
    /*! The AREG union allows to address the AREG either as a single unsigned
        integer of as a bitfield structure.
        \param integer      an int
        \param bitField     a LO_AREG_BITFIELD */
    typedef union {
        //! AREG in integer format
        /*! The LO AREG in this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! AREG in bitfield format.
        /*! The LO AREG in this format is very easy to update. */
        LO_AREG_BITFIELD    bitField;
    } LO_AREG_UNION;




    //! LO BREG bitfield (12-bit+4 -> 16-bit)
    /*! The BREG defines the current state of several submodules and which
        monitor point is currently selected.
        \param  monitorPoint                an unsigned int     :5
        \param  nullLoopIntegrator          an unsigned int     :1
        \param  sidebandLockPolaritySelect  an unsigned int     :1
        \param  loopBandwidthSelect         an unsigned int     :1
        \param  clearUnlockDetectLatch      an unsigned int     :1
        \param  photomixerBiasEnable        an unsigned int     :1 */
    typedef struct {
        //! Selected monitor point (bit 4-0)
        /*! This bit field defines the currently selected monitor point:
                - 00000 -> Photomixer Bias Voltage
                - 00001 -> Photomixer Bias Current
                - 00010 -> PLL Lock Detector Voltage
                - 00011 -> PLL Correction Voltage
                - 00100 -> PLL Assembly Temperature
                - 00101 -> PLL +5V Supply Voltage
                - 00110 -> PLL REF Total Power
                - 00111 -> PLL IF Total Power
                - 01000 -> PA Gate A Voltage
                - 01001 -> PA Drain A Voltage
                - 01010 -> PA Drain A Current
                - 01011 -> PA Gate B Voltage
                - 01100 -> PA Drain B Voltage
                - 01101 -> PA Drain B Current
                - 01110 -> Unused
                - 01111 -> PA -3V Supply Voltage
                - 10000 -> AMC Gate A Voltage
                - 10001 -> AMC Drain A Voltage
                - 10010 -> AMC Drain A Current
                - 10011 -> AMC Gate B Voltage
                - 10100 -> AMC Drain B Voltage
                - 10101 -> AMC Drain B Current
                - 10110 -> AMC +5V Supply Voltage
                - 10111 -> AMC Multiplier D Current
                - 11000 -> AMC Gate E Voltage
                - 11001 -> AMC Drain E Voltage
                - 11010 -> AMC Drain E Current
                - 11011 -> PA +5V Supply Voltage
                - 11100 -> Unused
                - 11101 -> Unused
                - 11110 -> Unused
                - 11111 -> Unused */
        unsigned int monitorPoint               :5;
        //! PLL null loop integrator (bit 5)
        /*! This bit field defines the current state of the PLL null loop
            integrator:
                - 0 -> Operate
                - 1 -> Null/Zero */
        unsigned int nullLoopIntegrator      :1;
        //! PLL sideband lock polarity (bit 6)
        /*! This bit field defines the current selection of the PLL sideband
            lock polarity:
                - 0 -> Lower (LSB)
                - 1 -> Upper (USB) */
        unsigned int sidebandLockPolaritySelect :1;
        //! PLL loop bandwidth select (bit 7)
        /*! This bit field defines the current selection of the PLL loop
            bandwidth:
                - 0 -> Default
                - 1 -> Alternate */
        unsigned int loopBandwidthSelect        :1;
        //! PLL clear unlock detect latch (bit 8)
        /*! This bit field defines the current state of the PLL clear unlock
            detect latch:
                - 0 -> Clear
                - 1 -> Operate */
        unsigned int clearUnlockDetectLatch     :1;
        /* bit 10-9 are unused */
        unsigned int                            :2;
        //! Photomixer bias enable (bit 11)
        /*! This bit field defines the current state of the photomixer:
                - 0 -> Disable/OFF
                - 1 -> Enable/ON */
        unsigned int photomixerBiasEnable       :1;
        /* 4 extra bit to fill it up */
        unsigned int                            :4;
    } LO_BREG_BITFIELD;

    //! LO BREG
    /*! The BREG union allows to address the BREG either as a single integer or
        as a bitfield structure.
        \param  integer     an int
        \param  bitField    a LO_BREG_BITFIELD */
    typedef union {
        //! BREG in integer format
        /*! The LO BREG in this format is ready to be sent as data to the
            destination device. */
        int     integer;
        //! BREG in bit fields format
        /*! The LO BREG in this format is very easy to update. */
        LO_BREG_BITFIELD    bitField;
    } LO_BREG_UNION;



    //! LO status register bitfield (4-bit+12 -> 16-bit)
    /*! The LO status register defines the current state of several hardware
        within the LO:
        \param refIfState       an unsigned int     :1
        \param pllLockDetect    an unsigned int     :1
        \param adcReady         an unsigned int     :1 */
    typedef struct {
        /* bit 0 of the LO status register is unused */
        unsigned int                :1;
        //! REF/IF status bit
        /*! This 1-bit field returns the current status of the REF/IF:
                - 0 -> Falut detected
                - 1 -> OK */
        unsigned int refIfState     :1;
        //! PLL lock/unlock detect bit (this is latched in hardware)
        /*! This 1-bit field returns the current status of the PLL
                - 0 -> PLL lock OK
                - 1 -> Unlock condition detected and latched by hardware */
        unsigned int pllLockDetect  :1;
        //! ADC Ready bit
        /*! This 1-bit field return the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady       :1;
        /* 12 extra bit to fill it up */
        unsigned int                :12;
    } LO_STATUS_REG_BITFIELD;

    //! LO status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param integer      an int
        \param bitField     a LO_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The LO status register in this format is ready to be read as data
            from the addressed destination device. */
        int                         integer;
        //! Status register in bit fields format
        /*! The LO status register in this format is very easy to update. */
        LO_STATUS_REG_BITFIELD      bitField;
    } LO_STATUS_REG_UNION;




    //! LO pots register bitfield (34-bit+30 -> 64-bit)
    /*! This register contains the current word to be sent to the LO digital
        pots.
        \param pot0         an unsigned long long   :8
        \param pot1         an unsigned long long   :8
        \param stackBit10   an unsigned long long   :8
        \param pot2         an unsigned long long   :8
        \param pot3         an unsigned long long   :8
        \param stackBit32   an unsigned long long   :8 */
    typedef struct {
        //! Pot0
        /*! This contains the current state of pot0 */
        unsigned long long  pot0        :8;
        //! Pot1
        /*! This contains the current state of pot1 */
        unsigned long long  pot1        :8;
        //! Stack bit 1&0
        /*! This is an hardware configuration bit always set to 0 because of the
            actual hardware implementation. */
        unsigned long long  stackBit10  :1;
        //! Pot2
        /*! This contains the current state of pot2 */
        unsigned long long  pot2        :8;
        //! Pot3
        /*! This contains the current state of pot3 */
        unsigned long long  pot3        :8;
        //! Stack bit 3&2
        /*! This is an hardware configuration bit always set to 0 because of the
            actual hardware implementation. */
        unsigned long long  stackBit32  :1;
        /* 30 extra bit to fill it up */
        unsigned long long              :30;
    } LO_POT_REG_BITFIELD;

    //! LO pot register
    /*! The LO pot register union allows to address the pot registers either
        as an array of 4 ints or as a bitfield structure.
        \param  integer[]   an int
        \param  bitField    a LO_POT_REG_BITFIELD */
    typedef union {
        //! Pot register in array of integer format
        /*! The LO pot register in this format is ready to be read as data from
            the addressed destination device. */
        int                     integer[LO_POT_WORD_SIZE];
        LO_POT_REG_BITFIELD     bitField;
    } LO_POT_REG_UNION;




    //! LO registers */
    /*! This structure contains the current state of the LO registers.
        \param areg         a LO_AREG_UNION
        \param breg         a LO_BREG_UNION
        \param statusReg    a LO_STATUS_REG_UNION
        \param amcPotReg    a LO_POT_REG_UNION
        \param paPotReg     a LO_POT_REG_UNION
        \param adcData      an int */
    typedef struct {
        //! AREG
        /*! The AREG defines the current YTO coarse tune state (Write only
            register). */
        LO_AREG_UNION           aReg;
        //! BREG
        /*! The BREG defines which monitor point is currently selected and the
            current state of several submodules (Write only register). */
        LO_BREG_UNION           bReg;
        //! Status Registers
        /*! The status register contains the current state of several hardware
            subsystems of the LO */
        LO_STATUS_REG_UNION     statusReg;
        //! AMC pot register
        /*! The AMC pot register defines the current condition of the pots in
            the AMC system. */
        LO_POT_REG_UNION        amcPotReg;
        //! PA pot register
        /*! The PA pot register defines the current condition of the pots in
            the PA system. */
        LO_POT_REG_UNION        paPotReg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. */
        int                     adcData;
    } LO_REGISTERS;



    /* Globals */
    /* Externs */
    extern LO_REGISTERS loRegisters[CARTRIDGES_NUMBER]; //!< Local Oscillator Registers

    /* Prototypes */
    /* Statics */
    static int getLoAnalogMonitor(void); // Perform core analog monitor functions
    /* Externs */
    extern int setYtoCoarseTune(void); //!< This function set the YTO coarse tune
    extern int setPhotomixerEnable(unsigned char enable); //!< This function enables/disables the photomixer
    extern int getPhotomixer(unsigned char port); //!< This function monitors the photomixer bias
    extern int getPll(unsigned char port); //!< This function monitors different devices within the PLL
    extern int getPllStates(void); //!< This function monitors different states withing the PLL
    extern int setClearUnlockDetectLatch(void); //!< This function controls the operation of the clear unlock detect latch
    extern int setLoopBandwidthSelect(unsigned char bandwidth); //!< This function controls the loop bandwidth
    extern int setSidebandLockPolaritySelect(unsigned char sideband); //!< This function controls the sideband lock polarity
    extern int setNullLoopIntegrator(unsigned char state); //!< This function controls the operation of the null loop integrator
    extern int getAmc(unsigned char port); //!< This function monitors different devices within the AMC
    extern int setAmc(unsigned char port); //!< This function controls different devices within the AMC
    extern int getPa(unsigned char port); //!< This function controls different devices within the PA
    extern int getPaChannel(void); //!< This function monitors different devices within the PA channel
    extern int setPaChannel(void); //!< This function controls different devices within the PA channel
#endif /* _LOSERIALINTERFACE_H */
