#ifndef UNTAPPEDMATCHDESCRIPTOR_H
#define UNTAPPEDMATCHDESCRIPTOR_H

#include "../entity/card.h"
#include "../entity/deck.h"
#include "../entity/eventplayercourse.h"
#include "../entity/matchdetails.h"
#include "../entity/resultspec.h"

#include <QObject>

class UntappedMatchDescriptor : public QObject
{
    Q_OBJECT
private:
    MatchDetails matchDetails;
    QJsonArray getMatchGamesDescriptor();
    QJsonObject getMatchPlayerDescriptor();
    QJsonArray getMatchOpponentsDescriptor();
    QJsonObject getMatchEventDescriptor(EventPlayerCourse eventPlayerCourse);
    QJsonArray cardsToJsonArray(QMap<Card*, int> cards);
    QJsonObject deckToJsonObject(Deck deck, QMap<Card*, int> playerCommanders);
    QJsonValue resultSpecToJsonObject(ResultSpec resultSpec);
    QJsonValue eventCourseIntToJsonValue(QString eventId, int value);
    QJsonValue stringOrNullJsonValue(QString value);
    QJsonValue intOrNullJsonValue(int value);
    QJsonValue doubleOrNullToJsonValue(double value);

public:
    explicit UntappedMatchDescriptor(QObject *parent = nullptr);
    QJsonDocument prepareNewDescriptor(MatchDetails matchDetails,
                                       QString timestamp, QString uploadToken,
                                       EventPlayerCourse eventPlayerCourse);

signals:

public slots:
};

#endif // UNTAPPEDMATCHDESCRIPTOR_H
