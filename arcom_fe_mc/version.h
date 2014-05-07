/*! \file   version.h
    \brief  Version information file

    <b> File informations: </b><br>
    Created: 2004/10/06 16:47:08 by avaccari

    <b> CVS informations: </b><br>

    \$Id: version.h,v 1.36 2009/04/24 22:37:32 avaccari Exp $

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
    2009-04-24  002.001.001     (02-01-00.exe)
        Debug Release.
        This release contains a fix to prevent the CIPT from operating the mixer
        heaters incorrectly during the control software testing.
        This version is tagged Ver_02_00_01.

    2009-04-24  002.001.000     (fe_mc.exe or 02-01-00.exe or 2k90424.exe)
        Official Release.
        This version is tagged Ver_02_01_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).
        - Added simulator for ESNs number. The option can be controlled with
          the ESNS.INI configuration file.
        - Now when the software is terminated, all the cartridges will be turned
          off.
        - The storing of the PA channels information now follows the cartridge
          dependent mapping rather than just the CAN message.
        - The drain and gate voltages of the PAs are set to 0.0V every time a
          cartridge is initialized.
        - The current operation mode of the front end can be monitored. The setting
          is not implemented yet and it will not untill the safety checks are in
          place.

    2009-02-25  002.000.000     (fe_mc.exe or 02-00-00.exe or 2k90225.exe)
        Official Release.
        This version is tagged Ver_02_00_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).

    2008-10-08  002.000.000b1    (2-0-00b1.exe)
        The two previous beta revisions are rolled into the new revision
        including all the previous changes. Since this new revision is not
        backwards compatible with the previous versions it will be released as
        rev 2.
        This version is tagged Ver_02_00_00b1.

    2008-10-07  001.000.002b     (1-0-02b2.exe)
        - Modified LO to provide only 1 extra clock cycle for the YTO strobe (JIRA
          FE-13)
        This version is tagged Ver_01_00_02b2.

    2008-09-26  001.000.002b     (1-0-02b1.exe)
        *** Added functionality ***
        --- Console ---
        - If the console is activated, then pressing the ' key will retype the
          last submitted console command.
        - If the console is activated, then pressing the " key will resubmit the
          last submitted console command.
        *** Patches ***
        --- Cryostat ---
        - The TVO sensor number is now a string instead of an integer.
        - Modified return error values for the 230V supply current monitoring to
          take in account of the hardware retry issue (unstable monitor points)
        --- Console ---
        - Added #define to enable/disable console debugging
        --- Error ---
        - Corrected output number of cartridge to match the band number
        --- LO ---
        - Modified size of AREG to match a fix to an hardware problem that required
          a change in the CPLD code inside the LO monitor and control.
          (JIRA issue FEIC-73)
        This version is tagged Ver_01_00_02b1.

    2008-05-15  001.000.001     (01-00-01.exe)
        Debug Release.
        This release contains a fix to prevent the CIPT from operating the mixer
        heaters incorrectly during the control software testing.
        This version is tagged Ver_01_00_01.

    2008-04-09  001.000.000
        First Official Release.
        This is the first stable working release.
        This version is tagged Ver_01_00_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).

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
    Documentation relative to Version: 002.001.000 */

#ifndef _VERSION_H
    #define _VERSION_H

    /* Defines */
    #define VERSION_MAJOR   002  //!< Major version
    #define VERSION_MINOR   001  //!< Minor version
    #define VERSION_PATCH   000  //!< Patch level

    #define VERSION_DATE    "2009-04-24" //!< Version Date
    #define VERSION_NOTES   "" //!<Version Notes
    #define PRODUCT_TREE    "FEND-40.04.03.03-011-A-FRM" //! Product Tree number
    #define AUTHOR          "Andrea Vaccari - NRAO (avaccari@nrao.edu)"
    #define BUGZILLA        "jira.alma.cl"

    /* Prototypes */
    /* Statics */
    /* Externs */
    extern void displayVersion(void); //!< This function displays the version information


#endif /* _VERSION_H */
