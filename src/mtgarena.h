#ifndef MTGARENA_H
#define MTGARENA_H

#include <QObject>
#include <QTimer>

#include "mtgalogwatcher.h"

class MtgArena : public QObject
{
    Q_OBJECT

private:
	QTimer *timer;
	MtgaLogWatcher *logWatcher;

    void findGameWindow();
    void gameStarted();
    void gameStopped();
	void gameFocusChanged(bool hasFocus);
	void onNewLogContent(QString logNewContent);

public:
	explicit MtgArena(QObject *parent = nullptr);
	~MtgArena();
	bool isFocused;
	bool isRunning;

signals:
	void sgnGameFocusChanged(bool hasFocus);
	void sgnGameStarted();
	void sgnGameStopped();

public slots:
};

#endif // MTGARENA_H
