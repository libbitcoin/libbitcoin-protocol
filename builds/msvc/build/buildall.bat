@ECHO OFF
ECHO.
ECHO Downloading Libbitcoin Explorer dependencies from NuGet
CALL nuget.exe install ..\vs2013\libbitcoin_protocol\packages.config
CALL nuget.exe install ..\vs2013\libbitcoin_protocol_test\packages.config
ECHO.
CALL buildbase.bat ..\vs2013\libbitcoin_protocol.sln 12
ECHO.
PAUSE