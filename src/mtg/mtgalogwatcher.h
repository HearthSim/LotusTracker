#ifndef MTGALOGWATCHER_H
#define MTGALOGWATCHER_H

#include <QFile>
#include <QString>
#include <QObject>
#include <QTimer>

class MtgaLogWatcher : public QObject
{
    Q_OBJECT

private:
    QString logPath;
	QFile *logFile;
    QTimer *timer;
    QString logFilePath;
    qint64 lastFilePos;
    void checkForNewLogs();

public:
    MtgaLogWatcher(QObject *parent);
    ~MtgaLogWatcher();
    void setLogPath(QString filePath);
    void onGameFocusChanged(bool hasFocus);
    void startWatching();
    void stopWatching();

signals:
	void sgnNewLogContent(QString logNewContent);

public slots:
};

#endif // MTGALOGWATCHER_H
