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
    void createPatchRequest(QUrl url, QJsonDocument body, QString userToken);
    void requestOnFinish();

public:
    explicit FirebaseDatabase(QObject *parent = nullptr);
    ~FirebaseDatabase();
    void updatePlayerCollection(QMap<int, int> ownedCards);
    void updateUserInventory(PlayerInventory playerInventory);

signals:
    void sgnRequestFinished();

};

#endif // DATABASE_H
