# -------------------------------------------------
# Project created by QtCreator 2010-04-28T20:25:30
# -------------------------------------------------
TEMPLATE = app

QT += testlib \
    network \
    xml
QT -= gui

TARGET = telemetron-test
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../src/ \
               ../

#OBJECTS_DIR = ../build/.obj-test
#MOC_DIR = ../build/.moc-test
#RCC_DIR = ../build/.rcc-test
LIBS += ../build-telemetron/cloconnection.o\
        ../build-telemetron/httplogsender.o\
        ../build-telemetron/moc_devproxy.o       \
        ../build-telemetron/moc_sngconnection.o   \
        ../build-telemetron/sngmsgcreator.o\
        ../build-telemetron/cloproxy.o       \
        ../build-telemetron/logger.o             \
        ../build-telemetron/moc_httplogsender.o  \
        ../build-telemetron/moc_sngconnect.o      \
        ../build-telemetron/sngmsgparser.o\
        ../build-telemetron/configurator.o   \
        ../build-telemetron/mockcloud.o          \
        ../build-telemetron/moc_topologycloud.o   \
        ../build-telemetron/sngphysicaladdress.o\
        ../build-telemetron/cosmcloud.o      \
        ../build-telemetron/mockdev.o            \
        ../build-telemetron/moc_topologysensor.o  \
        ../build-telemetron/topologycloud.o\
        ../build-telemetron/cosmtime.o       \
        ../build-telemetron/message.o            \
        ../build-telemetron/moc_logger.o         \
        ../build-telemetron/modbus.o              \
        ../build-telemetron/topologysensor.o\
        ../build-telemetron/cosmxml.o        \
        ../build-telemetron/moc_cloconnection.o  \
        ../build-telemetron/moc_logsender.o      \
        ../build-telemetron/modbusrtuframe.o\
        ../build-telemetron/crc-count.o      \
        ../build-telemetron/moc_cloproxy.o       \
        ../build-telemetron/moc_mockcloud.o      \
        ../build-telemetron/sngconnection.o\
        ../build-telemetron/devconnection.o  \
        ../build-telemetron/moc_cosmcloud.o      \
        ../build-telemetron/moc_mockdev.o        \
        ../build-telemetron/sngconnect.o\
        ../build-telemetron/devproxy.o       \
        ../build-telemetron/moc_devconnection.o  \
        ../build-telemetron/moc_modbus.o         \
        ../build-telemetron/sngframe.o

SOURCES += main.cpp \
    sampletest.cpp \
    Cloud/Cosm/cosmxmltest.cpp \
    Cloud/Cosm/cosmtimetest.cpp \
    Sensors/Modbus/modbusframetest.cpp \
    Sensors/Modbus/modbustest.cpp \
    Sensors/Sng/sngtest.cpp \
    Sensors/Sng/sngmsgcreatortest.cpp \
    Sensors/Sng/sngphysicaladdresstest.cpp \
    Sensors/Sng/sngparsertest.cpp

HEADERS += \
    sampletest.h \
    Cloud/Cosm/cosmtimetest.h \
    Cloud/Cosm/cosmxmltest.h \
    Sensors/Modbus/modbusframetest.h \
    Sensors/Modbus/modbustest.h \
    Sensors/Sng/sngtest.h \
    Sensors/Sng/sngmsgcreatortest.h \
    Sensors/Sng/sngphysicaladdresstest.h \ 
    Sensors/Sng/sngparsertest.h \
    TestsManager.h
