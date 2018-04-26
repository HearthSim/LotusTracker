#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T21:27:58
#
#-------------------------------------------------

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = ArenaTracker
TEMPLATE = app
VERSION = 0.0.1

OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

SOURCES += \
    src/main.cpp \
    src/arenatracker.cpp \
    src/trayicon.cpp \
    src/logger.cpp \
    src/mtgarena.cpp \
    src/ui/preferences.cpp \
    src/mtgcards.cpp

HEADERS += \
    src/arenatracker.h \
    src/trayicon.h \
    src/logger.h \
    src/macros.h \
    src/mtgarena.h \
    src/ui/preferences.h \
    src/entities.h \
    src/mtgcards.h

FORMS += \
    src/ui/preferences.ui

RESOURCES += resources.qrc

mac {

  QT += macextras
  ICON = res/icon.icns
  DEFINES += PLATFORM=\\\"mac\\\"

  QMAKE_INFO_PLIST = Info.plist.app

  LIBS += -framework ApplicationServices

  SOURCES += src/utils/MacOSWindowFinder.cpp

  HEADERS += src/utils/MacOSWindowFinder.h

}

win32 {

  QT += winextras
  ICON = res/icon.ico
  CONFIG += embed_manifest_exe
  DEFINES += PLATFORM=\\\"win32\\\"
  DEFINES += _CRT_SECURE_NO_WARNINGS

  LIBS += user32.lib

  SOURCES += src/utils/WinWindowFinder.cpp

  HEADERS += src/utils/WinWindowFinder.h

}
