#/bin/bash

TP_QMAKE=/home/toolchains/out/tinyputer/host/usr/bin/qmake

mkdir tp-build 2>0

pushd tp-build

$TP_QMAKE -recursive ../telemetron.pro

make

popd
