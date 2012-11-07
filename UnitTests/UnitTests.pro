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
LIBS +=

SOURCES += main.cpp \
    sampletest.cpp

HEADERS += \
    sampletest.h \
    TestsManager.h
