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
    void onChangeLogPathClicked();
    void onPOEnabledChanged();
    void onOOEnabledChanged();
    void onSDADEnabledChanged();
    void onDOEnabledChanged();
    void onHideOnLoseGameFocusChanged();
    void onRestoreDefaultsSettingsClicked();

public:
    explicit TabGeneral(QWidget *parent = nullptr);
    ~TabGeneral();

signals:
    void sgnLogFilePathChanged(QString logPath);
    void sgnRestoreDefaults();
    void sgnDeckOverlayDraftEnabled(bool enabled);
    void sgnPlayerOverlayEnabled(bool enabled);
    void sgnOpponentOverlayEnabled(bool enabled);
    void sgnDraftOverlaySource(QString source);

public slots:
};

#endif // TABGENERAL_H
