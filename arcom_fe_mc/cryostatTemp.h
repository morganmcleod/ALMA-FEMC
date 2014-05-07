/*! \file       cryostatTemp.h
    \ingroup    cryostat
    \brief      Cryostat temperature sensor header file
    \todo       Add defines to specify what temperature is each sensor measuring.
                This informations are probably stored into the configuration
                database so they have to be downloaded at configuration time to
                the embedded controller. Make sure to reserve some CAN address
                to perform the download operation.

    <b> File informations: </b><br>
    Created: 2004/10/25 16:45:53 by avaccari

    <b> CVS informations: </b><br>
    \$Id: cryostatTemp.h,v 1.8 2007/08/28 21:46:47 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the dewar temperature sensor. */

/*! \defgroup   cryostatTemp  Dewar temperature sensor
    \ingroup    cryostat
    \brief      Cryostat temperature sensor module
    \note       The \ref cryostatTemp module doesn't include any submodule.

    For more information on this module see \ref cryostatTemp.h */

#ifndef _CRYOSTATTEMP_H
    #define _CRYOSTATTEMP_H

    /* Extra includes */
    /* GLOBAL DEFINITIONS Defines */
    #ifndef _GLOBALDEFINITIONS_H
        #include "globalDefinitions.h"
    #endif /* _GLOBALDEFINITIONS_H */

    /* Defines */
    #define CRYOSTAT_TEMP_SENSORS_NUMBER    13  //!< Number of temperature sensors in the dewar

    #define CRYOCOOLER_4K       0x0 //!< 00: 4K cryocooler (TVO)
    #define PLATE_4K_NEAR_LINK1 0x1 //!< 01: 4K plate near link1 (TVO)
    #define PLATE_4K_NEAR_LINK2 0x2 //!< 02: 4K plate near link2 (TVO)
    #define PLATE_4K_FAR_SIDE1  0x3 //!< 03: 4K plate far side1 (TVO)
    #define PLATE_4K_FAR_SIDE2  0x4 //!< 04: 4K plate far side2 (TVO)
    #define CRYOCOOLER_12K      0x5 //!< 05: 12K cryocooler (TVO)
    #define PLATE_12K_NEAR_LINK 0x6 //!< 06: 12K plate near link (TVO)
    #define PLATE_12K_FAR_SIDE  0x7 //!< 07: 12K plate far side (TVO)
    #define SHIELD_TOP_12K      0x8 //!< 08: 12K shield top (TVO)
    #define CRYOCOOLER_90K      0x9 //!< 09: 90K cryocooler (PRT)
    #define PLATE_90K_NEAR_LINK 0xA //!< 10: 90K plate near link (PRT)
    #define PLATE_90K_FAR_SIDE  0xB //!< 11: 90K plate far side (PRT)
    #define SHIELD_TOP_90K      0xC //!< 12: 90K shield top (PRT)

    /* Cryostat sensor evaluation defines */
    /* TVO sensors */
    #define TVO_SENSORS_NUMBER  9           // Number of TVO sensor in the dewar
    #define TVO_GAIN            454.545454  // TVO sensor gain factor
    #define TVO_COEFFS_NUMBER   7           // Coefficients necessary to calculate the TVO temperature
    #define TVO_COEFF_0         0           // Coefficient index for x^0
    #define TVO_COEFF_1         1           // Coefficient index for x^1
    #define TVO_COEFF_2         2           // Coefficient index for x^2
    #define TVO_COEFF_3         3           // Coefficient index for x^3
    #define TVO_COEFF_4         4           // Coefficient index for x^4
    #define TVO_COEFF_5         5           // Coefficient index for x^5
    #define TVO_COEFF_6         6           // Coefficient index for x^6
    #define TVO_RESISTOR_SCALE  1000.0      // Scaling coefficient for resistor readout
    /* PRT sensors */
    #define PRT_GAIN            125.0       // PRT sensor gain
    /* PRT sensor interpolation curve. There are 2 curves, the first
       (PRT_A_SCALE) works for values greater than 124 ohm (~60K), the other
       (PRT_B_SCALE) works for values smaller than 124 ohm. */
    #define PRT_A_SCALE         124.0
    #define PRT_A0              (0.513971)
    #define PRT_A1              (276.222931)
    #define PRT_A2              (-1038.573479)
    #define PRT_A3              (2460.959311)
    #define PRT_A4              (-3243.304766)
    #define PRT_A5              (2211.327698)
    #define PRT_A6              (-607.247388)
    #define PRT_B_SCALE         1000.0
    #define PRT_B0              (28.486734)
    #define PRT_B1              (278.396620)
    #define PRT_B2              (-260.205006)
    #define PRT_B3              (687.754698)
    #define PRT_B4              (-891.652830)
    #define PRT_B5              (583.158140)
    #define PRT_B6              (-152.808821)

    /* Submodule definistions */
    #define CRYOSTAT_TEMP_MODULES_NUMBER        1   // It's just the temperature

    /* Typedefs */
    //! Current state of the dewar temperature sensors
    /*! This structure represent the current state of the dewar temperature
        system.
        \ingroup    cryostat
        \param      temp[Op]    This contains the most recent read-back value
                                for the temperature
        \param      coeff[Cf]   Interpolation coefficient for the TVO
                                temperature sensors */
    typedef struct {
        //! Dewar temperature
        /*! This is the temperature (in K) as registered by the sensor. */
        float   temp[OPERATION_ARRAY_SIZE];
        //! TVO interpolation coefficient
        /*! These are the coefficients necessary to calculate the temperature
            of the TVO sensors. */
        float  coeff[TVO_COEFFS_NUMBER];
    } CRYOSTAT_TEMP;

    /* Globals */
    /* Externs */
    extern unsigned char currentCryostatTempModule; //!< Currently addressed cryostat temperature submodule
    extern unsigned int currentCryostatTempSensorAdd[CRYOSTAT_TEMP_SENSORS_NUMBER]; //!< Currently addressed cryostat temperature sensor

    /* Prototypes */
    /* Statics */
    static void tempHandler(void);
    /* Externs */
    extern void cryostatTempHandler(void); //!< This function deals with the incoming CAN message


#endif /* _CRYOSTATTEMP_H */
