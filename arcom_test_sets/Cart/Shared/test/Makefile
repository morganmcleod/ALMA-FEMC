# $Id: Makefile,v 1.2 2013/05/02 00:10:50 jpisano Exp $
# build & install feMCTest.exe
# Targets: project - compiles feMCTest.cpp & links feMC.lib to feMCTest.exe
#	   install - copies feMCTest.exe from src to bin

project : c:\Users\jpisano\FE\frontend\firmware\arcom\tSets\Cart\Shared\test\feMCTest.exe .SYMBOLIC

!include c:\Users\jpisano\FE\frontend\firmware\arcom\tSets\Cart\Shared\test\feMCTest.mk1


#just copy .exe to bin directory
install : feMCTest.exe .SYMBOLIC
   copy feMCTest.exe c:\Users\jpisano\FE\frontend\firmware\arcom\tSets\Cart\Shared\bin  /b /y
   @echo Installing $< to../bin
