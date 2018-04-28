#include "mtgalogwatcher.h"
#include "macros.h"

#include <QByteArray>
#include <QDir>
#include <QFileInfo>
#include <QStandardPaths>

#define LOG_PATH "AppData" + QDir::separator() + "LocalLow" + QDir::separator() + "Wizards of the Coast" + QDir::separator() + "MTGA"

MtgaLogWatcher::MtgaLogWatcher(QObject *parent) : QObject(parent), 
	timer(new QTimer(this)), lastFilePos(0)
{
    QString homeDir = QStandardPaths::writableLocation(QStandardPaths::HomeLocation);
    QString logPath = homeDir + QDir::separator() + LOG_PATH;
    if(QFileInfo(logPath + QDir::separator() + "output_log.txt").exists()) {
    	setLogPath(logPath);
    } else {
        LOGW(QString("Game log file not found: %1").arg(logPath));
    }
    connect(timer, &QTimer::timeout, this, &MtgaLogWatcher::checkForNewLogs);
    MtgArena* mtgArena = (MtgArena*) parent;
    connect(mtgArena, &MtgArena::sgnGameStarted, this, &MtgaLogWatcher::startWatching);
    connect(mtgArena, &MtgArena::sgnGameStopped, this, &MtgaLogWatcher::stopWatching);
}

MtgaLogWatcher::~MtgaLogWatcher()
{
	DELETE(logFile);
	DELETE(timer);
}

void MtgaLogWatcher::setLogPath(QString logPath){
    logFile = new QFile(logPath + QDir::separator() + "output_log.txt");
    if(logFile->exists()){
		logFilePath = logPath;
        LOGD(QString("Log file: %1").arg(logFilePath));
	    lastFilePos = logFile->size();	
    } else {
        LOGW(QString("Game log file not found: %1").arg(logPath));
    }
}

void MtgaLogWatcher::startWatching(){
	if(logFilePath.isEmpty()) {
      	LOGW("Log file path was not setted");
		return;
	}
    if(!logFile->open(QIODevice::ReadOnly)) {
        LOGW("Error oppening log file");
        return;
    }
    LOGD("Starting watch log");
	timer->start(100);
}

void MtgaLogWatcher::stopWatching(){
    LOGD("Stopping watch log");
	timer->stop();
    logFile->close();
}

void MtgaLogWatcher::checkForNewLogs()
{
	qint64 logSize = logFile->size();
    if(logSize == lastFilePos){
        return;
    }
	if(logSize < lastFilePos){
		LOGW( "Log file reseted.");
		lastFilePos = 0;
	} else {
		logFile->seek(lastFilePos);
		QByteArray logNewContent = logFile->readAll();
		emit sgnNewLogContent(QString::fromUtf8(logNewContent.trimmed()));
		lastFilePos = logFile->pos();
	}
}
