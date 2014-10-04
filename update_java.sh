#!/bin/bash
set -e

echo "updating java protocol buffer files"

# generate c++ source/header
echo "generating java sources"
protoc --java_out=./java ./proto/bitcoin/protocol/interface.proto

echo "update complete"

