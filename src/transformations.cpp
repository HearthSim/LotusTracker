#include "transformations.h"
#include "macros.h"

#include <QJsonParseError>

QString Transformations::colorIdentityListToString(QList<QChar> distinctManaSymbols)
{
    if (distinctManaSymbols.contains(QChar('a'))) { return "a"; }
    if (distinctManaSymbols.contains(QChar('c'))) { return "c"; }
    if (distinctManaSymbols.contains(QChar('m'))) { return "m"; }
    QString identity = "";
    if (distinctManaSymbols.contains(QChar('b'))) { identity += "b"; }
    if (distinctManaSymbols.contains(QChar('g'))) { identity += "g"; }
    if (distinctManaSymbols.contains(QChar('r'))) { identity += "r"; }
    if (distinctManaSymbols.contains(QChar('u'))) { identity += "u"; }
    if (distinctManaSymbols.contains(QChar('w'))) { identity += "w"; }
    if (identity == "bg") { identity = "bg"; }      // Golgari
    if (identity == "br") { identity = "br"; }      // Rakdos
    if (identity == "gr") { identity = "rg"; }      // Gruul
    if (identity == "bu") { identity = "ub"; }      // Dimir
    if (identity == "gu") { identity = "ug"; }      // Simic
    if (identity == "ru") { identity = "ur"; }      // Izzet
    if (identity == "bw") { identity = "wb"; }      // Orzhov
    if (identity == "gw") { identity = "wg"; }      // Selesnya
    if (identity == "rw") { identity = "wr"; }      // Boros
    if (identity == "uw") { identity = "wu"; }      // Azorius
    if (identity == "bgr") { identity = "brg"; }    // Jund
    if (identity == "bgu") { identity = "ubg"; }    // Sultai
    if (identity == "bru") { identity = "ubr"; }    // Grixis
    if (identity == "gru") { identity = "urg"; }    // Temur
    if (identity == "grw") { identity = "wrg"; }    // Naya
    if (identity == "bgw") { identity = "wbg"; }    // Abzan
    if (identity == "brw") { identity = "wbr"; }    // Mardu
    if (identity == "buw") { identity = "wub"; }    // Esper
    if (identity == "guw") { identity = "wug"; }    // Bant
    if (identity == "ruw") { identity = "wur"; }    // Jeskai
    return (identity.isEmpty()) ? "" : identity;
}

QJsonArray Transformations::stringToJsonArray(QString json)
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

QJsonObject Transformations::stringToJsonObject(QString json)
{
    if (json.isEmpty()) {
        return QJsonObject();
    }
    QJsonParseError parseError;
    QJsonObject jsonObject = QJsonDocument::fromJson(json.toUtf8(), &parseError).object();
    if (parseError.error != QJsonParseError::NoError) {
        LOGW(parseError.errorString());
        LOGW(json);
        return QJsonObject();
    } else {
        return jsonObject;       
    }
}

QImage Transformations::applyRoundedCorners2Image(QImage image, int cornerRadius)
{
    QImage roundedImage(image.width(), image.height(), QImage::Format_ARGB32);
    roundedImage.fill(Qt::transparent);
    QBrush brush(image);
    QPen pen;
    pen.setColor(Qt::darkGray);
    pen.setJoinStyle(Qt::RoundJoin);
    QPainter painter(&roundedImage);
    painter.setBrush(brush);
    painter.setPen(pen);
    painter.drawRoundedRect(0, 0, image.width(), image.height(), cornerRadius, cornerRadius);
    return roundedImage;
}

QImage Transformations::toGrayscale(QImage image)
{
    QImage grayscaleImage = image.convertToFormat(image.hasAlphaChannel() ?
          QImage::Format_ARGB32 : QImage::Format_RGB32);
    unsigned int *data = (unsigned int*)grayscaleImage.bits();
    int pixelCount = grayscaleImage.width() * grayscaleImage.height();
    // Convert each pixel to grayscale
    for(int i = 0; i < pixelCount; ++i) {
        int val = qGray(*data);
        *data = qRgba(val, val, val, qAlpha(*data));
        ++data;
    }
    return grayscaleImage;
}
