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

INCLUDEPATH += ../

OBJECTS_DIR = ../build/.obj-test
MOC_DIR = ../build/.moc-test
RCC_DIR = ../build/.rcc-test
LIBS += ../build/.obj/modbusrtuframe.o \
        ../build/.obj/modbus.o \
        ../build/.obj/sngconnection.o \
        ../build/.obj/sngphysicaladdress.o \
        ../build/.obj/sngframe.o\
        ../build/.obj/sngmsgcreator.o\
        ../build/.obj/sngmsgparser.o \
        ../build/.obj/devconnection.o \
        ../build/.obj/cloconnection.o \
        ../build/.obj/cloproxy.o \
        ../build/.obj/devproxy.o \
        ../build/.obj/pachubexml.o \
        ../build/.obj/pachubecloud.o \
        ../build/.obj/pachubetime.o \
        ../build/.obj/message.o \
        ../build/.obj/configurator.o \
        ../build/.obj/loggingHandler.o \
        ../build/.obj/moc_cloconnection.o \
        ../build/.obj/moc_devproxy.o \
        ../build/.obj/moc_mockdev.o \
        ../build/.obj/moc_devconnection.o \
        ../build/.obj/mockdev.o \
        ../build/.obj/moc_pachubecloud.o \
        ../build/.obj/moc_sngconnection.o \
        ../build/.obj/moc_cloproxy.o \
        ../build/.obj/mockcloud.o \
        ../build/.obj/moc_modbus.o


TEMPLATE = app
SOURCES += main.cpp \
    sampletest.cpp \
    Cloud/Pachube/pachubexmltest.cpp \
    Cloud/Pachube/pachubetimetest.cpp \
    Sensors/Modbus/modbusframetest.cpp \
    Sensors/Modbus/modbustest.cpp \
    Sensors/Sng/sngtest.cpp \
    Sensors/Sng/sngmsgcreatortest.cpp \
    Sensors/Sng/sngphysicaladdresstest.cpp \
    Sensors/Sng/sngparsertest.cpp

HEADERS += AutoTest.h \
    sampletest.h \
    Cloud/Pachube/pachubetimetest.h \
    Cloud/Pachube/pachubexmltest.h \
    Sensors/Modbus/modbusframetest.h \
    Sensors/Modbus/modbustest.h \
    Sensors/Sng/sngtest.h \
    Sensors/Sng/sngmsgcreatortest.h \
    Sensors/Sng/sngphysicaladdresstest.h \ 
    Sensors/Sng/sngparsertest.h
