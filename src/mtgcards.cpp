#include "mtgcards.h"
#include "macros.h"

#include <QByteArray>
#include <QDir>
#include <QFile>
#include <QFileInfo>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonParseError>
#include <QJsonValueRef>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QStandardPaths>

MtgCards::MtgCards(QObject *parent) : QObject(parent)
{
    QString dataDir = QStandardPaths::writableLocation(QStandardPaths::DataLocation);
    setsDir = dataDir + QDir::separator() + "sets";
    if(!QFile::exists(setsDir)) {
      QDir dir;
      dir.mkpath(setsDir);
    }
    loadSet("AKH");
    loadSet("HOU");
    loadSet("XLN");
    loadSet("RIX");
}

void MtgCards::loadSet(QString setCode)
{
    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
	if(QFileInfo(setFile).exists()) {
        loadSetFromFile(setCode + ".json");
	} else {
        downloadSet(setCode);
	}
}

void MtgCards::downloadSet(QString setCode)
{
    QString setUrl = QString("https://mtgjson.com/json/%1.json").arg(setCode);
    QNetworkRequest request(setUrl);
	QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, &MtgCards::downloadSetOnFinish);
    LOGD(QString("Downloading %1 cards from %2").arg(setCode).arg(setUrl));
}

void MtgCards::downloadSetOnFinish()
{
	QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
	QByteArray jsonData = reply->readAll();

	if(reply->error() == QNetworkReply::ContentNotFoundError) {
        QString setUrl = reply->url().toString();
        LOGW(QString("Error while downloading mtg card json: %1").arg(setUrl));
        return;
	}

	QJsonParseError error;
    QJsonObject jsonSet = QJsonDocument::fromJson(jsonData, &error).object();
    QString setCode = jsonSet["code"].toString();
    LOGD(QString("Downloaded %1 bytes from %2 json").arg(jsonData.size()).arg(setCode));

    QFile setFile(setsDir + QDir::separator() + setCode + ".json");
    setFile.open(QIODevice::WriteOnly);
    setFile.write(jsonData);
    setFile.close();

    loadSetFromFile(setCode + ".json");
}

void MtgCards::loadSetFromFile(QString setFileName) {
    LOGD(QString("Loading %1").arg(setFileName));

    QFile setFile(setsDir + QDir::separator() + setFileName);
	if(!QFileInfo(setFile).exists()) {
        LOGW(QString("%1 not found.").arg(setFileName));
		return;
	}

	bool opened = setFile.open(QIODevice::ReadOnly | QIODevice::Text);
	if(!opened) {
        LOGW(QString("Erro while opening %1.").arg(setFileName));
		return;
	}

    QByteArray jsonData = setFile.readAll();
	QJsonParseError error;
    QJsonObject jsonSet = QJsonDocument::fromJson(jsonData, &error).object();
    QString setCode = jsonSet["code"].toString();
    QJsonArray jsonCards = jsonSet["cards"].toArray();

    QList<Card*> setCards;
    for(QJsonValueRef jsonCardRef: jsonCards) {
		QJsonObject jsonCard = jsonCardRef.toObject();
		QString number = jsonCard["number"].toString();
		QString name = jsonCard["name"].toString();

        Card *card = new Card(number, name);
		setCards << card;
	}

    LOGI(QString("%1 set loaded with %2 cards").arg(setCode).arg(setCards.count()));
}
