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
    src/Cloud/cloconnection.cpp \
    src/Cloud/Mock/mockcloud.cpp \
    src/Configurator/configurator.cpp \
    src/Message/message.cpp
HEADERS += src/Sensors/devproxy.h \
    src/Cloud/cloconnection.h \
    src/Cloud/cloproxy.h \
    src/Cloud/Mock/mockcloud.h \
    src/Configurator/configurator.h \
    src/Message/message.h
