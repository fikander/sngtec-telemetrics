TELEMETRON_BASE = ..
TEMPLATE = app

TARGET = telemetron
VERSION = 0.1.0

QT += network \
    xml
QT -= gui

CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += .

HEADERS = Sensors/devproxy.h \
    Cloud/cloproxy.h \
    Configurator/configurator.h \
    Message/message.h \
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
    Sensors/Modbus/crc-count.h \
    Sensors/Modbus/modbus.h \
    Sensors/Modbus/modbusrtuframe.h \
    Logger/logger.h \
    LogSender/HTTPLogSender/httplogsender.h \
    LogSender/logsender.h \
    Sensors/SNG/sngframe.h \
    Sensors/SNG/sngmsgcreator.h \
    Sensors/SNG/sngmsgparser.h \
    CloudFactory.h \
    SensorFactory.h \
    Configurator2.h \
    Cloud/Cloud.h \
    Sensors/Sensor.h \
    KeyValueMap.h \
    Cloud/DummyCloud.h \
    debug.h \
    Sensors/DummySensor.h \
    Cloud/cloconnection.h \
    Cloud/sngconnect/SNGConnectCloud.h \
    Cloud/sngconnect/SNGConnectAPI.h

SOURCES =  main.cpp \
    Sensors/devproxy.cpp \
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
    Sensors/Modbus/crc-count.cpp \
    Sensors/Modbus/modbus.cpp \
    Sensors/Modbus/modbusrtuframe.cpp \
    Logger/logger.cpp \
    LogSender/HTTPLogSender/httplogsender.cpp \
    Sensors/SNG/sngframe.cpp \
    Sensors/SNG/sngmsgcreator.cpp \
    Sensors/SNG/sngmsgparser.cpp \
    CloudFactory.cpp \
    SensorFactory.cpp \
    Configurator2.cpp \
    Cloud/Cloud.cpp \
    Cloud/DummyCloud.cpp \
    Sensors/Sensor.cpp \
    Sensors/DummySensor.cpp \
    Cloud/sngconnect/SNGConnectCloud.cpp \
    Cloud/sngconnect/SNGConnectAPI.cpp

    

OBJECTS_DIR = $$TELEMETRON_BASE/build-telemetron
MOC_DIR = $$TELEMETRON_BASE/build-telemetron
DESTDIR = $$TELEMETRON_BASE/bin
