#ifndef RQTUPDATEPLAYERDECK_H
#define RQTUPDATEPLAYERDECK_H

#include "firestorerequest.h"
#include "rqtcreateplayerdeck.h"
#include "../entity/deck.h"

#include <QDate>

class RqtUpdatePlayerDeck : public RqtCreatePlayerDeck
{
public:
    RqtUpdatePlayerDeck(QString userId, Deck deck)
        : RqtCreatePlayerDeck(userId, deck) {
        _path += "?updateMask.fieldPaths=name";
        _path += "&updateMask.fieldPaths=cards";
        _path += "&updateMask.fieldPaths=colors";
        _hasDuplicateQuery = true;
    }
    virtual ~RqtUpdatePlayerDeck() {}

};

#endif // RQTUPDATEPLAYERDECK_H
