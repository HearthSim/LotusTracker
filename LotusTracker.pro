#-------------------------------------------------
#
# Project created by QtCreator 2018-04-20T21:27:58
#
#-------------------------------------------------

QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

TARGET = LotusTracker
TEMPLATE = app
VERSION = 1.4.1

CONFIG += c++11

# asmCrashReport - https://github.com/asmaloney/asmCrashReport
include(asmCrashReport.pri)

# QtKeychain
INCLUDEPATH += ./include
INCLUDEPATH += ../qtkeychain

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
    src/api/lotusapi.h \
    src/api/requestdata.h \
    src/api/rqtparsedeckposside.h \
    src/api/rqtplayerdeck.h \
    src/api/rqtplayerdeckpublish.h \
    src/api/rqtplayerdeckupdate.h \
    src/api/rqtupdateplayerinventory.h \
    src/api/rqtupdateplayercollection.h \
    src/api/rqtuploadmatch.h \
    src/api/rqtuploadeventresult.h \
    src/api/rqtuploadplayermatch.h \
    src/api/untappedapi.h \
    src/entity/card.h \
    src/entity/deck.h \
    src/entity/deckarch.h \
    src/entity/eventplayercourse.h \
    src/entity/gamedetails.h \
    src/entity/gameinfo.h \
    src/entity/matchdetails.h \
    src/entity/resultspec.h \
    src/entity/user.h \
    src/entity/matchplayer.h \
    src/entity/matchstatediff.h \
    src/entity/matchzone.h \
    src/entity/matchzonetransfer.h \
    src/entity/opponentinfo.h \
    src/entity/untappeduploaddata.h \
    src/mtg/mtgarena.h \
    src/mtg/mtgalogparser.h \
    src/mtg/mtgalogwatcher.h \
    src/mtg/mtgamatch.h \
    src/mtg/mtgcards.h \
    src/mtg/mtgdecksarch.h \
    src/ui/cardblinkinfo.h \
    src/ui/deckoverlaybase.h \
    src/ui/deckoverlaydraft.h \
    src/ui/deckoverlayopponent.h \
    src/ui/deckoverlayplayer.h \
    src/ui/preferencesscreen.h \
    src/ui/startscreen.h \
    src/ui/tababout.h \
    src/ui/tabgeneral.h \
    src/ui/tablogs.h \
    src/ui/taboverlay.h \
    src/ui/trayicon.h \
    src/utils/appsecure.h \
    src/utils/appsettings.h \
    src/utils/influxdb.hpp \
    src/utils/logger.h \
    src/utils/lotusexception.h \
    src/utils/metrics.h \
    src/utils/untapped.h \
    src/utils/untappedmatchdescriptor.h \
    src/updater/sparkleupdater.h \
    src/credentials.h \
    src/ganalytics.h \
    src/lotustracker.h \
    src/macros.h \
    src/transformations.h \
    src/urls.h

SOURCES += \
    src/api/lotusapi.cpp \
    src/api/untappedapi.cpp \
    src/entity/matchdetails.cpp \
    src/main.cpp \
    src/ganalytics.cpp \
    src/lotustracker.cpp \
    src/transformations.cpp \
    src/entity/deck.cpp \
    src/mtg/mtgarena.cpp \
    src/mtg/mtgalogparser.cpp \
    src/mtg/mtgalogwatcher.cpp \
    src/mtg/mtgamatch.cpp \
    src/mtg/mtgcards.cpp \
    src/mtg/mtgdecksarch.cpp \
    src/ui/deckoverlaybase.cpp \
    src/ui/deckoverlaydraft.cpp \
    src/ui/deckoverlayopponent.cpp \
    src/ui/deckoverlayplayer.cpp \
    src/ui/preferencesscreen.cpp \
    src/ui/startscreen.cpp \
    src/ui/tababout.cpp \
    src/ui/tabgeneral.cpp \
    src/ui/tablogs.cpp \
    src/ui/taboverlay.cpp \
    src/ui/trayicon.cpp \
    src/utils/appsecure.cpp \
    src/utils/appsettings.cpp \
    src/utils/logger.cpp \
    src/utils/metrics.cpp \
    src/utils/untapped.cpp \
    src/utils/untappedmatchdescriptor.cpp

FORMS += \
    src/ui/decktrackerbase.ui \
    src/ui/preferences.ui \
    src/ui/start.ui \
    src/ui/tababout.ui \
    src/ui/tabgeneral.ui \
    src/ui/tablogs.ui \
    src/ui/taboverlay.ui

DEFINES += VERSION=\\\"$$VERSION\\\"

RESOURCES += resources.qrc

mac {

    QT += macextras
    ICON = res/icons/icon.icns
    DEFINES += PLATFORM=\\\"mac\\\"

    QMAKE_INFO_PLIST = Info.plist.app

    INCLUDEPATH += "\ -F/Library/Frameworks"

    LIBS += -L../qtkeychain/build -lqt5keychain
    LIBS += -framework ApplicationServices -F/Library/Frameworks \
      -framework AppKit -framework Sparkle

    HEADERS += src/utils/macautostart.h \
        src/utils/cocoainitializer.h \
        src/utils/macwindowfinder.h \
        src/updater/macsparkleupdater.h

    SOURCES += src/utils/macautostart.cpp \
        src/utils/macwindowfinder.cpp

    OBJECTIVE_SOURCES += \
        src/utils/cocoainitializer.mm \
        src/updater/macsparkleupdater.mm

}

win32 {

    QT += winextras
    RC_FILE += lotustracker.rc

    CONFIG += embed_manifest_exe
    DEFINES += PLATFORM=\\\"win32\\\"
    DEFINES += _CRT_SECURE_NO_WARNINGS

    LIBS += -luser32 -lpsapi

    INCLUDEPATH += ../WinSparkle/include
    LIBS += -L../WinSparkle/Release -lWinSparkle

    CONFIG(debug, debug|release) {
        INCLUDEPATH += ../qtkeychain/build/Debug
        LIBS += -L../qtkeychain/build/Debug -llibqt5keychain
    }

    CONFIG(release, debug|release) {
        INCLUDEPATH += ../qtkeychain/build/Release
        LIBS += -L../qtkeychain/build/Release -llibqt5keychain
        #Lib crow and dependencies
        LIBS += -L$$PWD/libs -llibcrow
        LIBS += -L$$PWD/libs -llibcurl
        LIBS += -L$$PWD/libs -llibcrypto
        LIBS += -L$$PWD/libs -llibssl
        LIBS += -L$$PWD/libs -llibzlibstatic
        LIBS += -L$$PWD/libs -llibws2_32
    }

    HEADERS += src/utils/winautostart.h \
        src/utils/winwindowfinder.h \
        src/updater/winsparkleupdater.h

    SOURCES += src/utils/winautostart.cpp \
        src/utils/winwindowfinder.cpp \
        src/updater/winsparkleupdater.cpp

}
