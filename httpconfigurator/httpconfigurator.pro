HTTPCONFIGURATOR_BASE = ..
TEMPLATE = app

TARGET = httpconfigurator
VERSION = 0.1.0

QT += network
QT -= gui
CONFIG += debug

QHTTPSERVER_BASE = ../../qhttpserver

INCLUDEPATH += $$QHTTPSERVER_BASE/src
LIBS += -L$$QHTTPSERVER_BASE/lib -lqhttpserver

SOURCES=httpconfigurator.cpp
HEADERS=httpconfigurator.h

OBJECTS_DIR = $$HTTPCONFIGURATOR_BASE/build-httpconfigurator
MOC_DIR = $$HTTPCONFIGURATOR_BASE/build-httpconfigurator
DESTDIR = $$HTTPCONFIGURATOR_BASE/bin
