#ifndef TABABOUT_H
#define TABABOUT_H

#include <QWidget>

namespace Ui { class TabAbout; }

class TabAbout : public QWidget
{
    Q_OBJECT

private:
    Ui::TabAbout *ui;

public:
    explicit TabAbout(QWidget *parent = nullptr);
    ~TabAbout();

signals:

public slots:
};

#endif // TABABOUT_H
