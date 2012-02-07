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
InstallDir "C:\libpololu-avr"
ShowInstDetails show
AllowSkipFiles on
XPStyle on

Page directory
Page custom optionsPage optionsPageLeave
Page instfiles

Var /GLOBAL VSShellPath
Var Dialog

# Toolchain variables
Var WinAVRPath
Var AS50Path
Var AS51Path

Var WinAVRBox
Var AS50Box
Var AS51Box

Var WinAVRIntegrate
Var AS50Integrate
Var AS51Integrate

# When the installer starts, detect the different things we need to detect.
Function .onInit
    Call WinAVRCheck
    Call AS5Check
    Call VSShellCheck
    
    # Check the assumptions we make when calling NSD_GetState and NSD_SetState.
    ${If} ${BST_CHECKED} != 1
      Abort
    ${EndIf}
    ${If} ${BST_UNCHECKED} != 0
      Abort
    ${EndIf}
    
    # By default, any checkbox that is available will be checked.
    StrCpy $WinAVRIntegrate "1"
    StrCpy $AS50Integrate "1"
    StrCpy $AS51Integrate "1"
    
    DetailPrint "Hello"
FunctionEnd

!macro CreateToolchainCheckBox Name Path Checked Box Y
    ${If} "${Path}" != ""
      StrCpy $1 "${Name} (${Path})"
      StrCpy $2 1                    # enable the checkbox
      StrCpy $3 "${Checked}"         # check the box if appropriate
    ${Else}
      StrCpy $1 "${Name} (not detected)"
      StrCpy $2 0                    # disable the checkbox
      StrCpy $3 0                    # uncheck the checkbox
    ${Endif}
    ${NSD_CreateCheckBox} 0 ${Y} 100% 12u $1
    Pop ${Box}
    EnableWindow ${Box} $2     # disable/enable the checkbox
    ${NSD_SetState} ${Box} $3
!macroend

# Creates the optionsPage and all the checkboxes in it.    
Function optionsPage
    nsDialogs::Create 1018
    Pop $Dialog
    ${If} $Dialog == error
        Abort
    ${EndIf}
    
    GetFunctionAddress $0 "optionsPageLeave"
    nsDialogs::OnBack $0
    
    ${NSD_CreateLabel} 0 0 100% 12u "Install the library as part of the following toolchains:"
    Pop $0

    !insertmacro CreateToolchainCheckBox "WinAVR" $WinAVRPath $WinAVRIntegrate $WinAVRBox 20u
    !insertmacro CreateToolchainCheckBox "AVR Studio 5.0" $AS50Path $AS50Integrate $AS50Box 40u
    !insertmacro CreateToolchainCheckBox "AVR Studio 5.1" $AS51Path $AS51Integrate $AS51Box 60u
    
    nsDialogs::Show
FunctionEnd

# Gets called when leaving the options page (forward or back)
# Stores the user's choices from the check boxes.
Function optionsPageLeave
  ${NSD_GetState} $WinAVRBox $WinAVRIntegrate
  ${NSD_GetState} $AS50Box $AS50Integrate
  ${NSD_GetState} $AS51Box $AS51Integrate
FunctionEnd

Section
    SetOutPath "$INSTDIR"
    File /r /x libpololu-avr-*.exe "${libpololu-avr}\*.*"
    Call WinAVRCheck
    StrCmp $WinAVRPath "" no_winavr
            DetailPrint "Detected WinAVR.  Installing library in it..."
	    DetailPrint "WinAVR = $WinAVRPath" # tmphax
            #TODO: SetOutPath "$WinAVRPath\avr\lib"
            #TODO: File "${libpololu-avr}\libpololu_*.a"
            #TODO: SetOutPath "$WinAVRPath\avr\include"
            #TODO: File /r "${libpololu-avr}\pololu"
            goto end_winavr
        no_winavr:
            DetailPrint "Did not detect WinAVR."
            goto end_WinAVR
        end_winavr:
        
        
    StrCmp $AS50Path "" no_as5
            DetailPrint "Detected AVR Studio 5.0.  Installing library in it..."
            #SetOutPath "$AS50Path\AVR Toolchain\avr\lib"
            #File "${libpololu-avr}\libpololu_*.a"
            #SetOutPath "$AS50Path\AVR Toolchain\avr\include"
            #File /r "${libpololu-avr}\pololu"
            #SetOutPath "$AS50Path\tools\STK500\xml"
            #File /r "${libpololu-avr}\avr_studio_5\stk500_xml\*.xml" # TODO: Don't override existing files here
            goto end_as5
        no_as5:
            DetailPrint "Did not detect AVR Studio 5.0."
            goto end_as5
        end_as5:
    
    StrCmp $VSShellPath "" no_vsix
        DetailPrint "Detected Visual Studio 10.  Installing extension..."
        # TODO: ExecWait "$VSShellPath\vsixinstaller.exe /skuName:AvrStudio /skuVersion:5.0 /quiet Desktop\extension.vsix"
        Goto end_vsix
    no_vsix:
        DetailPrint "Visual Studio 10.0 Isolated Shell not detected, skipping extension install."
        Goto end_vsix
    end_vsix:
    
SectionEnd

# Checks to see if WinAVR is installed.
# Sets $WinAVRPath to the Pathation or "" if not found.
Function WinAVRCheck
    # ReadRegStr $WinAVRVer HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR" 'DisplayVersion'
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion\Uninstall\WinAVR" 'UninstallString'
    ${GetParent} $0 $WinAVRPath
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
# Sets $AS50Path and $AS51Path to the location or "" if not found.
Function AS5Check
    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\AppEnv\10.0\Apps\AVRStudio_5.0" 'StubExePath'
    ${GetParent} $0 $AS50Path

    ReadRegStr $0 HKLM "SOFTWARE\Microsoft\AppEnv\10.0\Apps\AVRStudio_5.1" 'StubExePath'
    ${GetParent} $0 $AS51Path
FunctionEnd

# We need to find the location of the Visual Studio 2010 isolated shell.
# This is available in HKCU "Software\Atmel\AVRStudio\5.0_Config" 'ShellFolder' or
# in HKCU "Software\Atmel\AVRStudio51\5.1_Config" 'ShellFolder', but that is a bad place
# to look for the reasons above.  The registry key suggested by Microsoft is better, but
# the "1033" number in there does assume the user is using English.
#  HKLM "Software\Microsoft\VisualStudio\10.0\Setup\IsoShell\1033" 'ProductDir'
#  http://msdn.microsoft.com/en-us/library/bb932484.aspx
Function VSShellCheck
    ReadRegStr $VSShellPath HKLM "Software\Microsoft\VisualStudio\10.0\Setup\IsoShell\1033" 'ProductDir'
    DetailPrint "tmphax: found shell at $VSShellPath"
    StrCmp $VSShellPath "" 0 VSShellCheckDone
    ReadRegStr $VSShellPath HKCU "SOFTWARE\Atmel\AvrStudio\5.0_Config" 'ShellFolder'
    VSShellCheckDone:
FunctionEnd
