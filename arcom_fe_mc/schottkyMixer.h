/*! \file       schottkyMixer.h
    \ingroup    polarization
    \brief      Schottky mixer header file

    <b> File informations: </b><br>
    Created: 2004/08/24 15:54:47 by avaccari

    <b> CVS informations: </b><br>
    \$Id: schottkyMixer.h,v 1.11 2006/11/30 23:01:43 avaccari Exp $

    This files contains all the informations necessary to define the
    characteristics and operate the Schottky mixer. */

/*! \defgroup   schottkyMixer   Schottky Mixer
    \ingroup    polarization
    \brief      Schottky mixer module
    \warning    The characteristics of the Schottky mixer have not yet been
                defined hence there are no information on the possible
                submodule.

    This module includes all the \ref schottkyMixer submodules.For more
    information on the \ref sideband module see \ref sideband.h */

#ifndef _SCHOTTKYMIXER_H
    #define _SCHOTTKYMIXER_H

    /* Extra includes */
    /* CAN module defines */
    #ifndef _CAN_H
        #include "can.h"
    #endif /* _CAN_H */

    /* Submodules definitions */
    #define SCHOTTKY_MIXER_MODULES_NUMBER      1       // It's just the dummyHandler

    /* Typedefs */
    //! Current state of the Schottky mixer
    /*! This structure represent the current state of the Schottky mixer.
        \ingroup    polarization
        \param      available   an unsigned char
        \param      dummy       an unsigned char
        \param      lastDummy   a LAST_CONTROL_MESSAGE
        \warning    The characteristics of the Schottky mixer have not yet
                    been defined hence this is just a dummy structure. */
    typedef struct {
        //! Schottky mixer availability
        /*! This variable indicates if this sideband is outfitted with this
            particular Schottky mixer. This value should be part of the device
            dependent informations retrived from the configuration database. */
        unsigned char   available;
        //! Schottky mixer description
        /*! This will contain the description of the Schottky mixer module.
            \warning    The characteristics of the Schottky mixer have not yet
                        been defined hence this is just a dummy structure. */
        unsigned char   dummy;
        //! Last control message: Schottky mixer description
        /*! This is the content of the last control message sent to the Schottky
            mixer description.
            \warning    This is added only as a place holder for the final setup
                        once the characteristics of the Schottky mixer will be
                        defined. */
        LAST_CONTROL_MESSAGE    lastDummy;
    } SCHOTTKY_MIXER;

    /* Globals */
    /* Externs */
    extern unsigned char currentSchottkyMixerModule; //!< Current addressed Schottky mixer submodule

    /* Prototypes */
    /* Statics */
    static void dummyHandler(void);
    /* Externs */
    extern void schottkyMixerHandler(void); //!< This function deals with the incoming can message

#endif /* _SCHOTTKYMIXER_H */
