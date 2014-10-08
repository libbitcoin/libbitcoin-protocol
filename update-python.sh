#!/bin/bash
set -e

echo "updating python protocol buffer files"
pushd proto

# generate c++ source/header
echo "generating python sources"
protoc --python_out=../python bitcoin/protocol/interface.proto

popd

echo "update complete"

