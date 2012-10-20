

1) prepare build environment (building in-place - sorry, a bit messy, but simplifies building http server)

export QMAKE_TP=/home/toolchains/out/tinyputer/host/usr/bin/qmake

cd PROJECT_ROOT
git clone https://github.com/fikander/qhttpserver
pushd qhttpserver; $QMAKE_TP; make; popd

git clone git@repos.synergiatech.pl:telemetrics.github
pushd telemetrics; $QMAKE_TP; make; popd

cp qhttpserver/lib/* telemetrics/httpconfigurator
pushd telemetrics/httpconfigurator; ln -s libqhttpconfigurator.so.0.1.0 libqhttpconfigurator.so.0; popd

# copy telemetrics/httpconfigurator/* to TP

#
# on TP
#
# run in the background
LD_LIBRARY_PATH=. httpconfigurator &


2) resulting project directory

project
 - qhttpserver
   - src
   - lib
 - telemetrics
   - httpconfigurator

3) run http server
