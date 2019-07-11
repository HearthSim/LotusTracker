#ifndef UNTAPPED_H
#define UNTAPPED_H

#include "api/untappedapi.h"
#include "entity/eventplayercourse.h"
#include "entity/matchinfo.h"

#include <QJsonObject>
#include <QObject>
#include <QStack>

class Untapped : public QObject
{
    Q_OBJECT
private:
    EventPlayerCourse eventPlayerCourse;
    UntappedAPI *untappedAPI;
    QString dataDir;
    QJsonObject matchDescriptor;
    void setupUntappedAPIConnections();
    void preparedMatchLogFile(QStack<QString> matchLogMsgs);

public:
    explicit Untapped(QObject *parent = nullptr);
    void checkForUntappedUploadToken();
    void setEventPlayerCourse(EventPlayerCourse eventPlayerCourse);
    void uploadLogFile(MatchInfo matchInfo, QStack<QString> matchLogMsgs);

signals:

public slots:
};

#endif // UNTAPPED_H
