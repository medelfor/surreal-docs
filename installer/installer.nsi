#
#  Copyright © 2023, Medelfor, Limited. All rights reserved.
#  For any additional information refer to https://medelfor.com
#

Unicode True

!include "MUI2.nsh"
!include "WordFunc.nsh"
!include "Junction.nsh"
Name "Surreal Docs v${DIST_VERSION}"
OutFile "installer/SurrealDocs-v${DIST_VERSION}-x64.exe"
RequestExecutionLevel admin

!define CLI_EXE "surdocs.exe"
!define CLI_DLL "M-UDocs-CLI.dll"

Function CreateGUID
  System::Call 'ole32::CoCreateGuid(g .s)'
FunctionEnd

Function .onInit
  SetShellVarContext all

  Var /GLOBAL CACHE_DIR
  StrCpy $CACHE_DIR "$LocalAppData\Medelfor\SurrealDocs"

  Var /GLOBAL BIN_DIR
  StrCpy $BIN_DIR "$INSTDIR\bin"

  SetOutPath $TEMP
  File /oname=spltmp.bmp "Splash.bmp"

  splash::show 2500 $TEMP\spltmp

  Pop $0

  Delete $TEMP\spltmp.bmp
FunctionEnd

InstallDir "$PROGRAMFILES64\Medelfor\SurrealDocs\"
BrandingText "Copyright (c) 2023, Medelfor, Limited. All rights reserved"
InstallDirRegKey HKLM "Software\Medelfor\SurrealDocs" ""

RequestExecutionLevel admin

!define MUI_ABORTWARNING
!define MUI_LICENSEPAGE_CHECKBOX
!define MUI_ICON "Icon.ico"
!define MUI_UNICON "Icon.ico"
!define MUI_WELCOMEFINISHPAGE_BITMAP "InstallerSurrealDocsWelcomeFinish.bmp"

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_LICENSE "EULA.rtf"
!insertmacro MUI_PAGE_COMPONENTS
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH

!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH

!insertmacro MUI_LANGUAGE "English"

Section "!Surreal Docs Core" SecCore
  SetRegView 64
  SectionInstType RO

  SetOutPath "$CACHE_DIR"

  File ${DIST_PATH}\res\misc\other\udocs_processor\settings.json

  SetOutPath "$INSTDIR"

  File /r /x "${CLI_DLL}" /x "${CLI_EXE}" "${DIST_PATH}\"

  EnVar::SetHKLM
  EnVar::Check "Path" "$INSTDIR"
  Pop $0
  ${If} $0 != 0
    EnVar::AddValue "Path" "$INSTDIR"
    Pop $0
    ${If} $0 != 0
      MessageBox MB_OK|MB_ICONEXCLAMATION \
         "Unable to add $INSTDIR to Path. Please add it manually" /SD IDOK
    ${EndIf}
  ${EndIf}

  WriteRegStr HKLM "Software\Medelfor" "" ""
  WriteRegStr HKLM "Software\Medelfor\SurrealDocs" "" $INSTDIR
  WriteRegStr HKLM "Software\Medelfor\SurrealDocs" "WasInstalled" "1"
  WriteRegStr HKLM "Software\Medelfor\SurrealDocs" "Version" "${DIST_VERSION}"
  WriteRegStr HKLM "Software\Medelfor\SurrealDocs" "Cache" "$CACHE_DIR"
  Call CreateGUID
  Pop $0
  WriteRegStr HKLM "Software\Medelfor\SurrealDocs" "ClientId" "$0"

  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "" ""
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayName" "Surreal Docs v${DIST_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayVersion" "${DIST_VERSION}"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayIcon" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "UninstallPath" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "UninstallString" "$INSTDIR\uninst.exe"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "InstallLocation" "$INSTDIR"
  WriteRegStr HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "Publisher" "Medelfor, Limited"

  inetc::get "https://storage.googleapis.com/deployed.medelfor.com/prod/html2png/html2png/win64/html2png.zip" "$INSTDIR\html2png.zip"
  Pop $0
    StrCmp $0 "OK" dlok
    MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to download Html2png. Click OK to exit the installer" /SD IDOK
    Quit
  dlok:

  nsisunz::UnzipToLog "$INSTDIR\html2png.zip" "$INSTDIR\bin"
  Pop $0
  StrCmp $0 "success" ok
    MessageBox MB_OK|MB_ICONEXCLAMATION "Unable to extract Html2png. Click OK to exit the installer" /SD IDOK
    Quit
  ok:

  WriteUninstaller "$INSTDIR\uninst.exe"
SectionEnd

Section "Surreal Docs CLI" SecCLI
  SetOutPath "$INSTDIR"

  File ${DIST_PATH}\${CLI_EXE}
  File ${DIST_PATH}\${CLI_DLL}
SectionEnd

LangString DESC_SecCore ${LANG_ENGLISH} "Core of Surreal Docs"
LangString DESC_SecCLI ${LANG_ENGLISH} "CLI to access Surreal Docs from commandline"

!insertmacro MUI_FUNCTION_DESCRIPTION_BEGIN
!insertmacro MUI_DESCRIPTION_TEXT ${SecCore} $(DESC_SecCore)
!insertmacro MUI_DESCRIPTION_TEXT ${SecCLI} $(DESC_SecCLI)
!insertmacro MUI_FUNCTION_DESCRIPTION_END

Section "Uninstall"
  SetRegView 64
  Delete "$INSTDIR\uninst.exe"

  RMDir /R "$INSTDIR"

  Delete "$INSTDIR\*.*"

  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayName"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayVersion"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "DisplayIcon"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "UninstallPath"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "UninstallString"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "InstallLocation"
  DeleteRegValue HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs" "Publisher"
  DeleteRegKey HKLM "Software\Microsoft\Windows\CurrentVersion\Uninstall\SurrealDocs"
SectionEnd
