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

DESTDIR = build
OBJECTS_DIR = tmp
MOC_DIR = tmp
RCC_DIR = tmp
UI_DIR = tmp

# The following define makes your compiler emit warnings if you use
# any feature of Qt which has been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

HEADERS += \
    src/arenatracker.h \
    src/extensions.h \
    src/logger.h \
    src/macros.h \
    src/entity/card.h \
    src/entity/deck.h \
    src/entity/user.h \
    src/mtg/mtgarena.h \
    src/mtg/mtgalogparser.h \
    src/mtg/mtgalogwatcher.h \
    src/mtg/mtgcards.h \
    src/ui/preferences.h \
    src/ui/decktrackeroverlay.h \
    src/ui/decktrackerui.h \
    src/ui/trayicon.h

SOURCES += \
    src/main.cpp \
    src/arenatracker.cpp \
    src/extensions.cpp \
    src/logger.cpp \
    src/mtg/mtgarena.cpp \
    src/mtg/mtgalogparser.cpp \
    src/mtg/mtgalogwatcher.cpp \
    src/mtg/mtgcards.cpp \
    src/ui/decktrackeroverlay.cpp \
    src/ui/decktrackerui.cpp \
    src/ui/preferences.cpp \
    src/ui/trayicon.cpp

FORMS += \
    src/ui/decktrackeroverlay.ui \
    src/ui/preferences.ui

RESOURCES += resources.qrc

CONFIG(debug, debug|release): DEFINES += _DEBUG

mac {

  QT += macextras
  ICON = res/icons/icon.icns
  DEFINES += PLATFORM=\\\"mac\\\"

  QMAKE_INFO_PLIST = Info.plist.app

  INCLUDEPATH += "\ -F/Library/Frameworks"
  LIBS += -framework ApplicationServices -F/Library/Frameworks -framework AppKit

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
