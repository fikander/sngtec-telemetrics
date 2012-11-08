isEmpty(PREFIX):PREFIX = /usr/local
isEmpty(LIBDIR):LIBDIR = $${PREFIX}/lib
isEmpty(INCLUDEDIR):INCLUDEDIR = $${PREFIX}/include

LIBMODBUS_BASE = ..
TEMPLATE = lib

TARGET = libmodbus
VERSION = 3.0.3

QT -= gui network

CONFIG += dll debug

INCLUDEPATH += $$LIBMODBUS_BASE/libmodbus

PUBLIC_HEADERS += modbus.h modbus-version.h modbus-rtu.h modbus-tcp.h

HEADERS = $$PRIVATE_HEADERS $$PUBLIC_HEADERS
SOURCES = *.c

OBJECTS_DIR = $$LIBMODBUS_BASE/build-libmodbus
MOC_DIR = $$LIBMODBUS_BASE/build-libmodbus
DESTDIR = $$LIBMODBUS_BASE/lib

target.path = $$LIBDIR
headers.path = $$INCLUDEDIR
headers.files = $$PUBLIC_HEADERS
INSTALLS += target headers
