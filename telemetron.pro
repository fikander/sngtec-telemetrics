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
    src/Configurator/configurator.cpp \
    src/Cloud/cloconnection.cpp \
    src/Message/message.cpp
HEADERS += src/Sensors/devproxy.h \
    src/Cloud/cloproxy.h \
    src/Configurator/configurator.h \
    src/Cloud/cloconnection.h \
    src/Message/message.h
