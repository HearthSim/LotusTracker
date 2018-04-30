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
    if (!QFile::exists(dataDir)) {
      QDir dir;
      dir.mkpath(dataDir);
    }
    qDebug("Logging to -> %s", qUtf8Printable(dataDir));
    QString appName = qApp->applicationName();
    logFile = new QFile(dataDir + QDir::separator() + appName + ".log");
    if (logFile) {
        logFile->open(QIODevice::WriteOnly | QIODevice::Text);
    }
}

Logger::~Logger()
{
    if (logFile) {
        delete logFile;
        logFile = NULL;
    }
}

void Logger::logI(const QString &source, const int line, const QString &msg)
{
    log(INFO, source, line, msg);
}

void Logger::logD(const QString &source, const int line, const QString &msg)
{
    log(DEBUG, source, line, msg);
}

void Logger::logW(const QString &source, const int line, const QString &msg)
{
    log(WARNING, source, line, msg);
}

void Logger::log(LogType type, const QString &source, const int line, const QString &msg)
{
    QString timestamp = QTime::currentTime().toString("hh:mm:ss");
    QString log = QString("[%1] %2 %3(%4): %5")
            .arg(timestamp)
            .arg(LOG_TYPE_NAMES[type], 7)
            .arg(source.left(source.indexOf("::")), 20)
            .arg(line)
            .arg(msg);

    switch (type) {
    case DEBUG:
        qDebug("%s", qUtf8Printable(log));
        break;
    case INFO:
        qInfo("%s", qUtf8Printable(log));
        break;
    case WARNING:
        qWarning("%s", qUtf8Printable(log));
        break;
    }

    if (logFile && logFile->isOpen()) {
      QTextStream out(logFile);
      out << log << endl;
      logFile->flush();
    }
    emit sgnLog(type, log);
}
