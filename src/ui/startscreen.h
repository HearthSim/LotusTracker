#ifndef STARTSCREEN_H
#define STARTSCREEN_H

#include <QMainWindow>

namespace Ui { class Start; }

class StartScreen : public QMainWindow
{
    Q_OBJECT
private:
    Ui::Start *ui;
    void onLoginClick();
    void onNewUserClick();

public:
    explicit StartScreen(QWidget *parent = nullptr);
    ~StartScreen();

signals:

public slots:
};

#endif // STARTSCREEN_H
