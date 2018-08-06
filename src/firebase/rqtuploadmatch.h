#ifndef RQTUPLOADMATCH_H
#define RQTUPLOADMATCH_H

#include "firestorerequest.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

#include <QDate>

class RqtUploadMatch: public FirestoreRequest
{
public:
    RqtUploadMatch(MatchInfo matchInfo, QString playerRankClass,
                   Deck playerDeck, Deck opponentDeck) {
        QString first = matchInfo.playerGoFirst ? "player1" : "player2";
        QString winner = matchInfo.playerWins ? "player1" : "player2";
        QString result = QString("%1x%2").arg(matchInfo.playerGameWins)
                .arg(matchInfo.playerGameLoses);
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                    {"event", QJsonObject{
                            {"stringValue", matchInfo.eventId}
                        }},
                    {"mode", QJsonObject{
                            {"stringValue", MatchInfo::MatchModeToString(matchInfo.mode)}
                        }},
                    {"first", QJsonObject{
                            {"stringValue", first}
                        }},
                    {"player1", QJsonObject{
                            {"mapValue", toJsonFields(playerDeck, matchInfo,
                                                      true, playerRankClass)}
                        }},
                    {"player2", QJsonObject{
                            {"mapValue", toJsonFields(opponentDeck, matchInfo, false) }
                        }},
                    {"result", QJsonObject{
                            {"stringValue", result}
                        }},
                    {"winner", QJsonObject{
                            {"stringValue", winner}
                        }}
                }}
        };

        _body = QJsonDocument(jsonObj);
        QDate date = QDate::currentDate();
        _path = QString("matches/%2/%3").arg(date.year()).arg(date.month());
    }

private:
    QJsonObject toJsonFields(Deck deck, MatchInfo matchInfo,
                             bool player, QString playerRankClass = QString(""))
    {
        QJsonObject jsonCards;
        QMap<Card*, int> cards = player ? deck.cards() : deck.currentCards();
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId),
                             QJsonObject{{ "integerValue", QString::number(cards[card]) }});
        }
        QString color = player ? deck.colorIdentity() : deck.colorIdentity(false);
        bool mulligan = player ? matchInfo.playerTakesMulligan
                               : matchInfo.opponentTakesMulligan;
        QString rank = player ? playerRankClass
                              : matchInfo.opponentInfo.opponentRankClass();
        QJsonObject jsonFields{
            {"fields", QJsonObject{
                    { "cards", QJsonObject{
                            { "mapValue", QJsonObject{
                                    {"fields", jsonCards}
                                }}
                        }},
                    { "colors", QJsonObject{
                            { "stringValue", color }
                        }},
                    {"mulligan", QJsonObject{
                            { "booleanValue", mulligan }}
                    },
                    {"rank", QJsonObject{
                            { "stringValue", rank }
                        }}
                }
            }
        };
        return jsonFields;
    }

};

#endif // RQTUPLOADMATCH_H
