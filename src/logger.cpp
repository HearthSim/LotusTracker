#include "logger.h"

#include <QApplication>
#include <QDir>
#include <QStandardPaths>
#include <QTextStream>
#include <QTime>
#include <QtGlobal>

Logger::Logger(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    if(!QFile::exists(dataDir)) {
      QDir dir;
      dir.mkpath(dataDir);
    }
    qDebug("Logging to -> %s", qUtf8Printable(dataDir));
    QString appName = qApp->applicationName();
    logFile = new QFile(dataDir + QDir::separator() + appName + ".log");
    if(logFile) {
        logFile->open(QIODevice::WriteOnly | QIODevice::Text);
    }
}

Logger::~Logger()
{
    if(logFile) {
        delete logFile;
        logFile = NULL;
    }
}

void Logger::logI(const QString &msg)
{
    log(INFO, msg);
}

void Logger::logD(const QString &msg)
{
    log(DEBUG, msg);
}

void Logger::logW(const QString &msg)
{
    log(WARNING, msg);
}

void Logger::log(LogType type, const QString &msg)
{
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString line = QString("[%1] %2: %3")
            .arg(timestamp)
            .arg(LOG_TYPE_NAMES[type])
            .arg(msg);

    switch (type) {
    case DEBUG:
        qDebug("%s", qUtf8Printable(line));
        break;
    case INFO:
        qInfo("%s", qUtf8Printable(line));
        break;
    case WARNING:
        qWarning("%s", qUtf8Printable(line));
        break;
    }

    if(logFile && logFile->isOpen()) {
      QTextStream out(logFile);
      out << line << endl;
      logFile->flush();
    }
    emit sgnLog(type, msg);
}
