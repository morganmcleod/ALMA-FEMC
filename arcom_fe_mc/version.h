/*! \file   version.h
    \brief  Version information file

    <b> File informations: </b><br>
    Created: 2004/10/06 16:47:08 by avaccari

    <b> CVS informations: </b><br>
    \$Id: version.h,v 1.25 2007/10/02 22:04:58 avaccari Exp $

    This files contains all the informations about the current version of the
    software and its revision history.

    The revision numbering system is according to the following rules:
        - The <em>Patch level</em> is changed when bugs are fixed
        - The <em>Minor version</em> is changed when features are added
        - The <em>Major version</em> is changed when major changes are made to
          the software:
            - Affected backward compatibility
            - Changed functionality
            - Official global new releases
            - ...

    REVISION HISTORY
    2007-08-09  000.007.000
        Development Release.
        This release fixed bugs in the previous release and added monitor and
        control support for the LPR module.
        There is also partial support for configuration through INI files.

    2007-05-29  000.006.000
        Deveolpment Release.
        This release fixed bugs in the previous release and added monitor and
        control support for the Cryogenics. The temperature section of the cryo
        M&C still have to be debugged with hardware.

    2007-01-16  000.005.000
        Development Release.
        This release fixed bugs in the previous release and added monitor and
        control support for the IF switch module.

    2006-11-22  000.004.000
        Development Release.
        This release fixed bugs in the previous release and added monitor and
        control support for the cartridge temperature sensors.

    2006-11-14  000.003.000
        Development Release.
        This release fixed bugs in the previous release and added monitor and
        control support for the cartridge power distribution system.

    2006-10-10  000.002.001
        Development Release.
        This release fixed bugs in the previous release.

    2006-10-05  000.002.000
        Development Release.
        This release fixed bugs in the previus release and added monitor and
        control for the LO.

    2006-08-21  000.001.000
        Development Release.
        This release implement a working cartridge bias module. */

/*! \mainpage
    \section    sectionVersion  Version
    Documentation relative to Version: 000.008.000RC */

#ifndef _VERSION_H
    #define _VERSION_H

    /* Defines */
    #define VERSION_MAJOR   000  //!< Major version
    #define VERSION_MINOR   007  //!< Minor version
    #define VERSION_PATCH   000  //!< Patch level
    #define VERSION_DATE    "2007-08-09" //!< Version Date
    #define PRODUCT_TREE    "FEND-40.04.03.03-011-A-FRM" //! Product Tree number
    #define AUTHOR          "Andrea Vaccari - NRAO (avaccari@nrao.edu)"
    #define BUGZILLA        "bugzilla.cv.nrao.edu"

    /* Prototypes */
    /* Statics */
    /* Externs */
    extern void displayVersion(void); //!< This function displays the version information


#endif /* _VERSION_H */
