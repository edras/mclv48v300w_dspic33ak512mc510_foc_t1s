@echo off
setlocal

:: ============================================================
:: get_pyx2cscope.bat
:: Downloads the latest pyX2Cscope Windows release from GitHub,
:: extracts it into a pyX2Cscope subfolder, then installs scipy.
:: ============================================================

set "REPO=X2Cscope/pyx2cscope"
set "API_URL=https://api.github.com/repos/%REPO%/releases/latest"
set "INSTALL_DIR=%~dp0pyX2Cscope"
set "TEMP_ZIP=%TEMP%\pyX2Cscope_latest.zip"
set "TEMP_EXTRACT=%TEMP%\pyX2Cscope_extracted"

echo [1/4] Fetching latest pyx2cscope release info from GitHub...
for /f "usebackq tokens=*" %%A in (
    `powershell -NoProfile -Command "(Invoke-RestMethod -Uri '%API_URL%').assets | Where-Object { $_.name -like '*win_amd64*' } | Select-Object -ExpandProperty browser_download_url"`
) do set "DOWNLOAD_URL=%%A"

if "%DOWNLOAD_URL%"=="" (
    echo ERROR: Could not retrieve download URL. Check your internet connection.
    exit /b 1
)

echo     URL: %DOWNLOAD_URL%

echo [2/4] Downloading release...
powershell -NoProfile -Command "Invoke-WebRequest -Uri '%DOWNLOAD_URL%' -OutFile '%TEMP_ZIP%' -UseBasicParsing"
if errorlevel 1 (
    echo ERROR: Download failed.
    exit /b 1
)

echo [3/4] Extracting to %INSTALL_DIR%...
if not exist "%INSTALL_DIR%" mkdir "%INSTALL_DIR%"
if exist "%TEMP_EXTRACT%" rd /s /q "%TEMP_EXTRACT%"
powershell -NoProfile -Command "Expand-Archive -Path '%TEMP_ZIP%' -DestinationPath '%TEMP_EXTRACT%' -Force"
if errorlevel 1 (
    echo ERROR: Extraction failed.
    exit /b 1
)

:: The zip contains a single pyX2Cscope subfolder — copy its contents into INSTALL_DIR
powershell -NoProfile -Command "Copy-Item -Path '%TEMP_EXTRACT%\pyX2Cscope\*' -Destination '%INSTALL_DIR%' -Recurse -Force"
if errorlevel 1 (
    echo ERROR: Copy failed.
    exit /b 1
)

:: Clean up temp files
del /q "%TEMP_ZIP%"
rd /s /q "%TEMP_EXTRACT%"

echo [4/4] Installing scipy...
"%INSTALL_DIR%\pyX2Cscope.exe" --install scipy
if errorlevel 1 (
    echo ERROR: scipy installation failed.
    exit /b 1
)

echo.
echo Done. pyX2Cscope updated and scipy installed successfully.
endlocal
