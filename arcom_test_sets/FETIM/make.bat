@ECHO off
wcl -4 -fp5 -bcl=COM -ox -mt FETIM.c

GOTO:EOF

if ERRORLEVEL 1 GOTO:EOF

@ftp -i -s:"%~f0"&GOTO:EOF
open 10.12.96.254
su
su
cd ALMA
bin
put FETIM.com
disconnect
bye
