/*! \file       fetimSerialInterface.h
    \ingroup    serialInterface
    \brief      FETIM serial interface header file

    <b> File informations: </b><br>
    Created: 2011/05/04 10:38:01 by avaccari

    <b> CVS informations: </b><br>
    \$Id: fetimSerialInterface.h,v 1.2 2011/11/09 00:40:30 avaccari Exp $

    This file contains all the informations necessary to define the
    characteristics and operate the FETIM serial interface. */

/*! \defgroup   fetimSerialInterface  FETIM serial interface
    \ingroup    serialInterface
    \brief      FETIM serial interface
    \note       The \ref fetimSerialInterface module doesn't include any submodule.

    For more informations on this module see \ref fetimSerialInterface.h */

#ifndef _FETIMSERIALINTERFACE_H
    #define _FETIMSERIALINTERFACE_H

    /* Defines */
    /* General */
    #define FETIM_AREG_OUT      0
    #define FETIM_BREG_OUT      1
    #define FETIM_CREG_OUT      2
    #define FETIM_DREG_OUT      3

    #define FETIM_AREG_IN       0
    #define FETIM_BREG_IN       1
    #define FETIM_CREG_IN       2

    #define FETIM_DIG_FLOW_OOR          0x00
    #define FETIM_DIG_TEMP_OOR          0x01
    #define FETIM_DIG_GLITCH_CNT        0x02
    #define FETIM_DIG_SHTDWN_TRIG       0x03
    #define FETIM_DIG_SHTDWN_DELAY      0x04
    #define FETIM_DIG_SINGLE_FAIL       0x05
    #define FETIM_DIG_MULTI_FAIL        0x06
    #define FETIM_DIG_COMP_CBL_STA      0x07
    #define FETIM_DIG_INTRLK_STA        0x08
    #define FETIM_DIG_HE2_PRESS_OOR     0x09
    #define FETIM_DIG_EXT_TEMP_OOR      0x0A

    #define N2_FILL_ENABLE              1
    #define N2_FILL_DISABLE             0

    #define FE_STATUS_SAFE              1
    #define FE_STATUS_UNSAFE            0

    #define EXT_TEMP_1                  1
    #define EXT_TEMP_2                  0

    #define NO_FETIM_HARDWARE           0x00

    /* Command words:
       - RgO is the output register
       - RgI is the input register */
    #define FETIM_PARALLEL_WRITE(RgO)   (0x00+RgO)
    #define FETIM_SER_ADC_ACCESS        0x04
    // 0x05 -> not used
    #define FETIM_PAR_ADC_CONV_STROBE   0x06
    // 0x07 -> not used
    #define FETIM_PARALLEL_READ(RgI)    (0x08+RgI)
    // 0x0B->0x1F -> not used







    /* --- AREG_OUT definitions (3-bit) --- */
    /* Write Only: 3-bit + 2 bit for latching.
       AREG_OUT is the register that contains the monitor point selection for the
       parallel ADC. */
    #define FETIM_AREG_OUT_SIZE             5
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define FETIM_AREG_OUT_SHIFT_SIZE       2
    #define FETIM_AREG_OUT_SHIFT_DIR        SHIFT_LEFT
    /* bit 2-0 -> Monitor points:
       - Tf is the temperature sensor
       - Af is the airflow sensor */
    #define FETIM_AREG_OUT_TEMPERATURE(Tf)  (Tf)
    #define FETIM_AREG_OUT_GLITCH_VALUE     0x05
    #define FETIM_AREG_OUT_FLOW(Af)         (0x06+Af)



    /* --- BREG_OUT definitions (2-bit) --- */
    /* Write Only: 2-bit + 2 bit for latching.
       BREG_OUT is the register that contains the monitor point selection for the
       serial ADC. */
    #define FETIM_BREG_OUT_SIZE             4
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define FETIM_BREG_OUT_SHIFT_SIZE       2
    #define FETIM_BREG_OUT_SHIFT_DIR        SHIFT_LEFT
    /* bit 1-0 -> Monitor points:
       - Tc is the temperature sensor */
    #define FETIM_BREG_OUT_TEMPERATURE(Tc)  (Tc)
    #define FETIM_BREG_OUT_HE2_PRESS        0x02



    /* --- CREG_OUT definitions (1-bit) --- */
    /* Write Only: 1-bit + 2 bit for latching.
       CREG_OUT is the register that control the N2 fill tank system. */
    #define FETIM_CREG_OUT_SIZE             3
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define FETIM_CREG_OUT_SHIFT_SIZE       2
    #define FETIM_CREG_OUT_SHIFT_DIR        SHIFT_LEFT
    /* bit 0 -> Control point */
    #define FETIM_CREG_OUT_N2_FILL          0x00





    /* --- DREG_OUT definitions (1-bit) --- */
    /* Write Only: 1-bit + 2 bit for latching.
       CREG is the register that control the FE safe status bit. */
    #define FETIM_DREG_OUT_SIZE             3
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bits the hardware need to latch the data. */
    #define FETIM_DREG_OUT_SHIFT_SIZE       2
    #define FETIM_DREG_OUT_SHIFT_DIR        SHIFT_LEFT
    /* bit 0 -> Control point */
    #define FETIM_DREG_OUT_FE_STATUS        0x00




    /* --- BREG_IN definitions (12-bit) --- */
    /* Read only: 12-bit.
       BREG_IN handles the digital data */
    #define FETIM_BREG_IN_SIZE          12
    #define FETIM_BREG_IN_SHIFT_SIZE    NO_SHIFT
    #define FETIM_BREG_IN_SHIFT_DIR     NO_SHIFT






    /* --- CREG_IN definitions (5-bit) --- */
    /* Read only: 5-bit.
       CREG_IN handles the fimrware revision data */
    #define FETIM_CREG_IN_SIZE          5
    #define FETIM_CREG_IN_SHIFT_SIZE    NO_SHIFT
    #define FETIM_CREG_IN_SHIFT_DIR     NO_SHIFT



    /* --- Parallel ADC definitions (8-bit) --- */
    #define FETIM_PAR_ADC_STROBE_SIZE           6           // Bit size of the parallel ADC convert strobe
    #define FETIM_PAR_ADC_STROBE_SHIFT_SIZE     NO_SHIFT    // The strobe outgoing data is not shifted
    #define FETIM_PAR_ADC_STROBE_SHIFT_DIR      NO_SHIFT    // The strobe outgoing data is not shifted
    #define FETIM_PAR_ADC_DATA_MASK             0x00FF      // Mask to extract the parallel ADC data from the incoming payload
    #define FETIM_PAR_ADC_RANGE                 256         // Full parallel ADC range
    #define FETIM_PAR_ADC_DATA_SIZE             8           // Read only
    #define FETIM_PAR_ADC_DATA_SHIFT_SIZE       NO_SHIFT    // The incoming data is not shifted
    #define FETIM_PAR_ADC_DATA_SHIFT_DIR        NO_SHIFT    // The incoming data is not shifted
    #define FETIM_PAR_ADC_TEMP_OFFSET           0.0         // Offset factor for the inside temperature sensors
    #define FETIM_PAR_ADC_TEMP_SCALE            60.0        // Scale factor for the inside temperature sensors
    #define FETIM_PAR_ADC_GLITCH_OFFSET         (-100.0)    // Offset factor for the glitch counter
    #define FETIM_PAR_ADC_GLITCH_SCALE          100.0       // Scale factor for the glitch counter
    #define FETIM_PAR_ADC_FLOW_SCALE            2.0         // Scale factor for the airflow sensors



    /* --- Serial ADC definitions (12-bit) --- */
    #define FETIM_SER_ADC_RANGE                 4096        // Full parallel ADC range
    #define FETIM_SER_ADC_DATA_SIZE             17          // Read only: 1 extra + 4 zeros followed by 12-bit data
    #define FETIM_SER_ADC_DATA_SHIFT_SIZE       NO_SHIFT    // The incoming data is not shifted
    #define FETIM_SER_ADC_DATA_SHIFT_DIR        NO_SHIFT    // The incoming data is not shifted
    #define FETIM_SER_ADC_DATA_MASK             0x0FFF      // Mask to extract the serial ADC data from the incoming payload
    #define FETIM_SER_ADC_TEMP_OFFSET           0.0         // Offset factor for the outside temperature sensors
    #define FETIM_SER_ADC_TEMP_SCALE            120.0       // Scale factor for the outside temperature sensors
    #define FETIM_SER_ADC_HE2_PRESS_OFFSET      (-1.0)      // Offset factor for the He2 pressure sensor
    #define FETIM_SER_ADC_HE2_PRESS_SCALE       5.0         // Scale factor for the He2 pressure sensor



    //! FETIM AREG_OUT bitfield (3-bit+13 -> 16-bit)
    /*! The AREG_OUT bitfield defines the desired analog monitor point for the
        parallel ADC:
        \param monitorPoint             an unsigned int         :3 */
    typedef struct {
        //! Selected monitor point (bit 2-0)
        /*! This bitfield defines the currently selected monitor point:
                - 000       interlock internal temperature 1
                - 001       interlock internal temperature 2
                - 010       interlock internal temperature 3
                - 011       interlock internal temperature 4
                - 100       interlock internal temperature 5
                - 101       interlock glitch counter value
                - 110       interlock internal airflow 1
                - 111       interlock internal airflow 2 */
        unsigned int monitorPoint           :3;
        /* 13 extra bit to fill it up */
        unsigned int                        :13;
    } FETIM_AREG_OUT_BITFIELD;

    //! FETIM AREG_OUT
    /*! The AREG_OUT union allows to address the AREG_OUT either as a single integer
        or as a bitfield structure.
        \param integer      an int
        \param bitField     a FETIM_AREG_OUT_BITFIELD */
    typedef union {
        //! AREG_OUT in integer format
        /*! The FETIM AREG_OUT in this format is ready to be sent as data to the
            destination device. */
        int                     integer;
        //! AREG_OUT in bit field format
        /*! The FETIM AREG_OUT in this format is very easy to update. */
        FETIM_AREG_OUT_BITFIELD bitField;
    } FETIM_AREG_OUT_UNION;




    //! FETIM BREG_OUT bitfield (2-bit+14 -> 16-bit)
    /*! The BREG_OUT bitfield defines the desired analog monitor point for the
        serial ADC:
        \param monitorPoint             an unsigned int         :2 */
    typedef struct {
        //! Selected monitor point (bit 1-0)
        /*! This bitfield defines the currently selected monitor point:
                - 00        compressor external temperature 1
                - 01        compressor external temperature 2
                - 10        compressor He2 thanks pressure */
        unsigned int monitorPoint           :2;
        /* 14 extra bit to fill it up */
        unsigned int                        :14;
    } FETIM_BREG_OUT_BITFIELD;

    //! FETIM BREG_OUT
    /*! The BREG_OUT union allows to address the BREG_OUT either as a single integer
        or as a bitfield structure.
        \param integer      an int
        \param bitField     a FETIM_BREG_OUT_BITFIELD */
    typedef union {
        //! BREG_OUT in integer format
        /*! The FETIM BREG_OUT in this format is ready to be sent as data to the
            destination device. */
        int                     integer;
        //! BREG_OUT in bit field format
        /*! The FETIM BREG_OUT in this format is very easy to update. */
        FETIM_BREG_OUT_BITFIELD bitField;
    } FETIM_BREG_OUT_UNION;







    //! FETIM CREG_OUT bitfield (1-bit+15 -> 16-bit)
    /*! The CREG_OUT bitfield controls the state of the N2 fill system:
        \param n2Fill                   an unsigned int         :1 */
    typedef struct {
        //! Selected monitor point (bit 0)
        /*! This bitfield defines the addressed hardware:
                - 0             N2 fill system */
        unsigned int n2Fill                 :1;
        /* 15 extra bit to fill it up */
        unsigned int                        :15;
    } FETIM_CREG_OUT_BITFIELD;

    //! FETIM CREG_OUT
    /*! The CREG_OUT union allows to address the CREG_OUT either as a single integer
        or as a bitfield structure.
        \param integer      an int
        \param bitField     a FETIM_CREG_OUT_BITFIELD */
    typedef union {
        //! CREG_OUT in integer format
        /*! The FETIM CREG_OUT in this format is ready to be sent as data to the
            destination device. */
        int                     integer;
        //! CREG_OUT in bit field format
        /*! The FETIM CREG_OUT in this format is very easy to update. */
        FETIM_CREG_OUT_BITFIELD bitField;
    } FETIM_CREG_OUT_UNION;







    //! FETIM DREG_OUT bitfield (1-bit+15 -> 16-bit)
    /*! The CREG_OUT bitfield controls the state of the FE safe status signal:
        \param feStatus                 an unsigned int         :1 */
    typedef struct {
        //! Selected monitor point (bit 0)
        /*! This bitfield defines the addressed hardware:
                - 0             FE Status */
        unsigned int feStatus               :1;
        /* 15 extra bit to fill it up */
        unsigned int                        :15;
    } FETIM_DREG_OUT_BITFIELD;

    //! FETIM DREG_OUT
    /*! The DREG_OUT union allows to address the DREG_OUT either as a single integer
        or as a bitfield structure.
        \param integer      an int
        \param bitField     a FETIM_DREG_OUT_BITFIELD */
    typedef union {
        //! DREG_OUT in integer format
        /*! The FETIM DREG_OUT in this format is ready to be sent as data to the
            destination device. */
        int                     integer;
        //! DREG_OUT in bit field format
        /*! The FETIM DREG_OUT in this format is very easy to update. */
        FETIM_DREG_OUT_BITFIELD bitField;
    } FETIM_DREG_OUT_UNION;




    //! FETIM BREG_IN register bitfield (12-bit+4 -> 16-bit)
    /*! The FETIM BREG_IN register defines the current state of several hardware
        within the FETIM:
        \param intrlkFlowOutRng         an unsigned int     :1
        \param intrlkTempOutRng         an unsigned int     :1
        \param glitchCntTrig            an unsigned int     :1
        \param shutdownTrig             an unsigned int     :1
        \param shutdownDelayTrig        an unsigned int     :1
        \param singleFail               an unsigned int     :1
        \param multiFail                an unsigned int     :1
        \param compCableStatus          an unsigned int     :1
        \param compIntrlkStatus         an unsigned int     :1
        \param he2PressOutRng           an unsigned int     :1
        \param compExtTemp1OutRng       an unsigned int     :1
        \param compExtTemp2OutRng       an unsigned int     :1 */
    typedef struct {
        //! Interlock flow sensor out of range
        /*! This 1-bit field returns the current status of the interlock flow
            sensors:
                - 0 -> OK
                - 1 -> At least one sensor is out of range */
        unsigned int intrlkFlowOutRng       :1;
        //! Interlock temp sensor out of range
        /*! This 1-bit field returns the current status of the interlock temp
            sensors:
                - 0 -> OK
                - 1 -> At least one sensor is out of range */
        unsigned int intrlkTempOutRng       :1;
        //! Glitch counter triggered
        /*! This 1-bit field returns the triggered status of the glitch counter:
                - 0 -> OK
                - 1 -> triggered */
        unsigned int glitchCntTrig          :1;
        //! Shutdown sequence triggered
        /*! This 1-bit field returns the triggered status of the shutdown
            sequence:
                - 0 -> OK
                - 1 -> triggered */
        unsigned int shutdownTrig           :1;
        //! Shutdown delay triggered
        /*! This 1-bit field returns the triggered status of the shutdown
            delay:
                - 0 -> OK
                - 1 -> triggered */
        unsigned int shutdownDelayTrig      :1;
        //! Single sensor failure
        /*! This 1-bit field returns the current status of the sensors:
                - 0 -> OK
                - 1 -> At lease one sensor failed */
        unsigned int singleFail             :1;
        //! Multiple sensor failure
        /*! This 1-bit field returns the current status of the sensors:
                - 0 -> OK
                - 1 -> More than one sensor failed */
        unsigned int multiFail              :1;
        //! Compressor cable status
        /*! This 1-bit field returns the current status of the cable to the
            compressor:
                - 0 -> OK
                - 1 -> ERROR */
        unsigned int compCableStatus        :1;
        //! Compressor interlock status
        /*! This 1-bit field returns the current status of the compressor
            interlock:
                - 0 -> OK
                - 1 -> Triggered */
        unsigned int compIntrlkStatus      :1;
        //! He2 buffer tank pressure sensor out of range
        /*! This 1-bit field returns the current status of the He2 buffer tank
            pressure sensors:
                - 0 -> OK
                - 1 -> The sensor is out of range */
        unsigned int he2PressOutRng         :1;
        //! Compressor external temperature sensor 1 out of range
        /*! This 1-bit field returns the current status of the compressor
            external temperature sensor 1:
                - 0 -> OK
                - 1 -> The sensor is out of range */
        unsigned int compExtTemp1OutRng     :1;
        //! Compressor external temperature sensor 2 out of range
        /*! This 1-bit field returns the current status of the compressor
            external temperature sensor 2:
                - 0 -> OK
                - 1 -> The sensor is out of range */
        unsigned int compExtTemp2OutRng     :1;
        /* 4 extra bit to fill it up */
        unsigned int                        :4;
    } FETIM_BREG_IN_BITFIELD;

    //! FETIM BREG_IN
    /*! The BREG_IN union allows to address the BREG_IN either as a single integer
        or as a bitfield structure.
        \param integer      an int
        \param bitField     a FETIM_BREG_IN_BITFIELD */
    typedef union {
        //! BREG_IN in integer format
        /*! The FETIM BREG_IN in this format is ready to be read as data from
            the destination device. */
        int                     integer;
        //! BREG_IN in bit field format
        /*! The FETIM BREG_IN in this format is very easy to read. */
        FETIM_BREG_IN_BITFIELD  bitField;
    } FETIM_BREG_IN_UNION;







    //! FETIM CREG_IN register bitfiels (5-bit+11 -> 16-bit)
    /*! The FETIM CREG_IN register defines the current hardware revision level
        of the FETIM board:
        \param hardwRev             an unsigned int     :5 */
    typedef struct {
        //! Hardware revision level
        /*! This 5-bit field returns the current hardware revision level of the
            FETIM board */
        unsigned int hardwRev       :5;
        /* 11 extra bits to fill it up */
        unsigned int                :11;
    } FETIM_CREG_IN_BITFIELD;


    //! FETIM_CREG_IN register
    /*! The hardware revision register union allows to address the hardware
        revision register either as a single unsigned int or a bitfield
        structure.
        \param  integer         an int
        \param  bitField        a FETIM_CREG_IN_BITFIELD */
    typedef union {
        //! Hardware revision register in integer format
        /*! The FETIM hardware revision revister in this format is ready to be
            read as data from the addressed destination device. */
        int                         integer;
        //! Hardware revision register in bit field format
        /*! The FETIM hardware revision register in this format is very easy to
            update. */
        FETIM_CREG_IN_BITFIELD      bitField;
    } FETIM_CREG_IN_UNION;



    //! FETIM registers
    /*! This structure contains the current state of the FETIM registers
        \param aRegOut          a FETIM_AREG_OUT_UNION
        \param bRegOut          a FETIM_BREG_OUT_UNION
        \param cRegOut          a FETIM_CREG_OUT_UNION
        \param dRegOut          a FETIM_DREG_OUT_UNION
        \param bRegIn           a FETIM_BREG_IN_UNION
        \param cRegIn           a FETIM_CREG_IN_UNION
        \param parAdcData       an unsigned char
        \param serAdcData       an unsinged int */
    typedef struct {
        //! AREG_OUT
        /*! The AREG_OUT defines which monitor point is currently selected for
            the parallel ADC (Write only register) */
        FETIM_AREG_OUT_UNION        aRegOut;
        //! BREG_OUT
        /*! The BREG_OUT defines which monitor point is currently selected for
            the serial ADC (Write only register) */
        FETIM_BREG_OUT_UNION        bRegOut;
        //! CREG_OUT
        /*! The CREG_OUT defines the current state of the N2 fill system (Write
            only register) */
        FETIM_CREG_OUT_UNION        cRegOut;
        //! DREG_OUT
        /*! The DREG_OUT defines the current state of the FE safe state signal
            (Write only register) */
        FETIM_DREG_OUT_UNION        dRegOut;
        //! BREG_IN
        /*! The BREG_IN defines the state of several hardware component of the
            FETIM system (Read only register) */
        FETIM_BREG_IN_UNION         bRegIn;
        //! CREG_IN
        /*! The CREG_IN defines the revision level of the FETIM board hardware
            (Read only register) */
        FETIM_CREG_IN_UNION         cRegIn;
        //! Current parallel ADC data
        /*! This variable contains the latest parallel ADC binary data stored by
            the last read operation from the parallel ADC. Due to hardware
            configuration (unipolar) this variable is an unsigned char. */
        unsigned char               parAdcData;
        //! Current serial ADC data
        /*! This variable contains the latest serial ADC binary data stored by
            the last read operation from the serial ADC. Due to hardware
            configuration (unipolar) this variable is an unsigned int. */
        unsigned int                serAdcData;
    } FETIM_REGISTERS;


    /* Globals */
    /* Externs */
    extern FETIM_REGISTERS      fetimRegisters; //!< FETIM Registers

    /* Prototypes */
    /* Statics */
    static int getFetimParallelMonitor(void); // Perform core analog monitor functions for the parallel ADC
    static int getFetimSerialMonitor(void); // Perform core analog monitor functions for the serial ADC
    /* Externs */
    extern int getInterlockTemp(void); //!< This function monitors the interlock internal temperature sensors.
    extern int getInterlockFlow(void); //!< This function monitors the interlock airflow sensors.
    extern int getFetimDigital(unsigned char port); //!< This function monitors the digital values of the FETIM.
    extern int getIntrlkGlitchValue(void); //!< This function monitor the interlock glitch analog value
    extern int getCompressorTemp(void); //!< This function monitors the compressor external temperature sensors.
    extern int getCompHe2Press(void); //!< This function monitors the compressor He2 pressure.
    extern int setN2FillEnable(unsigned char enable); //!< This function enables the automatic N2 fill for the dewar.
    extern int setFeSafeStatus(unsigned char sage); //!< This function sets the bit holding the safe/unsafe status of the FE.
    extern int getFetimHardwRevision(void); //!< Reads the FETIM module hardware revision level

#endif /* _FETIMSERIALINTERFACE_H */
