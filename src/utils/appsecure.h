#ifndef APPSECURE_H
#define APPSECURE_H

#include <QObject>
#include <keychain.h>

using namespace QKeychain;

class AppSecure : public QObject
{
    Q_OBJECT
public:
    explicit AppSecure(QObject *parent = nullptr);

    void store(QString key, QString value);
    QString restore(QString key);
    void remove(QString key);

private:
    WritePasswordJob wpj;
    ReadPasswordJob rpj;
    DeletePasswordJob dpj;

signals:

public slots:
};

#endif // APPSECURE_H
