# -*- coding: utf-8 -*-
"""
Created on Thu Aug  8 14:29:02 2019

@author: mmcleod@nrao.edu
Morgan McLeod

class FEMCFirmwareUpdate
A class to acceess the FTP server running on FEMC modules with 3.x firmware.
Has methods to:
 set/get the executable name that will run on next reboot.
 upload an executable or ini file
 download an executable or ini file
"""

from ftplib import FTP
import io
import os.path

class FEMCFirmwareUpdate:
    """A class to acceess the FTP server running on FEMC modules with 3.x firmware."""
    
    def __init__(self, host=''):
        """Constructor
        If 'host' is provided, this will automatically call connect()
        """
        self.host = host
        self.ftp = None
        if self.host:
            self.connect();
            
    def __del__(self):
        """ Descructor
        Calls disconnect()
        """
        self.disconnect()

    def connect(self, host=''):
        """Connect to the ftp host.
        Parameter 'host' can be provided here whether or not it was provided in the constructor.
        Connects using the 'update' login which only has acces to the FEMC module's C:\ALMA directory.
        """
        if host:
            self.host = host
        self.ftp = FTP(self.host)
        self.ftp.login('update', 'update', 'update')
        
    def disconnect(self):
        """Disconnect from the ftp host."""
        if self.ftp:
            self.ftp.quit();
            self.ftp = None
        
    def getExeName(self):
        """Fetch the contents of the RUN_FEMC.BAT file.
        This single-line batch file controls which firmware executable will run on next reboot.
        Returns a string containing the executable name - the first line of the batch file.
        """
        # define a callback to handle the retrieved lines:
        def cb(line, lines):            
            lines.append(line)
        # retrieve the contents of the batch file into 'lines':
        lines = [];
        self.ftp.retrlines('retr RUN_FEMC.BAT', lambda line: cb(line, lines))
        return lines[0]
    
    def setExeName(self, name):
        """Set the contents of the RUN_FEMC.BAT file.
        This single-line batch file controls which firmware executable will run on next reboot.
        Pass the executable name as the 'name' parameter.
        """
        lines = io.BytesIO(str.encode(name + '\n'))
        self.ftp.storlines('stor RUN_FEMC.BAT', lines)
        lines.close()
        
    def putFile(self, filename):
        """Upload a file from the local file system to the FEMC module.
        The file will be placed in the FEMC modules's C:\ALMA directory
        Any existing file with the same name will be replaced.
        The 'filename' parameter can be a simple filename or a full path to the source file.
        """
        basename = os.path.basename(filename)
        fp = open(filename, 'rb')
        self.ftp.storbinary('stor ' + basename, fp)
        fp.close();
        
    def getFile(self, filename):
        """Download a file from the FEMC module to the local file system.
        The file will be placed in the current working directory.
        The 'filename' parameter can be a simple name of a file.  Directory C:\ALMA is assumed.
        """
        # define a callback to handle the retrieved blocks:
        def cb(block, fp):
            fp.write(block)
        # retrieve the contents of the file and save locally:
        basename = os.path.basename(filename)
        fp = open(basename, 'wb')
        self.ftp.retrbinary('retr ' + filename, lambda block: cb(block, fp))
        fp.close()
