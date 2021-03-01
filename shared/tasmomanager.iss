#define Output "."

[Setup]
AppId={{ecb0429a-67bc-47fe-9f47-aaec1f405054}
AppName=TasmoManager
AppVersion={#AppVersion}
AppVerName=TasmoManager {#AppVersion}
AppPublisher=Tom Butcher
AppPublisherURL=https://github.com/tom-23/TasmoManager
AppSupportURL=https://github.com/tom-23/TasmoManager
AppUpdatesURL=https://github.com/tom-23/TasmoManager
DefaultDirName={pf}\TasmoManager
DefaultGroupName=TasmoManager
AllowNoIcons=yes
LicenseFile={#Source}\LICENSE
OutputDir={#Output}
OutputBaseFilename=tasmomanager-{#AppVersion}-setup
Compression=lzma
SolidCompression=yes
SetupIconFile=installicon.ico
WizardImageFile=logo.bmp
WizardSmallImageFile=logo-small.bmp

[Languages]
Name: en; MessagesFile: "compiler:Default.isl"

[CustomMessages]
en.ProgramFiles=Program Files

[Types]
Name: "full"; Description: "{code:GetFullInstallation}"

[Components]
Name: "program"; Description: "{cm:ProgramFiles}"; Types: full ; Flags: fixed

[Tasks]
Name: "desktopicon"; Description: "{cm:CreateDesktopIcon}"; Flags: unchecked

[Files]
Source: "{#Root}\tasmomanager.exe"; DestDir: "{app}"; Components: program; Flags: ignoreversion
Source: "{#Root}\LICENSE"; DestDir: "{app}"; Components: program; Flags: ignoreversion
Source: "{#Root}\README.md"; DestDir: "{app}"; Components: program; Flags: ignoreversion

; Qt and toolchain
Source: "{#Root}\bearer\*.dll"; DestDir: "{app}\bearer"; Components: program; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#Root}\imageformats\*.dll"; DestDir: "{app}\imageformats"; Components: program; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#Root}\platforms\*.dll"; DestDir: "{app}\platforms"; Components: program; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#Root}\styles\*.dll"; DestDir: "{app}\styles"; Components: program; Flags: ignoreversion recursesubdirs createallsubdirs
Source: "{#Root}\*.dll"; DestDir: "{app}"; Components: program; Flags: ignoreversion

[Icons]
Name: "{group}\TasmoManager"; Filename: "{app}\tasmomanager.exe"
Name: "{commondesktop}\TasmoManager"; Filename: "{app}\tasmomanager.exe"; Tasks: desktopicon

[Run]
Filename: "{app}\tasmomanager.exe"; Description: "{cm:LaunchProgram,TasmoManager}"; Flags: nowait postinstall skipifsilent

[Code]
function GetFullInstallation(Param: string): string;
begin
	Result := SetupMessage(msgFullInstallation);
end;