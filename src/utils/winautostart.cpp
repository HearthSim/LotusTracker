#include "winautostart.h"

#include <QApplication>
#include <QDir>
#include <QString>
#include <QSettings>
#include <QFileInfo>

#define REGISTER_RUN_PATH "HKEY_CURRENT_USER\\Software\\Microsoft\\Windows\\CurrentVersion\\Run"

// All credits to Track o'bot - https://github.com/stevschmid/track-o-bot

bool WinAutoStart::isEnabled()
{
    QString applicationName = QCoreApplication::applicationName();
    QSettings settings(REGISTER_RUN_PATH, QSettings::NativeFormat);
    return !settings.value(applicationName).toString().isEmpty();
}

void WinAutoStart::setEnabled(bool enabled)
{
    QString applicationName = QCoreApplication::applicationName();
    QString applicationPath = QCoreApplication::applicationFilePath();

    QSettings settings(REGISTER_RUN_PATH, QSettings::NativeFormat);
    if(enabled) {
        QString filePath = QDir::toNativeSeparators(QFileInfo(applicationPath).filePath());
        settings.setValue(applicationName, QString("\"%1\"").arg(filePath));
    } else {
        settings.remove(applicationName);
    }
}
