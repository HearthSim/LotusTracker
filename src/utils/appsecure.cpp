#include "appsecure.h"
#include "../macros.h"

AppSecure::AppSecure(QObject *parent) : QObject(parent),
    wpj(QLatin1String(APP_NAME)), rpj(QLatin1String(APP_NAME)), dpj(QLatin1String(APP_NAME)) {
    wpj.setAutoDelete(false);
    rpj.setAutoDelete(false);
    dpj.setAutoDelete(false);
}

void AppSecure::store(QString key, QString value)
{
    wpj.setKey(key);
    wpj.setTextData(value);
    QEventLoop loop;
    wpj.connect(&wpj, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    wpj.start();
    loop.exec();
    if (wpj.error()) {
        LOGW(QString("Storing key failed: %1").arg(wpj.errorString()));
    }
}

QString AppSecure::restore(QString key)
{
    rpj.setKey(key);
    QEventLoop loop;
    rpj.connect(&rpj, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    rpj.start();
    loop.exec();
    const QString value = rpj.textData();
    if (rpj.error()) {
        LOGW(QString("Restoring key failed: %1").arg(rpj.errorString()));
    }
    return value;

}

void AppSecure::remove(QString key)
{
    dpj.setKey(key);
    QEventLoop loop;
    dpj.connect(&dpj, SIGNAL(finished(QKeychain::Job*)), &loop, SLOT(quit()));
    dpj.start();
    loop.exec();
    if (dpj.error()) {
        LOGW(QString("Delete password failed: %1").arg(dpj.errorString()));
    }
}
