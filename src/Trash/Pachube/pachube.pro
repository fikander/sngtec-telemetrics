TEMPLATE = app
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
QT += xml
QT += network

# Input
SOURCES += main.cpp pachube.cpp
HEADERS += pachube.h
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
INSTALLS += target sources

