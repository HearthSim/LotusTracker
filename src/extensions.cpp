#include "extensions.h"
#include "macros.h"

#include <QJsonParseError>

QString Extensions::colorIdentityListToString(QList<QChar> distinctManaSymbols)
{
    if (distinctManaSymbols.contains(QChar('a'))) { return "a"; }
    if (distinctManaSymbols.contains(QChar('c'))) { return "c"; }
    if (distinctManaSymbols.contains(QChar('m'))) { return "m"; }
    QString identity = "";
    if (distinctManaSymbols.contains(QChar('b'))) { identity += "b"; }
    if (distinctManaSymbols.contains(QChar('g'))) { identity += "g"; }
    if (distinctManaSymbols.contains(QChar('r'))) { identity += "r"; }
    if (distinctManaSymbols.contains(QChar('w'))) { identity += "w"; }
    if (distinctManaSymbols.contains(QChar('u'))) { identity += "u"; }
    if (identity == "gr") { identity = "rg"; }
    if (identity == "bu") { identity = "ub"; }
    if (identity == "ru") { identity = "ur"; }
    if (identity == "bw") { identity = "wb"; }
    if (identity == "uw") { identity = "wu"; }
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

QImage Extensions::applyRoundedCorners2Image(QImage image, int cornerRadius)
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

QImage Extensions::toGrayscale(QImage image)
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
