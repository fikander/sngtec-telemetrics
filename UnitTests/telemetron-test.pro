# -------------------------------------------------
# Project created by QtCreator 2010-04-28T20:25:30
# -------------------------------------------------
QT += testlib \
    network \
    xml
QT -= gui

TARGET = telemetron-test
CONFIG += console
CONFIG -= app_bundle

INCLUDEPATH += ../src/ \
               ../

OBJECTS_DIR = ../build/.obj-test
MOC_DIR = ../build/.moc-test
RCC_DIR = ../build/.rcc-test
LIBS += ../build/.obj/cloconnection.o\
        ../build/.obj/httplogsender.o\
        ../build/.obj/moc_devproxy.o       \
        ../build/.obj/moc_sngconnection.o   \
        ../build/.obj/sngmsgcreator.o\
        ../build/.obj/cloproxy.o       \
        ../build/.obj/logger.o             \
        ../build/.obj/moc_httplogsender.o  \
        ../build/.obj/moc_sngconnect.o      \
        ../build/.obj/sngmsgparser.o\
        ../build/.obj/configurator.o   \
        ../build/.obj/loggingHandler.o     \
        ../build/.obj/mockcloud.o          \
        ../build/.obj/moc_topologycloud.o   \
        ../build/.obj/sngphysicaladdress.o\
        ../build/.obj/cosmcloud.o      \
        ../build/.obj/mockdev.o            \
        ../build/.obj/moc_topologysensor.o  \
        ../build/.obj/topologycloud.o\
        ../build/.obj/cosmtime.o       \
        ../build/.obj/message.o            \
        ../build/.obj/moc_logger.o         \
        ../build/.obj/modbus.o              \
        ../build/.obj/topologysensor.o\
        ../build/.obj/cosmxml.o        \
        ../build/.obj/moc_cloconnection.o  \
        ../build/.obj/moc_logsender.o      \
        ../build/.obj/modbusrtuframe.o\
        ../build/.obj/crc-count.o      \
        ../build/.obj/moc_cloproxy.o       \
        ../build/.obj/moc_mockcloud.o      \
        ../build/.obj/sngconnection.o\
        ../build/.obj/devconnection.o  \
        ../build/.obj/moc_cosmcloud.o      \
        ../build/.obj/moc_mockdev.o        \
        ../build/.obj/sngconnect.o\
        ../build/.obj/devproxy.o       \
        ../build/.obj/moc_devconnection.o  \
        ../build/.obj/moc_modbus.o         \
        ../build/.obj/sngframe.o

TEMPLATE = app
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
