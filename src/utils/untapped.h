#ifndef UNTAPPED_H
#define UNTAPPED_H

#include "api/untappedapi.h"
#include "entity/eventplayercourse.h"
#include "entity/matchinfo.h"

#include <QFile>
#include <QJsonObject>
#include <QObject>
#include <QStack>

class Untapped : public QObject
{
    Q_OBJECT
private:
    EventPlayerCourse eventPlayerCourse;
    UntappedAPI *untappedAPI;
    QString tempDir;
    QJsonObject matchDescriptor;
    MatchInfo matchInfo;
    void setupUntappedAPIConnections();
    void preparedMatchLogFile(QStack<QString> matchLogMsgs);
    void preparedMatchDescriptor(QString timestamp);
    void preparedPutPayloadFile();
    QJsonValue eventCourseIntToJsonValue(int value);

public:
    explicit Untapped(QObject *parent = nullptr);
    void checkForUntappedUploadToken();
    void setEventPlayerCourse(EventPlayerCourse eventPlayerCourse);
    void uploadLogFile(MatchInfo matchInfo, QStack<QString> matchLogMsgs);

signals:

private slots:
    void onS3PutInfo(QString putUrl, QString timestamp);

public slots:
};

#endif // UNTAPPED_H
