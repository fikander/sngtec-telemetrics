TELEMETRON_BASE = ..
TEMPLATE = app

TARGET = telemetron
VERSION = 0.1.0

QT += network script xml
QT -= gui

CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += .
INCLUDEPATH += ../libmodbus

LIBS += -L../lib -llibmodbus


HEADERS = Message/message.h \
    Logger/logger.h \
    LogSender/HTTPLogSender/httplogsender.h \
    LogSender/logsender.h \
    CloudFactory.h \
    SensorFactory.h \
    Configurator2.h \
    Cloud/Cloud.h \
    Sensors/Sensor.h \
    KeyValueMap.h \
    Cloud/DummyCloud.h \
    debug.h \
    Sensors/DummySensor.h \
    Cloud/sngconnect/SNGConnectCloud.h \
    Cloud/sngconnect/SNGConnectAPI.h \
    Sensors/CommandProcessor.h \
    Sensors/Modbus/ModbusSensor.h \
    sha2/sha2.h

SOURCES =  main.cpp \
    Message/message.cpp \
    Logger/logger.cpp \
    LogSender/HTTPLogSender/httplogsender.cpp \
    CloudFactory.cpp \
    SensorFactory.cpp \
    Configurator2.cpp \
    Cloud/Cloud.cpp \
    Cloud/DummyCloud.cpp \
    Sensors/Sensor.cpp \
    Sensors/DummySensor.cpp \
    Cloud/sngconnect/SNGConnectCloud.cpp \
    Cloud/sngconnect/SNGConnectAPI.cpp \
    Sensors/CommandProcessor.cpp \
    Sensors/Modbus/ModbusSensor.cpp \
    sha2/sha2.cpp

    

OBJECTS_DIR = $$TELEMETRON_BASE/build-telemetron
MOC_DIR = $$TELEMETRON_BASE/build-telemetron
DESTDIR = $$TELEMETRON_BASE/bin
