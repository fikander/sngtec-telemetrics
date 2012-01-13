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

OBJECTS_DIR = ../build/.obj-test
MOC_DIR = ../build/.moc-test
RCC_DIR = ../build/.rcc-test
LIBS += /home/toolchains/telemetron-build-desktop/build/.obj/modbusrtuframe.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/modbus.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/sngconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/sngphysicaladdress.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/devconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/cloconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/cloproxy.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/devproxy.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/pachubexml.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/pachubecloud.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/message.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/configurator.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/loggingHandler.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_cloconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_devproxy.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_mockdev.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_devconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/mockdev.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_pachubecloud.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_sngconnection.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_cloproxy.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/mockcloud.o \
        /home/toolchains/telemetron-build-desktop/build/.obj/moc_modbus.o


INCLUDEPATH += ../

TEMPLATE = app
SOURCES += main.cpp \
    sampletest.cpp \
    Sensors/Modbus/modbusframetest.cpp \
    Sensors/Modbus/modbustest.cpp

HEADERS += AutoTest.h \
    sampletest.h \
    Sensors/Modbus/modbusframetest.h \
    Sensors/Modbus/modbustest.h


