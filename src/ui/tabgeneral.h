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
    void onDOEnabledChanged();
    void onPOEnabledChanged();
    void onOOEnabledChanged();
    void onHideOnLoseGameFocusChanged();
    void onRestoreDefaultsSettingsClicked();

public:
    explicit TabGeneral(QWidget *parent = nullptr);
    ~TabGeneral();

signals:
    void sgnRestoreDefaults();
    void sgnDraftOverlayEnabled(bool enabled);
    void sgnPlayerOverlayEnabled(bool enabled);
    void sgnOpponentOverlayEnabled(bool enabled);

public slots:
};

#endif // TABGENERAL_H
