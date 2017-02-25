@echo off

SET ORICUTRON="..\..\..\oricutron\"

SET ORIGIN_PATH=%CD%
SET PATH=%PATH%;%CC65%

md release
md release\usr\
md release\usr\bin\

cl65 -o release\usr\bin\orickong -DTELEMON -DCC65 -ttelestrat orickong.c advanced_print_ca65.s ..\oric-common\lib\ca65\telestrat\graphics.s  ..\oric-common\lib\ca65\telestrat\music.s ..\oric-common\lib\ca65\telestrat\print.s ..\oric-common\lib\ca65\telestrat\returnline.s ..\oric-common\lib\ca65\telestrat\hires.s ..\oric-common\lib\ca65\telestrat\play.s ..\oric-common\lib\ca65\telestrat\sound.s ..\oric-common\lib\ca65\telestrat\keyboard.s


IF "%1"=="NORUN" GOTO End
copy release\usr\bin\orickong %ORICUTRON%\usbdrive\usr\bin

cd %ORICUTRON%
OricutronV4 -mt -d teledisks\stratsed.dsk
cd %ORIGIN_PATH%
:End


