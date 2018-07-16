#ifndef DATABASE_H
#define DATABASE_H

#include "entity/user.h"

#include <QObject>
#include <QNetworkAccessManager>

class FirebaseDatabase : public QObject
{
    Q_OBJECT
private:
    QNetworkAccessManager networkManager;
    void requestOnFinish();

public:
    explicit FirebaseDatabase(QObject *parent = nullptr);
    ~FirebaseDatabase();
    void updateUserInventory(PlayerInventory playerInventory);

signals:
    void sgnRequestFinished();

};

#endif // DATABASE_H
