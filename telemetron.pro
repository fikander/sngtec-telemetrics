# -------------------------------------------------
# Project created by QtCreator 2011-12-08T18:18:14
# -------------------------------------------------
QT += network
QT -= gui
TARGET = telemetron
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
SOURCES += src/main.cpp \
    src/Sensors/devproxy.cpp \
    src/Cloud/cloproxy.cpp \
    src/configurator.cpp \
    src/Cloud/cloconnection.cpp \
    src/message.cpp
HEADERS += src/Sensors/devproxy.h \
    src/Cloud/cloproxy.h \
    configurator.h \
    src/configurator.h \
    src/Cloud/cloconnection.h \
    src/message.h
