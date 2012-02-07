# NSIS installer script for the Pololu AVR C/C++ Library.
# Detects WinAVR and AVR Studio 5 and adds the library's .h and .a files
# in the appropriate directories.

# TODO: install AVR Studio 5.1 BETA and make sure this installer works with it!

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
            #File /r "${libpololu-avr}\avr_studio_5\stk500_xml\*.xml" # TODO: Don't override existing files here
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

# Checks to see if WinAVR is installed.
# Sets $WinAVRLoc to the location or "" if not found.
Function WinAVRCheck
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR" 'UninstallString'
    ${GetParent} $0 $WinAVRLoc
FunctionEnd

# The location of AVR Studio 5.0 is stored in several places:
# HKLM "SOFTWARE\Microsoft\AppEnv\10.0\Apps\AVRStudio_5.0" 'StubExePath'
# HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{60315A8A-5FCA-47CE-A856-681F3A9CDB5B}" 'InstallLocation'
# HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\avrstudio5.exe" (Default) (requires GetParent)
# HKLM "SOFTWARE\Atmel AVR\AvrStudio\5.0" 'InstallDir' 'InstallDir'   <-- does NOT work for 5.1 beta
# HKCU "SOFTWARE\Atmel\AVRStudio\5.0_Config" 'InstallDir'
#   
# The location of AVR Studio 5.1 is stored in several places:
# HKLM "SOFTWARE\Microsoft\AppEnv\10.0\Apps\AVRStudio_5.1" 'StubExePath' (requires GetParent)
# HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\{7BE9E558-BE53-4939-9565-A0BEA2F839D0}" 'InstallLocation'
# HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\App Paths\avrstudio.exe" (Default) (requires GetParent)
# HKCU "SOFTWARE\Atmel\AVRStudio51\5.1_Config" 'InstallDir'
#
# We would like to look in HKLM (HKEY_LOCAL_MACHINE) instead of HKCU (HKEY_CURRENT_USER) because
# it doesn't really make sense to look at user settings to find where an application installed.
# We would also like to use a registry key that appears to be used consistently between the
# different versions of AVR Studio 5 so it is easy to update our installers.
# We would also like one that is predictable (e.g. doesn't have a GUID in it).
# The only one that satisfies all of those criteria is the AppEnv one, so we will use that.

# Checks to see if AVR Studio 5 is installed.
# Sets $AS5Loc to the location or "" if not found.
Function AS5Check
    ReadRegStr $AS5Loc HKCU "SOFTWARE\Atmel\AVRStudio\5.0_Config" 'InstallDir'
FunctionEnd

Function VSIXCheck
    ReadRegStr $VSIXLoc HKCU "SOFTWARE\Atmel\AvrStudio\5.0_Config" 'ShellFolder'
FunctionEnd
