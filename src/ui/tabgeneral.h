#ifndef TABGENERAL_H
#define TABGENERAL_H

#include <QCloseEvent>
#include <QWidget>

namespace Ui { class TabGeneral; }

class TabGeneral : public QWidget
{
    Q_OBJECT

private:
    Ui::TabGeneral *ui;
    void applyCurrentSettings();
    void onStartAtLoginChanged();
    void onAutoUpdateChanged();
    void onPTEnabledChanged();
    void onOTEnabledChanged();
    void onRestoreDefaultsSettingsClicked();

public:
    explicit TabGeneral(QWidget *parent = nullptr);
    ~TabGeneral();

signals:
    void sgnRestoreDefaults();
    void sgnPlayerTrackerEnabled(bool enabled);
    void sgnOpponentTrackerEnabled(bool enabled);

public slots:
};

#endif // TABGENERAL_H
