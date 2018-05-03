#ifndef MTGARENA_H
#define MTGARENA_H

#include "mtgalogparser.h"
#include "mtgalogwatcher.h"
#include "mtgcards.h"

#include <QObject>
#include <QTimer>

class MtgArena : public QObject
{
    Q_OBJECT

private:
    QTimer *timer;
    MtgaLogParser *logParser;
	MtgaLogWatcher *logWatcher;

    void findGameWindow();
    void gameStarted();
    void gameStopped();
	void gameFocusChanged(bool hasFocus);
	void onNewLogContent(QString logNewContent);

public:
    explicit MtgArena(QObject *parent = nullptr, MtgCards *mtgCards = nullptr);
    ~MtgArena();
	bool isFocused;
	bool isRunning;
    MtgaLogParser* getLogParser();

signals:
	void sgnGameFocusChanged(bool hasFocus);
	void sgnGameStarted();
	void sgnGameStopped();

public slots:
};

#endif // MTGARENA_H
