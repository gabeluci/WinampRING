!include "UpgradeDLL.nsh"
!include "MUI.nsh"

; The name of the installer
Name "WinampRING 1.1"
Var NAME
Function .onInit
  StrCpy $NAME "WinampRING 1.1" ; this should always be the same as the name of the installer
FunctionEnd

; The file to write
OutFile "WinampRING.exe"

; The default installation directory
InstallDir $PROGRAMFILES\Winamp

InstallDirRegKey HKCU "Software\WinampRING" "InstallPath"

;--------------------------------
    !define MUI_PAGE_HEADER_TEXT "Select Winamp folder"
    !define MUI_PAGE_HEADER_SUBTEXT "Select the folder Winamp is installed to."
    !define MUI_DIRECTORYPAGE_TEXT_TOP "Setup will install $NAME to Winamp's plugin folder. If Winamp is installed in a different folder, click Browse and select the correct folder. Click Install to start the installation."
    !define MUI_DIRECTORYPAGE_TEXT_DESTINATION "Winamp folder"

  !insertmacro MUI_PAGE_DIRECTORY
  !insertmacro MUI_PAGE_INSTFILES

    !define MUI_FINISHPAGE_RUN $INSTDIR\winamp.exe
    !define MUI_FINISHPAGE_RUN_TEXT "Run Winamp"
    
    !define MUI_FINISHPAGE_SHOWREADME $INSTDIR\Plugins\Readme_WinampRING.txt
    !define MUI_FINISHPAGE_SHOWREADME_TEXT "View Readme file"

    !define MUI_FINISHPAGE_LINK "Visit author's website"
    !define MUI_FINISHPAGE_LINK_LOCATION "http://www.luci.ca/"
    
    !define MUI_FINISHPAGE_NOAUTOCLOSE

  !insertmacro MUI_PAGE_FINISH

;---------------------------------
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES

;---------------------------------
  !insertmacro MUI_LANGUAGE "English"

Section ""
  ; Set output path to the installation directory.
  SetOutPath $INSTDIR\Plugins\

  ; Put file there
  File "..\Release\gen_WinampRING.dll"
  File "..\Readme_WinampRING.txt"

  WriteRegStr HKCU "Software\WinampRING" "InstallPath" "$INSTDIR"
  WriteRegStr HKCU "Software\WinampRING" "Volume" "25"
  WriteRegStr HKCU "Software\WinampRING" "COMPort" "1"
  WriteRegStr HKCU "Software\WinampRING" "Percent" "0"
  WriteRegStr HKCU "Software\WinampRING" "Popup" "0"
  WriteRegStr HKCU "Software\WinampRING" "ChWinVol" "0"
  WriteRegStr HKCU "Software\WinampRING" "Enabled" "1"
  WriteRegStr HKCU "Software\WinampRING" "Action" "122"
  WriteRegStr HKCU "Software\WinampRING" "CallerID" "0"
  WriteRegStr HKCU "Software\WinampRING" "CallerIDInit" "AT#CID=1$\r"

  ; Write the uninstall keys for Windows
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinampRING" "DisplayName" "$NAME"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinampRING" "UninstallString" '"$INSTDIR\Plugins\un_WinampRING.exe"'
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinampRING" "NoModify" 1
  WriteRegDWORD HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinampRING" "NoRepair" 1
  WriteUninstaller $INSTDIR\Plugins\"un_WinampRING.exe"
  
SectionEnd

Section "Uninstall"

  ; Remove registry keys
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\WinampRING"
  DeleteRegKey HKCU "Software\WinampRING"

  ; Remove files and uninstaller
  Delete $INSTDIR\gen_WinampRING.dll
  Delete $INSTDIR\Readme_WinampRING.txt
  Delete $INSTDIR\un_WinampRING.exe

SectionEnd
