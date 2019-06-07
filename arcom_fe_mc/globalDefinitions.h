/*! \file   globalDefinitions.h
    \brief  Global definitions file

    <b> File information: </b><br>
    Created: 2004/08/24 13:24:53 by avaccari

    This file contains definitions that are used throughout the various modules
    of the program. */

#ifndef _GLOBALDEFINITIONS_H
    #define _GLOBALDEFINITIONS_H

    /* Defines */
    /* General definitions used by all the modules */
    //! Serial number size
    #define SERIAL_NUMBER_SIZE       8  /*! This is the size in byte of the
                                            array that have to contain serial
                                            number */

    /* Build file name definitions */
    #define MAX_STRING_SIZE     40                  //! Max lenght of string
    #define MAX_FILE_NAME_SIZE  13                  //! Max size of DOS8.3 file names

    /* General definitions */
    #define TRUE            1       //!< Global definition for true
    #define FALSE           0       //!< Global definition for false
    #define YES             1
    #define NO              0
    #define ENABLE          1
    #define DISABLE         0
    #define AVAILABLE       1
    #define UNAVAILABLE     0
    #define UNDEFINED       (-1)
    #define IN_RANGE        0       //!< Global definition for IN range condition
    #define ABOVE_RANGE     1       //!< Global definition for ABOVE range condition
    #define BELOW_RANGE     (-1)    //!< Global definition for BELOW range condition
    #define NO_SHIFT        0       //!< No shift signal
    #define SHIFT_LEFT      0       //!< Signal to shift left
    #define SHIFT_RIGHT     1       //!< Signal to shift right
    #define FLOAT_ERROR     (-1.0)  //!< Global definition for a float value error
    #define FLOAT_UNINIT    (-2.0)  //!< Global definition for an uninitialized float

    /* Disable "Hardware Availability" checks from INI files: */
    #undef CHECK_HW_AVAIL

    /* Useful fast macros */
    /* Round a float to the closest int */
    #define FLT_TO_INT(float)       ((int)(float+0.5))


    /* Typedefs */
    //! Handler function
    /*! This function is used by different modules in the program to address
        submodules. It is a general prototype of a function which takes a
        \em void as an argument and returns a \em void.
        It is mostly used as element of an array which will perform a series
        of redirection depending on the incoming RCA to address the correct
        function.
        \param  void
        \return void */
    typedef void (*HANDLER)(void);

    // Char[4] to float conversion union
    /* This union define an easy way to convert the incoming CAN message
       payload to several format and vice versa. */
    typedef union {
        long int        longint;
        unsigned int    uint[2];
        float           flt;
        unsigned char   chr[4];
    } CONVERSION;

    /* Some extra define to help in the conversion */
    #define CONV_UINT(idx)      convert.uint[idx]
    #define CONV_CHR(idx)       convert.chr[idx]
    #define CONV_CHR_ADD        convert.chr
    #define CONV_FLOAT          convert.flt
    #define CONV_LONGINT        convert.longint

    /* Globals */
    /* Externs */
    extern CONVERSION convert; //!< Union used to convert float to unsigned char[4]

    /* Prototypes */
    /* Externs */
    extern char *buildString(unsigned char *prefix, unsigned char number, unsigned char *suffix); //!< Build a string
    extern unsigned char checkRange(float low, float test, float high); //!< Check range
    extern void changeEndian(unsigned char *destination, unsigned char *source); //!< Change endianicity for floating point types
    extern void changeEndianInt(unsigned char *destination, unsigned char *source); //!< Change endianicity for int types
    extern void bogoFunction(void); //!< Place holder function
    extern long htol(char *hex); //!< Conversion from Hex to long int

#endif /* _GLOBALDEFINITIONS_H */
