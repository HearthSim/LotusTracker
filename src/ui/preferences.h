#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QCloseEvent>
#include <QMainWindow>

namespace Ui { class Preferences; }

class Preferences : public QMainWindow
{
    Q_OBJECT

private:
    void closeEvent(QCloseEvent *event);
    void onStartAtLoginChange();
    Ui::Preferences *ui;

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();
};

#endif // PREFERENCES_H
