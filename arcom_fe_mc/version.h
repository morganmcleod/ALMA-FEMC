/*! \file   version.h
    \brief  Version information file

    <b> File information: </b><br>
    Created: 2004/10/06 16:47:08 by avaccari
    Maintenance since 2013 by mmcleod@nrao.edu

    This file contains all the information about the current version of the
    software and its revision history.

    REVISION HISTORY

    2022-12-22 3.6.5
        Don't store cryostat timeout errors when in Troubleshooting mode

    2022-07-20 3.6.4
        Load TELEDYNE, COLLECTORP0, COLLECTORP1 keys from [PA] section
        Implement Teledyne PA algorithm
        Add messages set/get Teledyne PA options

    2022-03-04 3.6.3
        Adds FEMode=3 Simulation mode.
        Use 40 us busy wait delay in getLoAnalogMonitor, getBiasAnalogMonitor

    2022-01-19 3.6.1
        Clear parallel port EPP timeout between transactions

    2021-12-15 3.6.0
        Now boot into Operational Mode and don't bother with ppComm handshaking.
        Add command SET_PPCOMM_BYTES which overrides the default 8 bytes returned by GET_PPCOMM_TIME

    2021-07-29 3.5.3
    	Start/stop the FTP service when entering/exiting MAITENANCE_MODE

    2021-04-23 3.5.2
        In SET_CARTRIDGE[Ca]_POL[Po]_SB[Sb]_LNA_ENABLE, if Ca=Band1 or Band2 and Sb = 1, also set Sb2.

    2021-04-16 3.5.1
        SIS Sense resistor for band 5 -> 5.1

    2020-10-15 3.5.0
        Adds set/get specific cryostat TVO sensor coefficients.
        Fix console PA LIMITS report to display 1-based band numbers.

    2020-02-20 3.0.0
        Delete remaining DATABASE_RANGE code
        Undefine CHECK_HW_AVAIL: No longer reading availability and other things which don't change from INI files.
        Delete all DATABASE_HW code; delete database.h; Delete 'available' for pol, sb, cartridgeTemp, lna, lnaStage, lnaLed
        Only 3 LNA stages.
        Delete error codes MON_ERROR_RNG, MON_WARN_RNG, MON_WARN_ACT, MON_HARDW_FUT, HARDW_UPD_WARN, HARDW_RETRY
        Add error codes ERC_... to be used by all subsystems.
        Delete PLL_LOOP_BANDWIDTH_UNDEFINED
        Much less printing on startup and shutdown: only print loaded config and errors.
        Suppress errors about redirect stderr on startup; reassigned IRQ.  
        Delete OWB simulator and stored list.     
        Added console system report.
        Added ifSwitch: allChannelsHandler, cryostatTemp: sequentialCoeffHandler, monitor readback LPR EDFA coeff.
        Always allow LO PA drain voltage setting for BAND1 and BAND2.
        Create CRYO_HRS.INI file if it doesn't exist.

    2018-04-06 2.8.7
        Bugfix: fetimSerialInterface::getFetimExtTemp() stores in 
          frontend.fetim.compressor.temp[currentAsyncFetimExtTempModule] instead of [currentCompressorModule].
        Bugfix: he2press::pressHandler() returns last async value instead of reading synchronously.
        Renamed compTemp.* to fetimExtTemp.*
        Renamed all refs to "FETIM compressor temperature" to "FETIM external temperature"
        Delete DATABASE_RANGE stuff from (FETIM) compressor.c, fetimExtTemp.c, he2Press.c
        
    2017-06-27 2.8.6
        Adds check for [CRYO] AVAILABLE=N to suppress cryostat async and all m&c
        
    2017-01-08 2.8.5
        Official accepted release to the ALMA array.

    2017-01-05 2.8.4
        asyncCartridgeGoStandby2 returns ASYNC_DONE.   Should be immaterial.
        added DEBUG_GO_STANDBY2.
        simplified GoStandby2 methods to use currentModule, currentBiasModule, currentPolarizationModule instead of params.

    2016-12-31 2.8.3
    2016-12-30 2.8.2
        Rolled back cryostat cold head hours changes.

    2016-12-27 2.8.1-beta1
        Separated ASYNC_CRYO_LOG_HOURS into three async phases
        Disabled writing the CRYO_HRS file.

    2016-12-16 2.8.0-beta5
        Fixed bug in cryostatAsync(): was only repeating temp measurement once per hour?

    2016-12-14 2.8.0-beta4
        CRYOSTAT_LOG_HOURS_THRESHOLD changed to 265.0 K
        CRYOSTAT_LOG_HOURS triggers if 2 of 3 cryostat stage sensors are below the threshold.
        For testing on 2 antennas

    2016-12-08 2.8.0-beta3
        Tested on FE-12 for 2.8.0 release
        deleted schottkyMixer
        Added command to set LPR photodetector conversion coeff.  No longer loading from file.

    2016-10-20  2.6.11-beta1
        Prototype band 1 & band 2 support

    2015-07-29  2.6.10-beta1
        Prototype band 2 support

    2014-08-22  2.6.6
        Release version for continued testing at OSF
        GET_PPCOMM_TIME now returns 8 bytes of 0xFF, as the ICD says it should.
        SET_IF_SWITCH_CHANNEL[Po][Sb]_TEMP_SERVO_ENABLE now returns error if a value
          than 0 or 1 is sent.

    2014-07-28  2.6.5
        Release version for continued testing at OSF

    2014-07-23  2.006.005 beta2
        Turned off console debugging.   Commented out printfs.

    2014-07-22  2.6.5 beta1
        Testing version for FE-15 at OSF.

    2014-04-22  2.6.4 beta
        Beta release for testing at the OSF.
          Prints some debugging messages NOT SUIATABLE FOR RELEASE TO ANTENNA
        Implements LO PA max output power features.
        In TROUBLESHOOTING mode ignore cryostat temperature sensor check when
          setting LO PA drain voltage.
        In TROUBLESHOOTING mode ignore LO PA max output power restrictions.

    2013-12-10  2.6.3 beta
        Beta release for FETIM testing at the OSF.
        Modified and built by Morgan McLeod <mmcleod@nrao.edu>
        - FETIM glitch counter displays as a positive percentage.
        - FETIM added +0.25 MPa (=50 mV) offset to He2 buffer tank monitor
          value due to an offset induced by the isolation amplifier.
        - Changed turbo pump HARDWARE_BLOCKED lower temperatur 15C -> 10C.
        - Also changed TURBO_PUMP_MIN_WARN_TEMP to 15C.

    2013-07-12  2.6.2 beta
        Beta release for FETIM testing at the OSF.
        Modified and built by Morgan McLeod <mmcleod@nrao.edu>
        - FETIM He2 buffer tank is monitored asynchronously.
        - FETIM temperature sensor scaling corrected to match hardware.
        - Added bit shift one place to right when monitoring He2 buffer tank 
          and external temperature sensors.
        - Disabled DEBUG_STARTUP which was left enabled in previous release.
        - Fixed infinite loop preventing shutdown initiated by FETIM.

    Mantenance taken over by Morgan McLeod

    2012-04-13  002.006.001     (fe_mc.exe or 02-06-01.exe or 20120413.exe)
        Official Release
        This version is tagged Ver_02_06_01(ALMA-40_00_00_00-75_35_25_00_C_ICD).
        - Allowed number to power up cartridges now depend on FE operation mode:
          troubleshooting: all, all the other modes: 3.
        - Cartrdige temperature sensors: added check to verify that the measured
          voltage is within the temperature calibration curve (for the lowest
          voltage -> highest temperature).
        - Assigned PA channel mapping for bands 5 and 10.

    2011-12-12  002.006.000     (fe_mc.exe or 02-06-00.exe or 20111212.exe)
        Official Release
        This version is tagged Ver_02_06_00(ALMA-40_00_00_00-75_35_25_00_C_ICD).
        - Fully implemented FE operation mode. When in MAINTENANCE_MODE only
          the special RCA will be available. (FE-6)
        - Added automatic monitor on control RCA when control issued using
          serial console.
        - Modified code to enable a cartridge asychronously. This modifies the
          5ms latency required by the cartridge hardware from blocking to non
          blocking (Jira ticket: FE-276).
        - Incresed size for TVO sensor name field from 7 to 31 characters.
        - Modified WCA photodetector code to return the absolute value of the
          bias current (Jira ticket: FE-273).
        - Modified behavior of initialization of parallel port communication. A
          timer was implemented while waiting for the AMBSI1 to become ready. If
          the AMBSI1 is not responsive during intialization, the timer will
          expire and the bootup continue and an error will be stored. The CAN
          communication will NOT be available but the console will still be
          responsive to allow debug.
        - Added control message and console option to read ESN available on OWB.
          This allows to search for available ESN whenever is desired without
          having to reboot the module (Jira ticket: FE-298).
        - Modified behavior of gate valve behavior due to the change in hardware
          of the gate valve driver. (Jira ticket: FE-232).
        - Added shutdown procedure to the LPR to maximize safety of personel
          (Jira ticket: FE-247).
        - Implemented software interlock to prevent usage of turbo pump outside
          allowed temperature range. (Jira ticket: FE-293).
        - Implemented software interlock to prevent starting of cooldown if
          dewar pressure > 5x10e-4. Within the async loop, this will be
          continuously tested and depending on the condition, the FE status bit
          for the FETIM will be set so SAFE or UNSAFE. (Jora ticket: FE-311).

    2010-11-04  002.005.000     (fe_mc.exe or 02-05-00.exe or 2101104.exe)
        Official Release
        This version is tagged Ver_02_05_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).
        - The limits for the control message to modify the IF switch channels
          attenuation are now hardcoded and implemented. (Jira ticket: FE-270)
        - The limits for the control message to modify the YTO coarse tuning are
          now hardcoded and implemented. (Jira ticket: FE-269)
        - A direct communication test between the main board and the multiplexer
          board has been added during boot up time. This test will assure that
          the communication is established before allowing further execution of
          the code. (Jira ticket: FE-45)
        - The error counting and returning has been fixed. (Jira ticket: FE-151)
        - The limits for the control message to set the LPR optical channel are
          now hardcoded and implemented. (Jira ticket: FE-258)
        - The limits for the control message to set the IF switch cartridge are
          now hardcoded and implemented. (Jira ticket: FE-238)
        - The monitoring of the gate valve has been modified to allow monitoring
          also when the backing pump is turned off. In this case the evaluation
          of the state is based only on 2 of the 4 sensors but an evaluation of
          the current state is still possible. (Jira ticket: FE-232)
        - Modified scaling for LPR laser photodetector current and removed the
          hardware undefined error. This point will return meaningful data only
          for LPR SN.226 and above. It is not implemented in previous serial
          number. (Jira ticket: FE-233)

    2010-08-11  002.004.000     (fe_mc.exe or 02-04-00.exe or 2100811.exe)
        Official Release
        This version is tagged Ver_02_04_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).
        - Modified interface to cryostat to support both hardware revisions of
          the crysotat M&C board. The two hardware revisions require different
          scaling to be applied to the readback voltage before a conversion to
          the actual temperature can be performed. (Jira ticket: FE-220)
        - Adjusted the scaling for the PRT sensor for higher accuracy.
        - Increase to 100ms the time between the temperature channel switching
          and an the initiation of the ADC conversion in the cryostat. This
          should allow for more stable readings. This timeout doesn't affect any
          CAN interface timing since it's running in an async process.

    2010-04-28  002.003.000     (fe_mc.exe or 02-03-00.exe or 2100428.exe)
        Official Release
        This version is tagged Ver_02_03_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).
        - During initialization of the OWB, the detected ESNs will be reported
          both with the family code first and with the family code last.
          (Jira ticket: HANDOVER-879)
        - Modified interface to IF switch to support both hardware revisions of
          the IF switch M&C board. (Jira ticket: FE-158)
        - Resolved issue with IF switch temperatures monitoring returning
          errors. (Jira tickets: FE-154, AIV-1675)
        - Fixed issue that will leave timer running if error during wait for
          ADC to get ready in several subsystems.
        - Changes B9 SIS heater timeout to 10s. Also created independent
          timeouts one for each polarization.
        - Added the support for different multiplier scales via the WCA
          configuration files. (Jira ticket: FE-153)

    2009-10-13  002.002.000     (fe_mc.exe or 02-02-00.exe or 2k91013.exe)
        Official Release
        This version is tagged Ver_02_02_00(ALMA-40_00_00_00-75_35_25_00_B_ICD).
        - Modified scaling factor for LPR temperature sensors
        - Added timer to prevent band 9 heater from being turned on more than
          once every 10 second.
        - Added console switch to turn on and off async processes to allow
          easier console debugging.
        - Added 'EDFA driver status' monitor point. This will return a can
          status of -15 until the hardware is implemented
        - Changed scaling factor for the SIS heater current monitor to adapt to
          new BIAS module hardware.
        - Removed the SIS Heater Enable monitor point. This because the change
          in the BIAS module that introduced the auto shutoff causes the
          software to loose synchronization with the hardware state.
        - Reading WCA photmixer current twice (~100us) to get stable reading
        - Lowered to 5ms the timeout from power on to initialization of the CC
          and WCA
        - Implemented the error CAN interface.
        - The GET_ESN_FOUND monitor point will now reset the device index to the
          first device found.
        - The analog monitor for the BIAS and WCA now have a 40us delay between
          the selection of the channel and the beginning of the conversion. This
          was needed to fix an issue with speed of the analog circuits.
        - An asynchronous framework is in place to allow asyncronous operations
          to be performed in the background.
        - The cryostat analog monitoring is now attached to the asynchronous
          frame.
          Monitored data is stored in memory where it can be gathered with the
          standard CAN access.

    2009-05-13  002.001.003     (02-01-03.exe)
        Debug Release.
        This release is equivalent to 002.001.001. The only difference is that
        it uses the cryostat interface from revision 001.000.000.
        Tis version is tagged Ver_02_01_03

    2009-05-13  002.001.002     (02-01-02.exe)
        Debug Release.
        This release is equivalent to 002.001.000. The only difference is that
        it uses the cryostat interface from revision 001.000.000.
        This version is tagged Ver_02_01_02

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
    Documentation relative to Version: 002.006.001 */

#ifndef _VERSION_H
    #define _VERSION_H

    /* Defines */
    #define VERSION_MAJOR   3
    #define VERSION_MINOR   6
    #define VERSION_PATCH   5

    #define VERSION_DATE    "2022-12-22"
    #define VERSION_NOTES   "3.6.5: Implement Teledyne PA control.\n" \
                            "Don't store cryostat timeout errors when in Troubleshooting mode"

    #define PRODUCT_TREE    "FEND-40.04.03.03-011-A-FRM"
    #define AUTHOR          "Morgan McLeod - NRAO (mmcleod@nrao.edu)"
    #define BUGZILLA        "jira.alma.cl"

    /* Prototypes */
    /* Statics */
    /* Externs */
    extern void displayVersion(void); //!< This function displays the version information


#endif /* _VERSION_H */
