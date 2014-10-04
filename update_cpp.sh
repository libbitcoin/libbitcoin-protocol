#!/bin/bash
set -e

echo "updating c++ protocol buffer files"
pushd proto

# generate c++ source/header
echo "generating c++ sources"
protoc --cpp_out=dllexport_decl=BCP_API:../include bitcoin/protocol/interface.proto

popd

# modify generated header to introduce define.hpp inclusion
echo "modifying generated header"
sed -i '/protoc_insertion_point(includes)/ i\#include <bitcoin\/protocol\/define.hpp>' ./include/bitcoin/protocol/interface.pb.h

# relocate generated/modified files into c++ project
echo "relocating files to 'cpp' directory"
mv ./include/bitcoin/protocol/interface.pb.cc ./src/

echo "update complete"

