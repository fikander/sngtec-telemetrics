#-------------------------------------------------
#
# Project created by QtCreator 2011-12-31T19:33:45
#
#-------------------------------------------------

QT       += testlib

QT       -= gui

TARGET = tst_modbusframetesttest
CONFIG   += console
CONFIG   -= app_bundle

TEMPLATE = app
LIBS += /home/toolchains/telemetron-build-desktop/build/.obj/modbusrtuframe.o


SOURCES += tst_modbusframetesttest.cpp
DEFINES += SRCDIR=\\\"$$PWD/\\\"
