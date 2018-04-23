#pragma once

#ifndef PREFERENCES_H
#define PREFERENCES_H

#include <QCloseEvent>
#include <QMainWindow>

namespace Ui {
class Preferences;
}

class Preferences : public QMainWindow
{
    Q_OBJECT

public:
    explicit Preferences(QWidget *parent = 0);
    ~Preferences();

private:
    void closeEvent(QCloseEvent *event);
    Ui::Preferences *ui;
};

#endif // PREFERENCES_H
