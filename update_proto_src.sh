#!/bin/bash
set -e

pushd proto
protoc --cpp_out=dllexport_decl=BCP_API:. bitcoin/protocol/interface.proto
#protoc --cpp_out=. bitcoin/protocol/interface.proto
mv bitcoin/protocol/interface.pb.cc ../src/
mv bitcoin/protocol/interface.pb.h ../include/bitcoin/protocol/
popd

