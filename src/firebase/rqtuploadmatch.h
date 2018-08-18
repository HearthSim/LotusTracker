#ifndef RQTUPLOADMATCH_H
#define RQTUPLOADMATCH_H

#include "firestorerequest.h"
#include "../entity/deck.h"
#include "../entity/matchinfo.h"

#include <QDate>

class RqtUploadMatch: public FirestoreRequest
{
public:
    RqtUploadMatch(MatchInfo matchInfo, Deck playerDeck, QString playerRankClass) {
        QJsonObject jsonGamesMapValue = games2JsonMapValue(matchInfo.games);
        QString winner = matchInfo.playerMatchWins ? "player1" : "player2";
        QString result = QString("%1x%2").arg(matchInfo.playerGameWins)
                .arg(matchInfo.playerGameLoses);
        QJsonObject jsonObj{
            {"fields", QJsonObject{
                    {"event", QJsonObject{
                            {"stringValue", matchInfo.eventId}
                        }},
                    {"games", QJsonObject{
                            {"mapValue", QJsonObject{
                                {"fields", jsonGamesMapValue }
                            }}
                        }},
                    {"mode", QJsonObject{
                            {"stringValue", MatchInfo::MatchModeToString(matchInfo.mode)}
                        }},
                    {"player1", QJsonObject{
                            {"mapValue", player1JsonFields(playerDeck, playerRankClass)}
                        }},
                    {"player2", QJsonObject{
                            {"mapValue", player2JsonFields(matchInfo) }
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
        _path = QString("matches/%2/%3-%4").arg(date.year())
                .arg(date.month()).arg(date.day());
    }

private:
    QJsonObject cards2JsonMapValue(QMap<Card*, int> cards)
    {
        QJsonObject jsonCards;
        for (Card* card : cards.keys()) {
            jsonCards.insert(QString::number(card->mtgaId),
                             QJsonObject{{ "integerValue", QString::number(cards[card]) }});
        }
        return QJsonObject{ {"fields", jsonCards} };
    }

    QJsonObject games2JsonMapValue(QList<GameInfo> games){
        QJsonObject jsonGamesFields;
        int gameNumber = 0;
        for (GameInfo gameInfo : games) {
            gameNumber += 1;
            QString first = gameInfo.playerGoFirst ? "player1" : "player2";
            QJsonObject jsonOpponentCards = cards2JsonMapValue(gameInfo.opponentDeck.currentCards());
            QJsonObject jsonGameFields{
                {"fields", QJsonObject{
                        {"first", QJsonObject{
                                {"stringValue", first}
                            }},
                        { "opponentCards", QJsonObject{
                                { "mapValue", jsonOpponentCards}
                            }},
                        {"opponentMulligan", QJsonObject{
                                { "booleanValue", gameInfo.opponentMulligan }}
                        },
                        {"playerMulligan", QJsonObject{
                                { "booleanValue", gameInfo.playerMulligan }}
                        },
                        {"playerWins", QJsonObject{
                                {"booleanValue", gameInfo.playerWins }
                            }}
                    }}
            };
            jsonGamesFields.insert(QString("game%1").arg(gameNumber), QJsonObject{
                                       {"mapValue", jsonGameFields}
                                   });
        }
        return jsonGamesFields;
    }

    QJsonObject player1JsonFields(Deck deck, QString playerRankClass)
    {
        QJsonObject jsonCards = cards2JsonMapValue(deck.cards(true));
        QJsonObject jsonFields{
            {"fields", QJsonObject{
                    { "cards", QJsonObject{
                            { "mapValue",  jsonCards }
                        }},
                    { "colors", QJsonObject{
                            { "stringValue", deck.colorIdentity() }
                        }},
                    {"rank", QJsonObject{
                            { "stringValue", playerRankClass }
                        }}
                }
            }
        };
        return jsonFields;
    }

    QJsonObject player2JsonFields(MatchInfo matchInfo)
    {
        QJsonObject jsonFields{
            {"fields", QJsonObject{
                    { "colors", QJsonObject{
                            { "stringValue", matchInfo.getOpponentDeckColorIdentity() }
                        }},
                    {"rank", QJsonObject{
                            { "stringValue", matchInfo.opponentInfo.opponentRankClass() }
                        }}
                }
            }
        };
        return jsonFields;
    }

};

#endif // RQTUPLOADMATCH_H
