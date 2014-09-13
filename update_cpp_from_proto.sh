#!/bin/bash
set -e

echo "updating c++ protocol buffer files"
pushd proto

# generate c++ source/header
echo "generating c++ sources"
protoc --cpp_out=dllexport_decl=BCP_API:. bitcoin/protocol/interface.proto

# modify generated header to introduce define.hpp inclusion
echo "modifying generated header"
sed -i '/protoc_insertion_point(includes)/ i\#include <bitcoin\/protocol\/define.hpp>' bitcoin/protocol/interface.pb.h

# relocate generated/modified files into c++ project
echo "relocating files to 'cpp' directory"
mv bitcoin/protocol/interface.pb.h ../cpp/include/bitcoin/protocol/
mv bitcoin/protocol/interface.pb.cc ../cpp/src/

popd

echo "update complete"

