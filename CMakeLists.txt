#set cmaeke minimum version
cmake_minimum_required(VERSION 2.8)


# set project's name
PROJECT( TelemetronProject )

# with SET() command you can change variables or define new ones
# here we define TELEMETRON_SRCS variable that contains a list of all .cpp files
# note that we don't need  at the end of line
SET( TELEMETRON_SRCS
  src/main.cpp 
  src/Configurator/configurator.cpp
  src/Logger/logger.cpp
  src/Message/message.cpp
  src/Cloud/cloproxy.cpp
  src/Cloud/cloconnection.cpp
  src/Cloud/Mock/mockcloud.cpp
  src/Cloud/Pachube/pachubexml.cpp 
  src/Cloud/Pachube/pachubecloud.cpp 
  src/Sensors/Mock/mockdev.cpp
  src/Sensors/devproxy.cpp
  src/Sensors/devconnection.cpp 
  src/Sensors/SNG/sngconnection.cpp 
  src/Sensors/SNG/sngphysicaladdress.cpp
  src/Sensors/SNG/sngframe.cpp
  src/Sensors/SNG/sngmsgcreator.cpp
  src/Sensors/SNG/sngmsgparser.cpp
  src/Sensors/Modbus/modbus.cpp
  src/Sensors/Modbus/modbusrtuframe.cpp
)

# another list, this time it includes all header files that should be treated with moc
SET( TELEMETRON_HEADERS
	)

SET( TELEMETRON_MOC_HDRS
  src/Message/message.h 
  src/Sensors/devproxy.h 
  src/Logger/logger.h
  src/Cloud/cloconnection.h
  src/Cloud/cloproxy.h 
  src/Cloud/Mock/mockcloud.h 
  src/Cloud/Pachube/pachubexml.h 
  src/Cloud/Pachube/pachubecloud.h 
  src/Configurator/configurator.h 
  src/Sensors/devconnection.h 
  src/Sensors/SNG/sngconnection.h 
  src/Sensors/SNG/sngphysicaladdress.h 
  src/Sensors/SNG/sngframe.h
  src/Sensors/SNG/sngmsgcreator.h
  src/Sensors/SNG/sngmsgparser.h
  src/Sensors/Mock/mockdev.h 
  src/Sensors/Modbus/modbus.h
  src/Sensors/Modbus/modbusrtuframe.h
)

# enable warnings
ADD_DEFINITIONS( -Wall )

# by default only QtCore and QtGui modules are enabled
# other modules must be enabled like this:
#SET( QT_USE_QTGUI TRUE )
SET( QT_DONT_USE_QTGUI TRUE )
SET( QT_USE_QTNETWORK TRUE )
SET( QT_USE_QTXML TRUE )


# this command finds Qt4 libraries and sets all required variables
# note that it's Qt4, not QT4 or qt4
FIND_PACKAGE( Qt4 REQUIRED )

# add some useful macros and variables
# (QT_USE_FILE is a variable defined by FIND_PACKAGE( Qt4 ) that contains a path to CMake script)
INCLUDE( ${QT_USE_FILE} )

# this command will generate rules that will run rcc on all files from TELEMETRON_RCS
# in result TELEMETRON_RC_SRCS variable will contain paths to files produced by rcc
# so far, commented out. Uncomment if any resource files are used.
#QT4_ADD_RESOURCES( TELEMETRON_RC_SRCS ${TELEMETRON_RCS} )

# this will run uic on .ui files:
# So far commented out. We will no use them, but I leave it here so that one can use this file as any Qt cmake-base template.
#QT4_WRAP_UI( TELEMETRON_UI_HDRS ${TELEMETRON_UIS} )

# and finally this will run moc:
QT4_WRAP_CPP( TELEMETRON_MOC_SRCS ${TELEMETRON_MOC_HDRS} )

# we need this to be able to include headers produced by uic in our code
# (CMAKE_BINARY_DIR holds a path to the build directory, while INCLUDE_DIRECTORIES() works just like INCLUDEPATH from qmake)
INCLUDE_DIRECTORIES( 
  ${CMAKE_BINARY_DIR} 
  ./src
  )
											

# here we instruct CMake to build "BACProject" executable from all of the source files
ADD_EXECUTABLE( telemetron ${TELEMETRON_SRCS} ${TELEMETRON_MOC_SRCS} ${TELEMETRON_RC_SRCS} ${TELEMETRON_UI_HDRS} )

# last thing we have to do is to tell CMake what libraries our executable needs,
# luckily FIND_PACKAGE prepared QT_LIBRARIES variable for us:
TARGET_LINK_LIBRARIES( telemetron ${QT_LIBRARIES} )
