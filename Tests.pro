include(arenatracker.pro)
SOURCES -= src/main.cpp

QT += testlib
QT -= gui

TEMPLATE = app
TARGET = test

CONFIG += qt console warn_on depend_includepath testcase
CONFIG -= app_bundle

HEADERS += \
	tests/macros_test.h \
	tests/testmtgalogparser.hpp

SOURCES += \
	tests/main.cpp

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

RESOURCES += tests/resources_test.qrc

# install
target.path = $$[QT_INSTALL_EXAMPLES]/qtestlib/tutorial1
INSTALLS += target
