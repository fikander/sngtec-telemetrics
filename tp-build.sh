#/bin/bash

#
# 1. create build directory, cd to it
# 2. copy this script to build directory
# 3. build qhttpserver
# 4. run it with telemetron.pro directory as parameter,e. g.
#
# ./tp-build.sh  ../telemetrics/telemetron.pro all
#

if [ $# -lt 1 ]; then
    echo "Usage: $0 TELEMETRON_PRO_PATH [MAKE_PARAMETERS]"
    exit 1
fi

TP_QMAKE=/home/toolchains/out/tinyputer/host/usr/bin/qmake
TELEMETRON_PRO=$1
shift

$TP_QMAKE -recursive $TELEMETRON_PRO

make $*

