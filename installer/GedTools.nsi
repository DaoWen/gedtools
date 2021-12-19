######################
# GedTools installer #
######################

;--------------------------------
; General

  ;Name and file
  Name "GedTools"
  OutFile "GedTools-20211219-win32-installer.exe"

  ;Default installation folder
  InstallDir $PROGRAMFILES\GedTools

  ;Request application privileges for Windows Vista
  RequestExecutionLevel admin

  ; Compression type
  SetCompressor lzma

;--------------------------------
; Languages

LoadLanguageFile "${NSISDIR}\Contrib\Language files\English.nlf"
LoadLanguageFile "${NSISDIR}\Contrib\Language files\TradChinese.nlf"

;--------------------------------
; Message Strings
LangString CloseGedToolsMsg ${LANG_ENGLISH} \
  "GedTools.exe is currently running.$\nWould you like to close it and continue installing?"
LangString CloseGedToolsMsg ${LANG_TRADCHINESE} \
  "「GedTools.exe」正在執行中。$\n您想要關閉「GedTools」然後繼續安裝嗎？"

;--------------------------------
; CheckAppRunning Macro
; Reference:
; http://nsis.sourceforge.net/Talk:Check_whether_your_application_is_running_during_uninstallation
; https://nsis.sourceforge.io/NsProcess_plugin
!macro CheckAppRunning
  DetailPrint "Searching for running instances of GedTools.exe"
  nsProcess::_FindProcess "GedTools.exe"
  Pop $R0
  IntCmp $R0 0 AppRunning
  Goto AppNotRunning

  AppRunning:
    DetailPrint "-> Found GedTools.exe running"
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON1 "$(CloseGedToolsMsg)" IDYES KillApp
    DetailPrint "Installation Aborted!"
    Abort
  KillApp:
    DetailPrint "Killing all running instances of GedTools.exe"
    nsProcess::_KillProcess "GedTools.exe"
    Pop $R0
    DetailPrint "-> Killing GedTools process returned code $R0"
    IntCmp $R0 0 KillSuccess
	Goto KillFail
  KillSuccess:
    sleep 1500
    Goto End
  KillFail:
    DetailPrint "-> Error in KillProc"
    Abort
  AppNotRunning:
    DetailPrint "No running instances of GedTools.exe found"
  End:
!macroend

;--------------------------------
; Installer
Section
    # Ensure GedTools.exe is not running first
    !insertmacro CheckAppRunning
    # Set output directory
    SetOutPath $INSTDIR\lang
    # Install files to output directory
    File ..\lang\GedTools_en.qm
    File ..\lang\GedTools_zh.qm
    File ..\lang\qt_zh.qm
    # Set output directory
    SetOutPath $INSTDIR
    # Install files to output directory
    File ..\bin\GedTools.exe
    File ..\gpl-3.0.txt
    File ..\PinyinMap.dat
    File ..\Readme.md
    File libgcc_s_dw2-1.dll
    File libstdc++-6.dll
    File libwinpthread-1.dll
    File QtCore4.dll
    File QtGui4.dll
    File QtNetwork4.dll
    # Create Uninstaller
    WriteUninstaller $INSTDIR\Uninstall.exe
    # Create Start Menu Entries
    CreateDirectory $SMPROGRAMS\GedTools
    CreateShortCut  $SMPROGRAMS\GedTools\GedTools.lnk $INSTDIR\GedTools.exe
    CreateShortCut  $SMPROGRAMS\GedTools\Uninstall.lnk $INSTDIR\Uninstall.exe
    # Create Desktop Shortcut
    CreateShortCut  $DESKTOP\GedTools.lnk $INSTDIR\GedTools.exe
    # Create Add/Remove Programs Registry Entry
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GedTools" \
                     "DisplayName" "GedTools"
    WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GedTools" \
                     "UninstallString" "$\"$INSTDIR\Uninstall.exe$\""
SectionEnd

;--------------------------------
; Uninstaller
Section "Uninstall"
    # Delete installed files
    Delete $INSTDIR\uninstall.exe
    Delete $INSTDIR\GedTools.exe
    Delete $INSTDIR\gpl-3.0.txt
    Delete $INSTDIR\libgcc_s_dw2-1.dll
    Delete $INSTDIR\libstdc++-6.dll
    Delete $INSTDIR\libwinpthread-1.dll
    Delete $INSTDIR\PinyinMap.dat
    Delete $INSTDIR\QtCore4.dll
    Delete $INSTDIR\QtGui4.dll
    Delete $INSTDIR\QtNetwork4.dll
    Delete $INSTDIR\Readme.md
    Delete $INSTDIR\Changes.txt
    Delete $INSTDIR\lang\GedTools_en.qm
    Delete $INSTDIR\lang\GedTools_zh.qm
    Delete $INSTDIR\lang\qt_zh.qm
    RMDir  $INSTDIR\lang
    RMDir  $INSTDIR
    Delete $SMPROGRAMS\GedTools\GedTools.lnk
    Delete $SMPROGRAMS\GedTools\Uninstall.lnk
    RMDir  $SMPROGRAMS\GedTools
    Delete $DESKTOP\GedTools.lnk
    # Remove Add/Remove Programs Registry Entry
    DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\GedTools"
SectionEnd
