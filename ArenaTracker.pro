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
    src/logger.cpp \
    src/mtgarena.cpp \
    src/mtgalogwatcher.cpp \
    src/mtgcards.cpp \
    src/trayicon.cpp \
    src/ui/preferences.cpp \
    src/mtgalogparser.cpp

HEADERS += \
    src/arenatracker.h \
    src/entities.h \
    src/logger.h \
    src/macros.h \
    src/mtgarena.h \
    src/mtgalogwatcher.h \
    src/mtgcards.h \
    src/trayicon.h \
    src/ui/preferences.h \
    src/mtgalogparser.h

FORMS += \
    src/ui/preferences.ui

RESOURCES += resources.qrc

CONFIG(debug, debug|release): DEFINES += _DEBUG

mac {

  QT += macextras
  ICON = res/icon.icns
  DEFINES += PLATFORM=\\\"mac\\\"

  QMAKE_INFO_PLIST = Info.plist.app

  LIBS += -framework ApplicationServices

  SOURCES += src/utils/macautostart.cpp \
      src/utils/macwindowfinder.cpp

  HEADERS += src/utils/macautostart.h \
      src/utils/macwindowfinder.h

}

win32 {

  QT += winextras
  RC_FILE += arenatracker.rc

  CONFIG += embed_manifest_exe
  DEFINES += PLATFORM=\\\"win32\\\"
  DEFINES += _CRT_SECURE_NO_WARNINGS

  LIBS += -luser32 -lpsapi

  SOURCES += src/utils/winautostart.cpp \
      src/utils/winwindowfinder.cpp

  HEADERS += src/utils/winautostart.h \
      src/utils/winwindowfinder.h

}
