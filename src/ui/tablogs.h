#ifndef TABLOGS_H
#define TABLOGS_H

#include "../utils/logger.h"

#include <QWidget>

namespace Ui { class TabLogs; }

class TabLogs : public QWidget
{
    Q_OBJECT

private:
    Ui::TabLogs *ui;

public:
    explicit TabLogs(QWidget *parent = nullptr);
    ~TabLogs();

signals:

public slots:
    void onNewLog(LogType type, const QString &log);

};

#endif // TABLOGS_H
