# -------------------------------------------------
# Project created by QtCreator 2011-12-08T18:18:14
# -------------------------------------------------
QT += network \
    xml
QT -= gui
TARGET = telemetron
CONFIG += console
CONFIG -= app_bundle
TEMPLATE = app
DESTDIR = bin/
DESTDIR = build
OBJECTS_DIR = build/.obj
MOC_DIR = build/.moc
RCC_DIR = build/.rcc
UI_DIR = build/.ui
SOURCES += src/main.cpp \
    src/Sensors/devproxy.cpp \
    src/Cloud/cloproxy.cpp \
    src/Cloud/cloconnection.cpp \
    src/Cloud/Mock/mockcloud.cpp \
    src/Cloud/Pachube/pachubexml.cpp \
    src/Cloud/Pachube/pachubecloud.cpp \
    src/Configurator/configurator.cpp \
    src/Message/message.cpp \
    src/Sensors/devconnection.cpp \
    src/Sensors/SNG/sngconnection.cpp \
    src/Sensors/Modbus/modbus.cpp \
    src/Sensors/SNG/sngphysicaladdress.cpp
HEADERS += src/Sensors/devproxy.h \
    src/Cloud/cloconnection.h \
    src/Cloud/cloproxy.h \
    src/Cloud/Mock/mockcloud.h \
    src/Cloud/Pachube/pachubexml.h \
    src/Cloud/Pachube/pachubecloud.h \
    src/Configurator/configurator.h \
    src/Message/message.h \
    src/Sensors/devconnection.h \
    src/Sensors/SNG/sngconnection.h \
    src/Sensors/Modbus/modbus.h \
    src/Sensors/SNG/sngphysicaladdress.h
