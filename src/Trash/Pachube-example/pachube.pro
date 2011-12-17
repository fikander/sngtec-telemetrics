BASEDIR = /home/toolchains/out/tinyputer/telemetrics/ 
TEMPLATE = app
TARGET +=
DEPENDPATH += .
INCLUDEPATH += .
QT += xml
QT += network

# Input
SOURCES += src/Trash/Pachube-example/main.cpp \
           src/Cloud/Pachube/pachubecloud.cpp \
           src/Cloud/Pachube/pachubexml.cpp \
           src/Cloud/cloconnection.cpp \
           src/Message/message.cpp \
           src/Configurator/configurator.cpp
HEADERS += \
           src/Cloud/Pachube/pachubecloud.h \
           src/Cloud/Pachube/pachubexml.h \
           src/Cloud/cloconnection.h \
           src/Message/message.h \
           src/Configurator/configurator.h
CONFIG += console

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
sources.files = $$SOURCES $$HEADERS $$FORMS $$RESOURCES *.pro *.png
sources.path = $$[QT_INSTALL_EXAMPLES]/qtconcurrent/runfunction
INSTALLS += target sources

