@echo off

premake5 vs2019
IF %ERRORLEVEL% NEQ 0 (
  PAUSE
) ELSE (
  echo Successfully generated project files.
  timeout 5
)
