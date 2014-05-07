/*! \file       cryostatSerialInterface.h
    \ingroup    serialInterface
    \brief      Cryostat serial interface header file

    <b> File informations: </b><br>
    Created: 2007/04/10 11:08:20 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cryostatSerialInterface.h,v 1.12 2008/02/28 22:15:05 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the cryostat serial interface. */

/*! \defgroup   cryostatSerialInterface  Cryostat serial interface
    \ingroup    serialInterface
    \brief      Cryostat System Serial Interface
    \note       The \ref cryostatSerialInterface module doesn't include any
                submodule.

    For more information on this module see \ref cryostatSerialInterface.h */

#ifndef _CRYOSTATSERIALINTERFACE_H
    #define _CRYOSTATSERIALINTERFACE_H

    /* Extra includes */
    /* CRYOSTATTEMP defines */
    #ifndef _CRYOSTATTEMP_H
        #include "cryostatTemp.h"
    #endif /* _CRYOSTATTEMP_H */

    /* GLOBALDEFINITIONS defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    /* General */
    #define BACKING_PUMP_ENABLE             1
    #define BACKING_PUMP_DISABLE            0
    #define TURBO_PUMP_ENABLE               1
    #define TURBO_PUMP_DISABLE              0
    #define VACUUM_CONTROLLER_ENABLE        1
    #define VACUUM_CONTROLLER_DISABLE       0
    #define VACUUM_CONTROLLER_HRDW_ENABLE   0   // Hardware logic is inverted
    #define VACUUM_CONTROLLER_HRDW_DISABLE  1   // Hardware logic is inverted
    #define VACUUM_CONTROLLER_HRDW_OK       1   // Hardware logic is inverted
    #define VACUUM_CONTROLLER_HRDW_ERROR    0   // Hardware logic is inverted
    #define VACUUM_CONTROLLER_OK            0
    #define VACUUM_CONTROLLER_ERROR         1
    #define CRYO_AREG                       0
    #define CRYO_BREG                       1
    #define SOLENOID_VALVE_CLOSE            0
    #define SOLENOID_VALVE_OPEN             1
    #define SOLENOID_VALVE_UNKNOWN          2
    #define SOLENOID_VALVE_ERROR            3
    #define SOLENOID_VALVE_SENSORS_UNKNOWN  0x0
    #define SOLENOID_VALVE_SENSORS_OPEN     0x1
    #define SOLENOID_VALVE_SENSORS_CLOSE    0x2
    #define GATE_VALVE_CLOSE                0
    #define GATE_VALVE_OPEN                 1
    #define GATE_VALVE_UNKNOWN              2
    #define GATE_VALVE_ERROR                3
    #define GATE_VALVE_SENSORS_UNKNOWN      0x0
    #define GATE_VALVE_SENSORS_OPEN         0x5
    #define GATE_VALVE_SENSORS_CLOSE        0xA
    #define CRYOSTAT_ANALOG_READOUTS        10
    #define CRYOSTAT_TEMP_CONV_ERR          FLOAT_ERROR
    #define CRYOSTAT_PRESS_CONV_ERR         FLOAT_ERROR


    /* Command words:
       - Rg is the register */
    #define CRYO_PARALLEL_WRITE(Rg)     (Rg)
    // 0x02-0x07 -> not used
    #define CRYO_ADC_CONVERT_STROBE     0x08
    #define CRYO_ADC_DATA_READ          0x09
    #define CRYO_PARALLEL_READ          0x0A
    // 0x0B-0x1F -> unused



    /* --- AREG definitions (11-bit) --- */
    /* Write Only: 11-bit + 1 bit for latching.
       AREG is the register that defines which device has to be monitored. */
    #define CRYO_AREG_SIZE              12
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define CRYO_AREG_SHIFT_SIZE        1
    #define CRYO_AREG_SHIFT_DIR         SHIFT_LEFT
    /* bit 10-0 -> Monitor points:
       - Se is the temperature sensor number
       - Ps is the pressure sensor number */
    #define CRYO_AREG_TEMPERATURE(Se)       currentCryostatTempSensorAdd[Se]
    #define CRYO_AREG_PRESSURE(Ps)          (0x100*Ps+0x200)
    #define CRYO_AREG_SUPPLY_CURRENT_230V   0x400


    /* --- BREG definitions (5-bit) --- */
    /* Write Only: 5-bit + 1 bit for latching.
       BREG is a state register, it can only be written as a whole and every
       time is updated in the hardware, the state described by the different
       fields is update accordingly. */
    #define CRYO_BREG_SIZE              6
    /* This register has to be shifted left before being sent out for alignment.
       This is because of the extra bit the hardware need to latch the data. */
    #define CRYO_BREG_SHIFT_SIZE        1
    #define CRYO_BREG_SHIFT_DIR         SHIFT_LEFT



    /* --- Status register definitions (10-bit) --- */
    /* Read only: 10-bit */
    #define CRYO_STATUS_REG_SIZE        10
    #define CRYO_STATUS_REG_SHIFT_SIZE  NO_SHIFT
    #define CRYO_STATUS_REG_SHIFT_DIR   NO_SHIFT



    /* --- ADC definitions (16-bit) --- */
    #define CRYO_ADC_STROBE_SIZE            3           // Bit size of the CRYO ADC convert strobe
    #define CRYO_ADC_STROBE_SHIFT_SIZE      NO_SHIFT    // The strobe outgoing data is not shifted
    #define CRYO_ADC_STROBE_SHIFT_DIR       NO_SHIFT    // The strobe outgoing data is not shifted
    #define CRYO_ADC_RANGE                  65536       // Full ADC range
    #define CRYO_ADC_DATA_SIZE              18          // Read only: 2 for internal use + 16-bit
    #define CRYO_ADC_DATA_SHIFT_SIZE        NO_SHIFT    // The incoming ADC data is not shifted
    #define CRYO_ADC_DATA_SHIFT_DIR         NO_SHIFT    // The incoming ADC data is not shifted
    #define CRYO_ADC_BUSY                   0           // Busy state signal
    #define CRYO_ADC_VOLTAGE_IN_SCALE       10.0        // Scale factor for the read voltage
    #define CRYO_ADC_SUPPLY_CURRENT_SCALE   1.488645855 // Scale factor for the supply current (includes the vactor 10 scaling for the voltage itself)
    #define CRYO_ADC_CRYO_PRESS_OFFSET      (-7.75)     // Offset factor for the cryostat pressure
    #define CRYO_ADC_CRYO_PRESS_SCALE       0.75        // Scale factor for the cryostat pressure (includes the vactor 10 scaling for the voltage itself)
    #define CRYO_ADC_VAC_PORT_PRESS_OFFSET  (-6.143)    // Offset factor for the vacuum port pressure
    #define CRYO_ADC_VAC_PORT_PRESS_SCALE   1.286       // Scale factor for the vacuum port pressure (includes the vactor 10 scaling for the voltage itself)


    //! CRYO AREG bitfield (11-bit+5 -> 16-bit).
    /*! The AREG defines which monitor point is currently selected.
        \param  monitorPoint            an unsinged int         :11 */
    typedef struct {
        //! Selected monitor point (bit 10-0)
        /*! This bitfield defined the currently selected monitor point:
                - 000xxx000xx       TVO Sensor 4K cryocooler
                - 000xxx001x0       TVO Sensor 4K plate near link #1
                - 000xxx001x1       TVO Sensor 4K plate near link #2
                - 000xxx0100x       TVO Sensor 4K plate far side
                - 000xxx0101x       TVO Sensor 4K plate far side
                - 000xxx011xx       TVO Sensor 12K cryocooler
                - 000xxx100xx       TVO Sensor 12K plate near link
                - 000xxx101xx       TVO Sensor 12K plate far side
                - 000xxx110xx       TVO Sensor 12K shield top
                - 001000xxxxx       PRT Sensor 90K cryocooler
                - 001001xxxxx       PRT Sensor 90K plate near link
                - 001010xxxxx       PRT Sensor 90K plate far side
                - 001011xxxxx       PRT Sensor 90K shield top
                - 010xxxxxxxx       Cryostat pressure
                - 011xxxxxxxx       Vacuum port pressure
                - 100xxxxxxxx       Backing pump current */
        unsigned int monitorPoint           :11;
        /* 5 extra bit to fill it up */
        unsigned int                        :5;
    } CRYO_AREG_BITFIELD;

    //! CRYO AREG
    /*! The AREG union allows to address the AREG either as a single integer or
        as a bitfield structure.
        \param  integer     an int
        \param  bitField    a CRYO_AREG_BITFIELD */
    typedef union {
        //! AREG in integer format
        /*! The CRYO AREG is this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! AREG in bit fields format
        /*! The CRYO AREG in this format is very easy to update. */
        CRYO_AREG_BITFIELD  bitField;
    } CRYO_AREG_UNION;




    //! CRYO BREG bitfield (5-bit+11 -> 16-bit).
    /*! The BREG defines the current state of several submodules.
        \param  vacuumController    an unsigned int     :1
        \param  turboPump           an unsigned int     :1
        \param  backingPump         an unsigned int     :1
        \param  solenoidValve       an unsigned int     :1
        \param  gateValve           an unsigned int     :1 */
    typedef struct {
        //! Vacuum controller state (bit 0)
        /*! This bit field defines the current state of the vacuum controller:
                - \ref VACUUM_CONTROLLER_ENABLE     -> ON
                - \ref VACUUM_CONTROLLER_DISABLE    -> OFF */
        unsigned int vacuumController   :1;
        //! Turbo pump state (bit 1)
        /*! This bit field defines the current state of the turbo pump:
                - \ref TURBO_PUMP_ENABLE    -> ON
                - \ref TURBO_PUMP_DISABLE   -> OFF */
        unsigned int turboPump          :1;
        //! Backing pump state (bit 2)
        /*! This bit field defines the current state of the backing pump:
                - \ref BACKING_PUMP_ENABLE    -> ON
                - \ref BACKING_PUMP_DISABLE   -> OFF */
        unsigned int backingPump        :1;
        //! Solenoid valve state (bit 3)
        /*! This bit field defines the current state of the solenoid valve:
                - \ref SOLENOID_VALVE_OPEN    -> OPEN
                - \ref SOLENOID_VALVE_CLOSE   -> CLOSE */
        unsigned int solenoidValve      :1;
        //! Gate valve state (bit 3)
        /*! This bit field defines the current state of the gate valve:
                - \ref GATE_VALVE_OPEN    -> OPEN
                - \ref GATE_VALVE_CLOSE   -> CLOSE */
        unsigned int gateValve          :1;
        /* 11 extra bit to fill it up */
        unsigned int                    :11;
    } CRYO_BREG_BITFIELD;


    //! CRYO BREG
    /*! The BREG union allows to address the BREG eithr as a single integer or
        as a bitfield structure.
        \param  integer     an int
        \param  bitField    a CRYO_BREG_BITFIELD */
    typedef union {
        //! BREG in integer format
        /*! The CRYO BREG in this format is ready to be sent as data to the
            destination device. */
        int                 integer;
        //! BREG in bit fields format
        /*! The CRYO BREG in this format is very easy to update. */
        CRYO_BREG_BITFIELD  bitField;
    } CRYO_BREG_UNION;



    //! CRYO status register bitfield (10-bit+6 -> 16-bit)
    /*! The CRYO status register defines the current state of several hardware
        within the cryostat:
        \param vacuumControllerState    an unsigned int     :1
        \param turboPumpError           an unsigned int     :1
        \param turboPumpSpeed           an unsigned int     :1
        \param gateValveState           an unsigned int     :4
        \param solenoidValveState       an unsigned int     :2
        \param adcReady                 an unsigned int     :1 */
    typedef struct {
        //! Vacuum controller state
        /*! This 1-bit field returns the current error status of the vacuum
            controller:
                - 0 -> OK
                - 1 -> Error detected */
        unsigned int vacuumControllerState  :1;
        //! Turbo pump error
        /*! This 1-bit field returns the current error status of the turbo pump:
                - 0 -> OK
                - 1 -> Error detected */
        unsigned int turboPumpError         :1;
        //! Turbo pump speed
        /*! This 1-bit field returns the current state of the turbo pump speed:
                - 0 -> Speed low
                - 1 -> Speed OK */
        unsigned int turboPumpSpeed         :1;
        //! Gate valve state
        /*! This 4-bit field returns the current state of the gate valve.
            In reality there are 4 sensor to detect the position of the gate
            valve:
                - open sensor #1
                - close sensor #1
                - open sensor #2
                - close sensor #2
            There are 16 possible combination of the output mostly of which
            describe an error state. */
        unsigned int gateValveState         :4;
        //! Solenoid valve state
        /*! This 2-bit field returns the current state of the solenoid valve.
            In reality there are 2 sensor to detect the position of the solenoid
            valve:
                - open sensor
                - close sensor
            There are 4 possible combination of the output. */
        unsigned int solenoidValveState     :2;
        //! ADC Ready bit
        /*! This 1-bit field return the current status of the ADC:
                - 0 -> ADC Busy
                - 1 -> ADC Ready */
        unsigned int adcReady               :1;
        /* 6 extra bits to fill it up */
        unsigned int                        :6;
    } CRYO_STATUS_REG_BITFIELD;

    //! CRYO status register
    /*! The status register union allows to address the status register either
        as a single unsigned int or as a bitfield structure.
        \param  integer     an int
        \param  bitField    a CRYO_STATUS_REG_BITFIELD */
    typedef union {
        //! Status register in integer format
        /*! The CRYO status register in this format is ready to be read as data
            from the addressed destination device. */
        int                         integer;
        //! Status register in bit field format
        /*! The CRYO status register in this format is very easy to update. */
        CRYO_STATUS_REG_BITFIELD    bitField;
    } CRYO_STATUS_REG_UNION;


    //! CRYO registers
    /*! This structure contains the current state of the CRYO registers.
        \param aReg         a CRYO_AREG_UNION
        \param bReg         a CRYO_BREG_UNION
        \param statusReg    a CRYO_STATUS_REG_UNION
        \param adcData      an unsigned int */
    typedef struct {
        //! AREG
        /*! The AREG defines which monitor point is currently selected (Write
            only register). */
        CRYO_AREG_UNION         aReg;
        //! BREG
        /*! The BREG defines the current state of several submodules (Write only
            register). */
        CRYO_BREG_UNION         bReg;
        //! Status register
        /*! The status register contains the current state of several hardware
            subsystems of the cryostat */
        CRYO_STATUS_REG_UNION   statusReg;
        //! Current ADC data
        /*! This variable contains the latest ADC binary data stored by a read
            operation from the ADC. Due to hardware configuration (unipolar)
            this variable is an unsigned int. */
        unsigned int            adcData;
    } CRYO_REGISTERS;

    /* Globals */
    /* Externs */
    extern CRYO_REGISTERS   cryoRegisters; //!< Cryostat Registers
    /* Prototypes */
    /* Statics */
    static int getCryoAnalogMonitor(void); // Perform core analog monitor functions

    /* Externs */
    extern int setBackingPumpEnable(unsigned char enable); //!< This function enables/disables/ the backing pump
    extern int getSupplyCurrent230V(void); //!< This function monitors the 230V supply current
    extern int setTurboPumpEnable(unsigned char enable); //!< This function enables/disables the turbo pump
    extern int getTurboPumpStates(void); //!< This function monitors the different states of the turbo pump
    extern int setGateValveState(unsigned char state); //!< This function opens/closes the gate valve
    extern int getGateValveState(void); //!< This function monitors the state of the gate valve
    extern int setSolenoidValveState(unsigned char state); //!< This function opens/closes the solenoid valve
    extern int getSolenoidValveState(void); //!< This function monitors the state of the solenoid valve
    extern int getVacuumSensor(void); //!< This function monitors the vacuum sensor pressure
    extern int setVacuumControllerEnable(unsigned char state); //!< This function enables/disables the vacuumn controller
    extern int getVacuumControllerState(void); //!< This function monitors the state of the vacuum controller
    extern int getCryostatTemp(void); //!< This function monitors the cryostat temperature

#endif /* _CRYOSTATSERIALINTERFACE_H */
