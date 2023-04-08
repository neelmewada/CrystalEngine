
@echo off

echo "Setting up environment..."

if not defined DevEnvDir (
    call "C:\Program Files\Microsoft Visual Studio\2022\Community\VC\Auxiliary\Build\vcvars64.bat"
)

if not defined DevEnvDir (
    call "C:\Program Files\Microsoft Visual Studio\2019\Community\VC\Auxiliary\Build\vcvars64.bat"
)

%comspec% /K sh BuildThirdParty.sh %1 %2
Rem start "" "%PROGRAMFILES%\Git\bin\git-bash.exe" BuildThirdParty.sh --login -i %1 %2

echo "Batch call finished"
