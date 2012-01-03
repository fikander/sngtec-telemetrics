# -------------------------------------------------
# Project created by QtCreator 2010-04-28T20:25:30
# -------------------------------------------------
QT += testlib
QT -= gui

TARGET = telemetron-test
CONFIG += console
CONFIG -= app_bundle

OBJECTS_DIR = ../build/.obj-test
MOC_DIR = ../build/.moc-test
RCC_DIR = ../build/.rcc-test
LIBS += ../build/.obj/modbusrtuframe.o
INCLUDEPATH += ../src

TEMPLATE = app
SOURCES += main.cpp \
    sampletest.cpp \
    Sensors/Modbus/modbusframetest.cpp

HEADERS += AutoTest.h \
    sampletest.h \
    Sensors/Modbus/modbusframetest.h
