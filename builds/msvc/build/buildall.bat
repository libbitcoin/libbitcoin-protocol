@ECHO OFF
ECHO.
ECHO Downloading Libbitcoin Explorer dependencies from NuGet
CALL nuget.exe install ..\vs2013\libbitcoin-protocol\packages.config
ECHO.
CALL buildbase.bat ..\vs2013\libbitcoin-protocol.sln 12
ECHO.
PAUSE