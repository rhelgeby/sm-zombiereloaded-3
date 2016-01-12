#!/bin/sh

BUILD_DIR=build
SRC=src/testsuite/zr
SM_INCLUDE=env/include
ZR_INCLUDE=src/include
SPCOMP=env/linux/bin/spcomp-1.3.4

mkdir -p $BUILD_DIR

$SPCOMP -i$SM_INCLUDE -i$ZR_INCLUDE -o$BUILD_DIR/respawnapitest.smx $SRC/respawnapitest.sp
$SPCOMP -i$SM_INCLUDE -i$ZR_INCLUDE -o$BUILD_DIR/infectapitest.smx $SRC/infectapitest.sp
