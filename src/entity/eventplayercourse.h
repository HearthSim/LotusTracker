#ifndef EVENTPLAYERCOURSE_H
#define EVENTPLAYERCOURSE_H

#include "deck.h"

#include <QJsonArray>

class EventPlayerCourse
{
public:
    QString eventId;
    Deck currentDeck;
    int maxWins;
    int maxLosses;
    int currentWins;
    int currentLosses;
    QJsonArray processedMatchIds;

    EventPlayerCourse(QString eventId = "", Deck currentDeck = Deck(), int maxWins = -1, int maxLosses = -1,
                      int currentWins = -1, int currentLosses = -1, QJsonArray processedMatchIds = {}):
        eventId(eventId), currentDeck(currentDeck), maxWins(maxWins), maxLosses(maxLosses),
        currentWins(currentWins), currentLosses(currentLosses), processedMatchIds(processedMatchIds) {}

};

#endif // EVENTPLAYERCOURSE_H
