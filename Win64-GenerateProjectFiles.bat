:: Copyright to Avram Traian. 2022 - 2022.
:: File created on August 21 2022.

@ECHO OFF
PUSHD "%~dp0"

CALL "Leaf\ThirdParty\premake\Bin\Win64\premake5.exe" --file="Leaf/Build/Dev-Workspace.lua" vs2022

POPD
PAUSE