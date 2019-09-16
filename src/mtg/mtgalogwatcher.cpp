#include "mtgalogwatcher.h"
#include "../macros.h"

#include <QByteArray>
#include <QDir>
#include <QFileInfo>

#define CHECK_FOR_NEW_LOGS_INTERVAL 1000
#define WATCH_TEST_LOG false
#define LOG_PATH_TEST "Documents"

MtgaLogWatcher::MtgaLogWatcher(QObject *parent) : QObject(parent), 
    logFile(nullptr), timer(new QTimer(this)), lastFilePos(0)
{
    enableVerboseLogs();
    logPath = LOTUS_TRACKER->appSettings->getLogPath();
    MtgArena* mtgArena = static_cast<MtgArena*>(parent);
    if (WATCH_TEST_LOG) {
        startWatching();
    } else {
        connect(mtgArena, &MtgArena::sgnMTGAFocusChanged,
                this, &MtgaLogWatcher::onGameFocusChanged);
        connect(mtgArena, &MtgArena::sgnMTGAStarted,
                this, &MtgaLogWatcher::startWatching);
        connect(mtgArena, &MtgArena::sgnMTGAStopped,
                this, &MtgaLogWatcher::stopWatching);
    }
    connect(timer, &QTimer::timeout,
            this, &MtgaLogWatcher::checkForNewLogs);
}

MtgaLogWatcher::~MtgaLogWatcher()
{
    DEL(logFile);
    DEL(timer);
}

void MtgaLogWatcher::setLogPath(QString logPath){
    this->logPath = logPath;
    logFile = new QFile(logPath + QDir::separator() + "output_log.txt");
    if (logFile->exists()) {
		logFilePath = logPath;
        LOGI(QString("Log file: %1").arg(logFilePath));
	    lastFilePos = logFile->size();	
    } else {
        LOGW(QString("Game log file not found: %1").arg(logPath));
    }
}

void MtgaLogWatcher::onGameFocusChanged(bool hasFocus)
{
    if (!hasFocus || timer->isActive()) {
        return;
    }
    if (QFileInfo(logPath + QDir::separator() + "output_log.txt").exists()) {
        setLogPath(logPath);
        startWatching();
    } else {
        LOGW(QString("Game log file not found: %1").arg(logPath));
    }
}

void MtgaLogWatcher::startWatching(){
    if (logFilePath.isEmpty()) {
        LOGW("Log file path was not setted. Looking for log file.");
        if (QFileInfo(logPath + QDir::separator() + "output_log.txt").exists()) {
            setLogPath(logPath);
        } else {
            LOGW(QString("Game log file not found: %1").arg(logPath));
            return;
        }
	}
    if (!logFile->open(QIODevice::ReadOnly | QIODevice::Text)) {
        LOGW("Error oppening log file");
        return;
    }
    LOGD("Starting watch log");
    timer->start(CHECK_FOR_NEW_LOGS_INTERVAL);
}

void MtgaLogWatcher::stopWatching(){
    LOGD("Stopping watch log");
	timer->stop();
    if (logFile->isOpen()) {
        logFile->close();
    }
}

void MtgaLogWatcher::checkForNewLogs()
{
	qint64 logSize = logFile->size();
    if (logSize == lastFilePos) {
        return;
    }
    if (logSize < lastFilePos) {
        LOGW("Log file reseted.");
		lastFilePos = 0;
	} else {
		logFile->seek(lastFilePos);
		QByteArray logNewContent = logFile->readAll();
        emit sgnNewLogContent(QString::fromUtf8(logNewContent));
        lastFilePos = logFile->pos();
    }
}

void MtgaLogWatcher::enableVerboseLogs()
{
    QSettings settings("Wizards Of The Coast", "MTGA");
    QString key = "UseVerboseLogs";
    QString fullKey = QString("%1_h%2").arg(key).arg(djb2Hash(key));
    settings.setValue(fullKey, 1);
}

unsigned int MtgaLogWatcher::djb2Hash(QString key)
{
    unsigned int hash = 5381;
    for (QChar c : key) {
        hash = (hash * 33) ^ c.unicode();
    }
    return hash;
}
