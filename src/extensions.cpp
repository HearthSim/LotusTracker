#include "extensions.h"
#include "macros.h"

#include <QJsonParseError>

QString Extensions::colorIdentityListToString(QList<QChar> distinctManaSymbols)
{
    QString identity = "";
    if (distinctManaSymbols.contains(QChar('b'))) { identity += "b"; }
    if (distinctManaSymbols.contains(QChar('g'))) { identity += "g"; }
    if (distinctManaSymbols.contains(QChar('r'))) { identity += "r"; }
    if (distinctManaSymbols.contains(QChar('w'))) { identity += "w"; }
    if (distinctManaSymbols.contains(QChar('u'))) { identity += "u"; }
    if (identity == "bgr") { identity = "brg"; }
    if (identity == "grw") { identity = "rgw"; }
    if (identity == "bru") { identity = "ubr"; }
    if (identity == "bwu") { identity = "wub"; }
    return (!identity.isEmpty()) ? identity : "default";
}

QJsonArray Extensions::stringToJsonArray(QString json)
{
    QJsonParseError parseError;
    QJsonArray jsonArray = QJsonDocument::fromJson(json.toUtf8(), &parseError).array();
    if (parseError.error != QJsonParseError::NoError) {
        LOGW(parseError.errorString());
        return QJsonArray();
    } else {
        return jsonArray;       
    }
}

QJsonObject Extensions::stringToJsonObject(QString json)
{
    QJsonParseError parseError;
    QJsonObject jsonObject = QJsonDocument::fromJson(json.toUtf8(), &parseError).object();
    if (parseError.error != QJsonParseError::NoError) {
        LOGW(parseError.errorString());
        return QJsonObject();
    } else {
        return jsonObject;       
    }
}
