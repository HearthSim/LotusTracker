#ifndef MTGARENA_H
#define MTGARENA_H

#include <QObject>
#include <QTimer>

class MtgArena : public QObject
{
    Q_OBJECT

private:
	QTimer *timer;

	void fetchCards();
    void findGameWindow();
    void gameStarted();
    void gameStopped();
	void gameFocusChanged(bool hasFocus);

public:
	explicit MtgArena(QObject *parent = nullptr);
	bool isFocused;
	bool isRunning;

signals:
	void sgnGameFocusChanged(bool hasFocus);
	void sgnGameStarted();
	void sgnGameStopped();

public slots:
};

#endif // MTGARENA_H
