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
    src/Configurator/configurator.cpp \
    src/Message/message.cpp \
    src/Cloud/cloconnection.cpp \
    src/Cloud/Mock/mockcloud.cpp \
    src/Cloud/Pachube/pachubexml.cpp \
    src/Cloud/Pachube/pachubecloud.cpp \
    src/Sensors/devconnection.cpp \
    src/Sensors/Mock/mockdev.cpp \
    src/Sensors/SNG/sngconnection.cpp \
    src/Sensors/SNG/sngphysicaladdress.cpp \
    src/Sensors/Modbus/modbus.cpp \
    src/Sensors/Modbus/modbusrtuframe.cpp \
    src/Logger/loggingHandler.cpp \
    src/Logger/logger.cpp \
    src/Sensors/SNG/sngframe.cpp \
    src/Sensors/SNG/sngmsgcreator.cpp
HEADERS += src/Sensors/devproxy.h \
    src/Cloud/cloproxy.h \
    src/Configurator/configurator.h \
    src/Message/message.h \
    src/Cloud/cloconnection.h \
    src/Cloud/Mock/mockcloud.h \
    src/Cloud/Pachube/pachubexml.h \
    src/Cloud/Pachube/pachubecloud.h \
    src/Sensors/devconnection.h \
    src/Sensors/Mock/mockdev.h \
    src/Sensors/SNG/sngphysicaladdress.h \
    src/Sensors/SNG/sngconnection.h \
    src/Sensors/Modbus/modbus.h \
    src/Sensors/Modbus/modbusrtuframe.h \
    src/Logger/loggingHandler.h \
    src/Logger/logger.h \
    src/Sensors/SNG/sngframe.h \
    src/Sensors/SNG/sngmsgcreator.h
