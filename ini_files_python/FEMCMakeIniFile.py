# -*- coding: utf-8 -*-
"""
Created on Thu Dec  5 11:52:27 2019

@author: mmcleod
"""

from xml.etree import ElementTree
from enum import IntEnum
import os.path

class fwVersion(IntEnum):
    """Enumeration of supported firmware versions.  Use the nearest, rounded down""" 
    V2_5 = 25
    V2_6 = 26
    V2_8 = 28
    V2_9 = 29
    V3_0 = 30
    DEFAULT = V2_8

class FEMCMakeIniFile:
    """A class to format the INI files 
    CARTn.INI, CRYO.INI, FRONTEND.INI, LPR.INI, WCAn.INI
    to be loaded onto the FEMC module flash disk.
    These are required for FEMC firmware 2.8.7 and before.
    They are optional for FEMC firmware 3.0 and after."""
    
    def __init__(self):
        self.reset()
        self.fwVersion = fwVersion.DEFAULT
                
    def reset(self):
        """Reset the variables we expect to load from a file or outside source."""
        self.band = 0
        self.kind = None
        self.ESN = None
        self.PA_LIMITS = []
                    
    def setFwVersion(self, ver):
        if isinstance(ver, fwVersion):
            self.fwVersion = ver
        else:
            print("setFwVersion: unsupported version '{}'.".format(ver))
    
    VER_STRINGS = {
       fwVersion.V2_5 : "2.5.x",
       fwVersion.V2_6 : "2.6.x",
       fwVersion.V2_8 : "2.8.x",
       fwVersion.V2_9 : "2.9.x",
       fwVersion.V3_0 : "3.0.x"
    }
    
    def getFwVersionString(self):
        return self.VER_STRINGS.get(self.fwVersion, "unknown")

    ASSEMBLY = {            
        # a dict mapping the ASSEMBLY tag to (kind, band):
        'CCA1':  ('CCA', 1),
        'CCA2':  ('CCA', 2),
        'CCA3':  ('CCA', 3),
        'CCA4':  ('CCA', 4),
        'CCA5':  ('CCA', 5),
        'CCA6':  ('CCA', 6),
        'CCA7':  ('CCA', 7),
        'CCA8':  ('CCA', 8),
        'CCA9':  ('CCA', 9),
        'CCA10': ('CCA', 10),
        'WCA1':  ('WCA', 1),
        'WCA2':  ('WCA', 2),
        'WCA3':  ('WCA', 3),
        'WCA4':  ('WCA', 4),
        'WCA5':  ('WCA', 5),
        'WCA6':  ('WCA', 6),
        'WCA7':  ('WCA', 7),
        'WCA8':  ('WCA', 8),
        'WCA9':  ('WCA', 9),
        'WCA10': ('WCA', 10),
        'LPR':   ('LPR', 0),
        'CRYOSTAT': ('CRYO', 0),
        'FRONTEND': ('FRONTEND', 0)
    }

    def setXmlFile(self, filename):
        """Read an XML file and save things that may be needed for the INI file output."""
        # reset things we expect to parse:
        self.reset()        
        
        # parse the provided file. TODO: try/except?
        tree = ElementTree.parse(filename)
        
        # confirm that it is an ALMA ConfigData structure:
        root = tree.getroot()
        if root.tag != 'ConfigData':
            print("No ConfigData found.")
            return
        
        # what assembly is this?
        assy = root.find('ASSEMBLY')
        if assy is None:
            print("No ASSEMBLY found.")
            return
        
        assyName = assy.get('value')
        if assyName is None:
            print("No ASSEMBLY.value found.")
            return
        
        # find assyName in the ASSEMBLY dict:        
        (self.kind, self.band) = self.ASSEMBLY.get(assyName, (None, 0))
        if self.kind is None:
            print("Unsupported ASSEMBLY:", assyName)
            return
        
        ESN = root.find('ESN')
        if ESN is not None:
            ESN = ESN.get('hex')
        if ESN is None:
            ESN = ''
        
        self.ESN = ESN
            
        if self.kind == 'WCA':
            # load the PA_LIMITS entries from XML:
            for entry in root.findall('PowerAmpLimit'):
                count = entry.get('count')
                VD0 = entry.get('VD0')
                VD1 = entry.get('VD1')
                self.PA_LIMITS.append({
                        'count': count, 
                        'VD0': VD0,
                        'VD1': VD1
                })
        
        else:
            print("setXmlFile:", self.kind, "is not implemented.")
            return
        
    def makeIniText(self):
        """Return a string formatted as the required for FEMC module INI file."""
        
        result = ""
        
        if self.kind == 'WCA':
            
            # write header section:
            result += (";\n"
                       "; WCA configuration file")            
            result +=  " for FEMC firmware version {}\n".format(
                               self.getFwVersionString())
            result +=  ";\n"
            
            # write INFO section:
            if self.fwVersion <= fwVersion.V2_6:
                result += ("\n[INFO]\n"
                           "; This section is required for firmware 2.6 and older.\n"
                           "ESN={}\n".format(self.ESN))
            
            # write PLL section:
            if self.fwVersion <= fwVersion.V2_6:
                loopBW = '9';     # undefined/don't care
                if self.band in (2, 3, 5, 6, 7, 10):
                    loopBW = '1'   # 15 MHz/V
                elif self.band in (4, 8, 9):
                    loopBW = '0'  # 30 MHz/V
                
                result += ("\n[PLL]\n"
                           "; This section is required for firmware 2.6 and older.\n"
                           "; PLL loop bandwidth select (0 = 7.5MHz/V, 1 = 15 MHz/V, 9 = undefined)\n")
                result +=  "LOOP_BW={}\n".format(loopBW)
        
            # write SCALING section:
            if self.fwVersion <= fwVersion.V2_6:
                result += ("\n[SCALING]\n"
                           "; These settings are the same for all bands.\n"
                           "; Nonetheless, this section is required for firmware 2.6 and older.\n"
                           "PLL_LOCK=19.09090909\n"
                           "PLL_CORR=19.09090909\n"
                           "SUPPLY_V=20.0\n"
                           "MULT_C=100.0\n"
                           "PLL_YIG_C_SCALE=400.0\n"
                           "PLL_YIG_C_OFFSET=150.0\n")
            
            # write PA_LIMITS section:
            if self.fwVersion >= fwVersion.V2_6:
                result += ("\n[PA_LIMITS]\n"
                           "; Output power limits section is supported but not required for firmware 2.6 and newer.\n"
                           "; The firmware will interpolate between table rows.  Format is:\n"
                           ";\n"
                           "; ENTRY_n=count, VD0 limit, VD1 limit\n"
                           ";\n"
                           "; Where count is a raw YTO tuning word and the limits are VD *set* values.\n")
                result += "ESN={}\n".format(self.ESN)
                result += "ENTRIES={}\n".format(len(self.PA_LIMITS))
                
                num = 0;
                for entry in self.PA_LIMITS:
                    num += 1
                    result += "ENTRY_{num}={count}, {VD0}, {VD1}\n".format(
                            num = num, 
                            count = entry['count'],
                            VD0 = entry['VD0'],
                            VD1 = entry['VD1'])
        else:
            print("makeIniText:", self.kind, "is not implemented.")
        return result
    
    def writeIniFile(self, targetDir = "."):
        """Write out an INI file suitable for use on the FEMC module to the given targetDir."""
        if not os.path.isdir(targetDir):
            print("writeIniFile: {} is not a directory.".format(targetDir))
            
        if self.kind == 'WCA':
            filename = os.path.join(targetDir, "WCA{}.INI".format(self.band))
        elif self.kind == 'CCA':
            filename = os.path.join(targetDir, "CCA{}.INI".format(self.band))
        elif self.kind == 'CRYO':
            filename = os.path.join(targetDir, "CRYO.INI")
        elif self.kind == 'LPR':
            filename = os.path.join(targetDir, "LPR.INI")
        else:
            print("writeIniFile: unsupported component: '{}'.".format(self.kind)) 
            return
            
        fp = open(filename, 'w')
        fp.write(self.makeIniText())
        fp.close()

def test_FEMCMakeIniFile():
    fe = FEMCMakeIniFile()
    #fe.setFwVersion(fwVersion.V2_6)
    fe.setXmlFile("./test/92884882929221748.xml")
    fe.writeIniFile("./test")
