set INCLUDE=%WATCOM%/h
wcl -w4 -zq -od -d1 -5 -bt=dos -fo=.obj -ml testPpComm.c timer.c
copy TestPpComm.exe T_PPCOM.EXE


