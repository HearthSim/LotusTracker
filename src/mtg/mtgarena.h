#ifndef MTGARENA_H
#define MTGARENA_H

#include "mtgalogparser.h"
#include "mtgalogwatcher.h"

#include <QObject>
#include <QTimer>

class MtgArena : public QObject
{
    Q_OBJECT

private:
    QString mtgaVersion;
    QTimer *timer;
    MtgaLogParser *logParser;
	MtgaLogWatcher *logWatcher;

    void findGameWindow();
    void onCurrentFocusChanged(bool hasFocus);
    void onMtgaStarted();
    void onMtgaStopped();
    void onMtgaFocusChanged(bool hasFocus);
    void onNewLogContent(QString logNewContent);

public:
    explicit MtgArena(QObject *parent = nullptr);
    ~MtgArena();
	bool isFocused;
	bool isRunning;
    QString getClientVersion();
    MtgaLogParser* getLogParser();

signals:
	void sgnMTGAFocusChanged(bool hasFocus);
    void sgnMTGAStarted();
    void sgnMTGAStopped();

public slots:
    void onLogFilePathChanged(QString logPath);
    void onMtgaClientVersion(QString version);
};

#endif // MTGARENA_H
