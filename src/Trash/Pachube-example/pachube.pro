BASEDIR = /home/toolchains/out/tinyputer/telemetrics/ 
TEMPLATE = app
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
QT += xml
QT += network

# Input
SOURCES += Trash/Pachube-example/main.cpp 
INCLUDEPATH += 
SOURCES += Sensors/devproxy.cpp \
    Cloud/cloproxy.cpp \
    Configurator/configurator.cpp \
    Message/message.cpp \
    Cloud/cloconnection.cpp \
    Cloud/Mock/mockcloud.cpp \
    Cloud/Cosm/cosmxml.cpp \
    Cloud/Cosm/cosmcloud.cpp \
    Cloud/Cosm/cosmtime.cpp \
    Cloud/TopologyCloud/topologycloud.cpp \
    Sensors/devconnection.cpp \
    Sensors/Mock/mockdev.cpp \
    Sensors/SNG/sngconnection.cpp \
    Sensors/SNG/sngphysicaladdress.cpp \
    Sensors/Topology/topologysensor.cpp \
    Sensors/Modbus/modbus.cpp \
    Sensors/Modbus/modbusrtuframe.cpp \
    Logger/loggingHandler.cpp \
    Logger/logger.cpp \
    LogSender/HTTPLogSender/httplogsender.cpp \
    Sensors/SNG/sngframe.cpp \
    Sensors/SNG/sngmsgcreator.cpp \
    Sensors/SNG/sngmsgparser.cpp \
    Sensors/Modbus/crc-count.cpp
HEADERS += Sensors/devproxy.h \
    Cloud/cloproxy.h \
    Configurator/configurator.h \
    Message/message.h \
    Cloud/cloconnection.h \
    Cloud/Mock/mockcloud.h \
    Cloud/Cosm/cosmxml.h \
    Cloud/Cosm/cosmcloud.h \
    Cloud/Cosm/cosmtime.h \
    Cloud/TopologyCloud/topologycloud.h \
    Sensors/devconnection.h \
    Sensors/Mock/mockdev.h \
    Sensors/SNG/sngphysicaladdress.h \
    Sensors/SNG/sngconnection.h \
    Sensors/Topology/topologysensor.h \
    Sensors/Modbus/modbus.h \
    Sensors/Modbus/modbusrtuframe.h \
    Logger/loggingHandler.h \
    Logger/logger.h \
    LogSender/HTTPLogSender/httplogsender.h \
    LogSender/logsender.h \
    Sensors/SNG/sngframe.h \
    Sensors/SNG/sngmsgcreator.h \
    Sensors/SNG/sngmsgparser.h \
    Sensors/Modbus/crc-count.h
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
INSTALLS += target sources

