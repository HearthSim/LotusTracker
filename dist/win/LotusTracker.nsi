;--------------------------------
;Include Modern UI

  !include "MUI2.nsh"

;--------------------------------
;General

  ;Name and file
  Name "Lotus Tracker 1.4.5"
  OutFile "Lotus Tracker 1.4.5.exe"

  ;Default installation folder
  InstallDir "$PROGRAMFILES\Lotus Tracker"
  
  ;Get installation folder from registry if available
  InstallDirRegKey HKCU "Software\Lotus Tracker" ""

  ;Request application privileges
  RequestExecutionLevel admin

;--------------------------------
;Variables

  Var StartMenuFolder

;--------------------------------
;Interface Configuration

  !define MUI_ICON "files\logo.ico"
  !define MUI_HEADERIMAGE
  !define MUI_HEADERIMAGE_BITMAP "files\logo.bmp" ; optional
  !define MUI_ABORTWARNING

;--------------------------------
;Pages

  !insertmacro MUI_PAGE_WELCOME
  !insertmacro MUI_PAGE_LICENSE "files\copyright.txt"
  !insertmacro MUI_PAGE_DIRECTORY

  ;Start Menu Folder Page Configuration
  !define MUI_STARTMENUPAGE_REGISTRY_ROOT "HKCU" 
  !define MUI_STARTMENUPAGE_REGISTRY_KEY "Software\Modern UI Test" 
  !define MUI_STARTMENUPAGE_REGISTRY_VALUENAME "Start Menu Folder"
  
  !insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
  
  !insertmacro MUI_PAGE_INSTFILES

  # These indented statements modify settings for MUI_PAGE_FINISH
  !define MUI_FINISHPAGE_NOAUTOCLOSE
  !define MUI_FINISHPAGE_RUN "$INSTDIR\LotusTracker.exe"
  !define MUI_FINISHPAGE_RUN_TEXT "Start Lotus Tracker"
  !insertmacro MUI_PAGE_FINISH
  
  !insertmacro MUI_UNPAGE_WELCOME
  !insertmacro MUI_UNPAGE_CONFIRM
  !insertmacro MUI_UNPAGE_INSTFILES
  !insertmacro MUI_UNPAGE_FINISH
  
;--------------------------------
;Languages
 
  !insertmacro MUI_LANGUAGE "English"

;--------------------------------
;Installer Sections

Section "Lotus Tracker" SecLotusTracker

  ExecWait `taskkill /f /im LotusTracker.exe`

  SetOutPath "$INSTDIR"
  
  File files\libcrypto.dll
  File files\libcrypto-1_1.dll
  File files\libeay32.dll
  File files\libssl.dll
  File files\libssl-1_1.dll
  File files\libgcc_s_dw2-1.dll
  File files\libqt5keychain.dll
  File files\libstdc++-6.dll
  File files\libwinpthread-1.dll
  File files\ssleay32.dll
  File files\Qt5Core.dll
  File files\Qt5Gui.dll
  File files\Qt5Network.dll
  File files\Qt5Widgets.dll
  File files\WinSparkle.dll
  File files\LotusTracker.exe

  SetOutPath "$INSTDIR\bin"
  File files\bin\tar.exe
  File files\bin\xz.exe

  SetOutPath "$INSTDIR\imageformats"  
  File files\imageformats\qicns.dll
  File files\imageformats\qico.dll
  File files\imageformats\qjpeg.dll

  SetOutPath "$INSTDIR\platforms"  
  File files\platforms\qwindows.dll

  SetOutPath "$INSTDIR\tools"  
  File files\tools\addr2line.exe
  File files\tools\cygiconv-2.dll
  File files\tools\cygintl-8.dll
  File files\tools\cygwin1.dll
  File files\tools\cygz.dll
  
  ;Store installation folder
  WriteRegStr HKCU "Software\Modern UI Test" "" $INSTDIR
  
  ;Create uninstaller
  WriteUninstaller "$INSTDIR\Uninstall.exe"

  !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
    
    ;Create shortcuts
    CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\LotusTracker.lnk" "$INSTDIR\LotusTracker.exe"
    CreateShortcut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
  
  !insertmacro MUI_STARTMENU_WRITE_END

SectionEnd

;--------------------------------
;Descriptions

  ;Language strings
  LangString DESC_SecLotusTracker ${LANG_ENGLISH} "A test section."

  ;Assign language strings to sections
  !insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
    !insertmacro MUI_DESCRIPTION_TEXT ${SecLotusTracker} $(DESC_SecLotusTracker)
  !insertmacro MUI_FUNCTION_DESCRIPTION_END
 
;--------------------------------
;Uninstaller Section

Section "Uninstall"

  ;ADD YOUR OWN FILES HERE...

  Delete "$INSTDIR\Uninstall.exe"

  Delete $INSTDIR\LotusTracker.exe
  Delete $INSTDIR\libcrypto.dll
  Delete $INSTDIR\libcrypto-1_1.dll
  Delete $INSTDIR\libeay32.dll
  Delete $INSTDIR\libssl.dll
  Delete $INSTDIR\libssl-1_1.dll
  Delete $INSTDIR\libgcc_s_dw2-1.dll
  Delete $INSTDIR\libqt5keychain.dll
  Delete $INSTDIR\libstdc++-6.dll
  Delete $INSTDIR\libwinpthread-1.dll
  Delete $INSTDIR\ssleay32.dll
  Delete $INSTDIR\Qt5Core.dll
  Delete $INSTDIR\Qt5Gui.dll
  Delete $INSTDIR\Qt5Network.dll
  Delete $INSTDIR\Qt5Widgets.dll
  Delete $INSTDIR\WinSparkle.dll
  Delete $INSTDIR\bin\tar.exe
  Delete $INSTDIR\bin\xz.exe
  Delete $INSTDIR\imageformats\qicns.dll
  Delete $INSTDIR\imageformats\qico.dll
  Delete $INSTDIR\imageformats\qjpeg.dll
  Delete $INSTDIR\platforms\qwindows.dll
  Delete $INSTDIR\tools\addr2line.exe
  Delete $INSTDIR\tools\cygiconv-2.dll
  Delete $INSTDIR\tools\cygintl-8.dll
  Delete $INSTDIR\tools\cygwin1.dll
  Delete $INSTDIR\tools\cygz.dll

  RMDir "$INSTDIR\bin"
  RMDir "$INSTDIR\imageformats"
  RMDir "$INSTDIR\platforms"
  RMDir "$INSTDIR\tools"
  RMDir "$INSTDIR"

  !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    
  Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
  RMDir "$SMPROGRAMS\$StartMenuFolder"
  
  DeleteRegKey /ifempty HKCU "Software\Modern UI Test"

SectionEnd