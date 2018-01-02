@ECHO OFF
ECHO Downloading libbitcoin vs2017 dependencies from NuGet
CALL nuget.exe install ..\vs2017\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2017\libbitcoin-protocol-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2015 dependencies from NuGet
CALL nuget.exe install ..\vs2015\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2015\libbitcoin-protocol-test\packages.config
ECHO.
ECHO Downloading libbitcoin vs2013 dependencies from NuGet
CALL nuget.exe install ..\vs2013\libbitcoin-protocol\packages.config
CALL nuget.exe install ..\vs2013\libbitcoin-protocol-test\packages.config
