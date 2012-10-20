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
LIBS += ../build/cloconnection.o\
        ../build/httplogsender.o\
        ../build/moc_devproxy.o       \
        ../build/moc_sngconnection.o   \
        ../build/sngmsgcreator.o\
        ../build/cloproxy.o       \
        ../build/logger.o             \
        ../build/moc_httplogsender.o  \
        ../build/moc_sngconnect.o      \
        ../build/sngmsgparser.o\
        ../build/configurator.o   \
        ../build/loggingHandler.o     \
        ../build/mockcloud.o          \
        ../build/moc_topologycloud.o   \
        ../build/sngphysicaladdress.o\
        ../build/cosmcloud.o      \
        ../build/mockdev.o            \
        ../build/moc_topologysensor.o  \
        ../build/topologycloud.o\
        ../build/cosmtime.o       \
        ../build/message.o            \
        ../build/moc_logger.o         \
        ../build/modbus.o              \
        ../build/topologysensor.o\
        ../build/cosmxml.o        \
        ../build/moc_cloconnection.o  \
        ../build/moc_logsender.o      \
        ../build/modbusrtuframe.o\
        ../build/crc-count.o      \
        ../build/moc_cloproxy.o       \
        ../build/moc_mockcloud.o      \
        ../build/sngconnection.o\
        ../build/devconnection.o  \
        ../build/moc_cosmcloud.o      \
        ../build/moc_mockdev.o        \
        ../build/sngconnect.o\
        ../build/devproxy.o       \
        ../build/moc_devconnection.o  \
        ../build/moc_modbus.o         \
        ../build/sngframe.o

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
