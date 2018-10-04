/*! \file   error.h
    \brief  Error handling header file

    <b> File informations: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: error.h,v 1.47 2011/04/15 15:17:19 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the error handling module. */

#ifndef _ERROR_H

    #define _ERROR_H

    /* Extra includes */
    /* Debug defines */
    #ifndef _DEBUG_H
        #include "debug.h"
    #endif /* _DEBUG_H */

    /* Defines */
    /* General */
    #define NO_ERROR                0       //!< Global definition of NO_ERROR
    #define ERROR                   (-1)    //!< Global definition of ERROR
    #define ERROR_HISTORY_LENGTH    0xFF    //!< Max length of the error circular buffer
    /* CAN Message Errors */
    /* General */
    #define HARDW_RNG_ERR   (-2)    //!< The addressed or connected hardware it is not installed or activated
    #define HARDW_BLKD_ERR  (-3)    //!< The addressed hardware is locked
    #define HARDW_CON_ERR   (-5)    //!< There was an error while applying a conversion calculation
    #define HARDW_ERROR     (-7)    //!< The selected hardware is in error state. Follow ICD instruction to proceed.
    /* Monitor */
    #define MON_CAN_RNG     (-12)   //!< Monitor message on non existing RCA
    #define MON_ERROR_ACT   (-13)   //!< Monitor message detected a problem and an action was taken
    /* Control */
    #define CON_ERROR_RNG   (-10)   //!< Value of last control message received is outside the allowed range
    /* Modules */
    #define ERR_ERROR               0x00 //!< Error in the Error Module
    #define ERR_unassigned01        0x01 //!< Unassigned
    #define ERR_PP                  0x02 //!< Error in the Parallel Port Module
    #define ERR_CAN                 0x03 //!< Error in the CAN Module
    #define ERR_CARTRIDGE           0x04 //!< Error in the Cartridge Module
    #define ERR_CARTRIDGE_TEMP      0x05 //!< Error in the Cartridge Temperature Sensor Module
    #define ERR_LO                  0x06 //!< Error in the LO Module
    #define ERR_PLL                 0x07 //!< Error in the PLL Module
    #define ERR_YTO                 0x08 //!< Error in the YTO Module
    #define ERR_PHOTOMIXER          0x09 //!< Error in the Photomixer Module
    #define ERR_AMC                 0x0A //!< Error in the AMC Module
    #define ERR_PA                  0x0B //!< Error in the PA Module
    #define ERR_PA_CHANNEL          0x0C //!< Error in the PA Channel Module
    #define ERR_POLARIZATION        0x0D //!< Error in the Polarization Module
    #define ERR_SIDEBAND            0x0E //!< Error in the Sideband Module
    #define ERR_LNA_LED             0x0F //!< Error in the LNA led Module
    #define ERR_SIS_HEATER          0x10 //!< Error in the SIS heater Module
    #define ERR_unassigned11        0x11 //!< Unassigned
    #define ERR_SIS                 0x12 //!< Error in the SIS Module
    #define ERR_SIS_MAGNET          0x13 //!< Error in the SIS Magnet Module
    #define ERR_LNA                 0x14 //!< Error in the LNA Module
    #define ERR_LNA_STAGE           0x15 //!< Error in the LNA stage Module
    #define ERR_POL_SPECIAL_MSGS    0x16 //!< Error in the Polarization Special Messages Module
    #define ERR_POL_DAC             0x17 //!< Error in the Polarization DAC
    #define ERR_SERIAL_INTERFACE    0x18 //!< Error in the Serial Interface Module
    #define ERR_SERIAL_MUX          0x19 //!< Error in the Serial Mux Board
    #define ERR_TIMER               0x1A //!< Error in the Timer Module
    #define ERR_BIAS_SERIAL         0x1B //!< Error in the Bias Serial Interface Module
    #define ERR_LO_SERIAL           0x1C //!< Error in the LO Serial Interface Module
    #define ERR_POWER_DISTRIBUTION  0x1D //!< Error in the Power Distribution Module
    #define ERR_PD_MODULE           0x1E //!< Error in the PD Module Module
    #define ERR_PD_CHANNEL          0x1F //!< Error in the PD Channel Module
    #define ERR_PD_SERIAL           0x20 //!< Error in the PD serial interface module
    #define ERR_IF_CHANNEL          0x21 //!< Error in the IF Channel Module
    #define ERR_IF_SWITCH           0x22 //!< Error in the IF Switch Module
    #define ERR_IF_SERIAL           0x23 //!< Error in the IF Switch serial interface module
    #define ERR_CRYOSTAT            0x24 //!< Error in the Cryostat Module
    #define ERR_TURBO_PUMP          0x25 //!< Error in the Turbo Pump Module
    #define ERR_VACUUM_CONTROLLER   0x26 //!< Error in the Vacuum Controller Module
    #define ERR_GATE_VALVE          0x27 //!< Error in the Gate Valve Module
    #define ERR_SOLENOID_VALVE      0x28 //!< Error in the Solenoid Valve Module
    #define ERR_VACUUM_SENSOR       0x29 //!< Error in the Vacuum Sensor Module
    #define ERR_CRYOSTAT_TEMP       0x2A //!< Error in the Cryostat Temperature Module
    #define ERR_CRYO_SERIAL         0x2B //!< Error in the Cryostat serial interface module
    #define ERR_MODULATION_INPUT    0x2C //!< Error in the EDFA modulation input module
    #define ERR_PHOTO_DETECTOR      0x2D //!< Error in the EDFA photo detector module
    #define ERR_LASER               0x2E //!< Error in the EDFA laser detector module
    #define ERR_EDFA                0x2F //!< Error in the EDFA module
    #define ERR_OPTICAL_SWITCH      0x30 //!< Error in the Optical Switch module
    #define ERR_LPR                 0x31 //!< Error in the LPR module
    #define ERR_LPR_TEMP            0x32 //!< Error in the LPR temperature module
    #define ERR_LPR_SERIAL          0x33 //!< Error in the LPR serial interface module
    #define ERR_MI_DAC              0x34 //!< Error in the Modulation input DAC module
    #define ERR_INI                 0x35 //!< Error in the INI file access module
    #define ERR_OWB                 0x36 //!< Error in the OWB module
    #define ERR_FETIM               0x37 //!< Error in the FETIM module
    #define ERR_INTERLOCK           0x38 //!< Error in the FETIM interlock module
    #define ERR_COMPRESSOR          0x39 //!< Error in the FETIM compressor module
    #define ERR_INTRLK_SENS         0x3A //!< Error in the FETIM interlock sensors module
    #define ERR_INTRLK_STATE        0x3B //!< Error in the FETIM interlock state module
    #define ERR_INTRLK_TEMP         0x3C //!< Error in the FETIM interlock temperature module
    #define ERR_INTRLK_FLOW         0x3D //!< Error in the FETIM interlock flow module
    #define ERR_INTRLK_GLITCH       0x3E //!< Error in the FETIM interlock glitch module
    #define ERR_FETIM_EXT_TEMP      0x3F //!< Error in the FETIM external temperature module
    #define ERR_COMP_HE2_PRESS      0x40 //!< Error in the FETIM compressor He2 pressure module
    /* Error codes - shared by all modules */
    #define ERC_NO_MEMORY           0x01 //!< Not enough memory
    #define ERC_REDIRECT_STDERR     0x02 //!< Error library: redirecting stderr
    #define ERC_IRQ_DISABLED        0x03 //!< Parallel port: IRQ was disabled
    #define ERC_IRQ_RANGE           0x04 //!< Parallel port: IRQ out of range
    #define ERC_AMBSI_WAIT          0x05 //!< Parallel port: Waiting for AMBSI ready
    #define ERC_AMBSI_EXPIRED       0x06 //!< Parallel port: Timed out waiting for AMBSI ready.  CAN disabled
    #define ERC_MAINT_MODE          0x07 //!< CAN: message blocked because FE is in MAINTENANCE mode
    #define ERC_HARDWARE_TIMEOUT    0x08 //!< Timeout waiting for hardware to become ready
    #define ERC_HARDWARE_ERROR      0x09 //!< Hardware is in an error state
    #define ERC_HARDWARE_WAIT       0x0A //!< Waiting for previous command to finish
    #define ERC_FLASH_ERROR         0x0B //!< Error reading/writing flash disk
    #define ERC_HARDWARE_BLOCKED    0x0C //!< Command blocked by safety check
    #define ERC_DEBUG_ME            0x0D //!< Software entered an invalid or impossible state
    #define ERC_0E                  0x0E //!< 
    #define ERC_0F                  0x0F //!< 
    #define ERC_MODULE_RANGE        0x10 //!< Submodule is out of range
    #define ERC_MODULE_ABSENT       0x11 //!< Submodule is not installed
    #define ERC_MODULE_POWER        0x12 //!< Submodule is not powered
    #define ERC_RCA_CLASS           0x13 //!< RCA class out of range
    #define ERC_RCA_RANGE           0x14 //!< RCA out of range
    #define ERC_COMMAND_VAL         0x15 //!< Command value out of range

    /* Globals */
    /* Externs */
    extern unsigned char errorNewest;    //!< A global to keep track of the newest error index
    extern unsigned char errorOldest;    //!< A global to keep track of the oldest error index
    extern unsigned int * errorHistory;  //!< A global pointer to the error history

    /* Prototypes */
    /* Statics */
    #ifdef ERROR_REPORT
        static void reportErrorConsole(unsigned char moduleNo,
                                       unsigned char errorNo);
    #endif /* ERROR_REPORT */
    /* Externs */
    extern int errorInit(void); //!< Initialize error routine
    extern int errorStop(void); //!< Shutdown the error routine
    extern void storeError(unsigned char moduleNo,
                           unsigned char errorNo); //!< Store error
    extern void criticalError(unsigned char moduleNo,
                              unsigned char errorNo); //!< Report critical error

#endif /* _ERROR_H */
