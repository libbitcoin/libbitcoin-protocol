@ECHO OFF
ECHO Downloading libbitcoin vs2022 dependencies from NuGet
CALL nuget.exe install ..\vs2022\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2022\libbitcoin-protocol-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2019 dependencies from NuGet
CALL nuget.exe install ..\vs2019\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2019\libbitcoin-protocol-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2017 dependencies from NuGet
CALL nuget.exe install ..\vs2017\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-protocol-test\packages.config
