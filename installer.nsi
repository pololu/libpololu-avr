# NSIS installer script for the Pololu AVR C/C++ Library.
# Detects WinAVR and AVR Studio 5 and adds the library's .h and .a files
# in the appropriate directories.

!define LIB_VER "YYMMDD"

!define libpololu-avr "."

SetCompressor /solid lzma

!include LogicLib.nsh
!include nsDialogs.nsh
!include FileFunc.nsh

RequestExecutionLevel admin
OutFile "libpololu-avr-${LIB_VER}.exe"
Name "Pololu AVR C/C++ Library"
InstallDir "C:\libpololu-avr\"
ShowInstDetails show
AllowSkipFiles on
XPStyle on

Page directory
#Page custom optionsPage
Page instfiles

Var /GLOBAL WinAVRLoc
Var /GLOBAL AS5Loc
Var /GLOBAL VSIXLoc
Var Dialog
Var Label
Var Text

Function optionsPage
    nsDialogs::Create 1018
    Pop $Dialog
    ${If} $Dialog == error
        Abort
    ${EndIf}
    
    ${NSD_CreateLabel} 0 0 100% 12u "Hello, welcome to nsDialogs!"
    Pop $Label

    ${NSD_CreateText} 0 13u 100% -13u "Type something here..."
    Pop $Text
    
    nsDialogs::Show
FunctionEnd

Section
    SetOutPath "$INSTDIR"
    File /r /x libpololu-avr-*.exe "${libpololu-avr}\*.*"
    Call WinAVRCheck
    StrCmp $WinAVRLoc "" no_winavr
            DetailPrint "Detected WinAVR.  Installing library in it..."
	    DetailPrint "WinAVR = $WinAVRLoc" # tmphax
            #TODO: SetOutPath "$WinAVRLoc\avr\lib"
            #TODO: File "${libpololu-avr}\libpololu_*.a"
            #TODO: SetOutPath "$WinAVRLoc\avr\include"
            #TODO: File /r "${libpololu-avr}\pololu"
            goto end_winavr
        no_winavr:
            DetailPrint "Did not detect WinAVR."
            goto end_WinAVR
        end_winavr:
        
        
    Call AS5Check
    StrCmp $AS5Loc "" no_as5
            DetailPrint "Detected AVR Studio 5.  Installing library in it..."
	    DetailPrint "AVR Studio 5 = $AS5Loc" # tmphax
            #SetOutPath "$AS5Loc\AVR Toolchain\avr\lib"
            #File "${libpololu-avr}\libpololu_*.a"
            #SetOutPath "$AS5Loc\AVR Toolchain\avr\include"
            #File /r "${libpololu-avr}\pololu"
            #SetOutPath "$AS5Loc\tools\STK500\xml"
            #File /r "${libpololu-avr}\avr_studio_5"
            goto end_as5
        no_as5:
            DetailPrint "Did not detect AVR Studio 5."
            goto end_as5
        end_as5:
    
    Call VSIXCheck
    StrCmp $VSIXLoc "" no_vsix
        DetailPrint "Detected Visual Studio 10.  Installing extension..."
	DetailPrint "VSIXLoc = $VSIXLoc"
        # TODO: ExecWait "$VSIXLoc\vsixinstaller.exe /skuName:AvrStudio /skuVersion:5.0 /quiet Desktop\extension.vsix"
        Goto end_vsix
    no_vsix:
        DetailPrint "Visual Studio 10 not detected, skipping extension install."
        Goto end_vsix
    end_vsix:
SectionEnd

Function WinAVRCheck
    ReadRegStr $R0 HKLM "Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR" 'UninstallString'
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR" 'UninstallString'
    ReadRegStr $R0 HKLM "Software\Wow6432Node\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR-20100110" 'UninstallString'
    ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR-20100110" 'UninstallString'
    ; ReadRegStr $R0 HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\blahblahblah" 'UninstallString'
    ${GetParent} $R0 $WinAVRLoc
    ; MessageBox MB_OKCANCEL "Contents of $$WinAVRLoc $WinAVRLoc" IDOK okay IDCANCEL cancel
        ; cancel:
        ; Quit
        ; okay:
    ; MessageBox MB_OKCANCEL "Contents of R0 - $R0, contents of 0 - $0" IDOK okay IDCANCEL cancel
        ; cancel:
        ; Quit
        ; okay:
        
    ; StrCmp $ "" isempty isnotempty
        ; isempty:
        ; MessageBox MB_OK "$$0 was $0 in isempty"
        ; StrCpy
        ; goto end

        ; isnotempty:
        ; MessageBox MB_OK "$$0 was $0 in isnotempty" 
        ; goto end

        ; end:
        
FunctionEnd

Function AS5Check
    ReadRegStr $R0 HKCU "Software\Atmel\AVRStudio\5.0_Config" 'InstallDir'
    ; Push $R0
    ; Pop $0
    StrCpy $AS5Loc $R0
FunctionEnd

Function VSIXCheck
    ReadRegStr $R0 HKCU "Software\Microsoft\VisualStudio\10.0_Config" 'InstallDir'
    ReadRegStr $R0 HKCU "Software\Microsoft\VCSExpress\10.0_Config" 'InstallDir'
    StrCpy $VSIXLoc $R0
FunctionEnd
