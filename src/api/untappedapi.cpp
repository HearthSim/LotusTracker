#include "untappedapi.h"
#include "../macros.h"
#include "../transformations.h"
#include "../urls.h"

#include <QJsonDocument>
#include <QJsonObject>
#include <QNetworkRequest>
#include <QNetworkReply>
#include <QUrl>

#define API_BASE_URL "https://api.mtga.untapped.gg/api/v1"
#define CLIENT_ID "7bIfoHez8t1tKPVurVpVbT6QfA9muN8PVgcctp02"
#define HEADER_AUTHORIZATION "Authorization"
#define S3_API "https://f4o4hys026.execute-api.us-west-2.amazonaws.com/live"

UntappedAPI::UntappedAPI(QObject *parent): QObject(parent),
    untappedUploadData(UntappedUploadData("", "", QByteArray()))
{

}

UntappedAPI::~UntappedAPI()
{

}

void UntappedAPI::fetchAnonymousUploadToken()
{
    QJsonObject jsonObj;
    jsonObj.insert("client_id", QJsonValue(CLIENT_ID));
    QByteArray body = QJsonDocument(jsonObj).toJson();

    QUrl url(QString("%1/users").arg(API_BASE_URL));
    if (LOG_REQUEST_ENABLED) {
        LOGD(QString("Request: %1").arg(url.toString()));
    }
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.post(request, body);
    connect(reply, &QNetworkReply::finished,
            this, &UntappedAPI::anonymousUploadTokenOnFinish);
}

void UntappedAPI::requestS3PutUrl()
{
    requestRetries = 0;
    requestS3PutUrlWithRetry();
}

void UntappedAPI::requestS3PutUrlWithRetry()
{
    if (requestRetries > 0) {
        LOGD(QString("S3PutUrl Retry: %1").arg(requestRetries))
    }
    QUrl url(S3_API);
    QNetworkRequest request(url);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    QNetworkReply *reply = networkManager.get(request);
    connect(reply, &QNetworkReply::finished, this, [&](){
        QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
        QString rsp = reply->readAll();
        if (LOG_REQUEST_ENABLED) {
            LOGD(rsp);
        }
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode < 200 || statusCode > 299) {
            if (requestRetries < 3) {
                requestRetries++;
                requestS3PutUrlWithRetry();
                return;
            }
            QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
            LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
            influx_metric(influxdb_cpp::builder()
                .meas("lt_untapped_s3_puturl_failed")
                .tag("status", QString("%1").arg(statusCode).toStdString())
                .tag("reason", reason.toStdString())
                .field("matchId", untappedUploadData.matchId.toStdString())
                .field("count", 1)
            );
            return;
        }

        QJsonObject jsonRsp = Transformations::stringToJsonObject(rsp);
        LOGI(QString("Received shortid: %1").arg( jsonRsp["shortid"].toString()));
        QString putUrl = jsonRsp["put_url"].toString();
        QString timestamp = reply->rawHeader("date");
        emit sgnS3PutInfo(putUrl, timestamp);
    });
}

void UntappedAPI::uploadMatch(UntappedUploadData untappedUploadData)
{
    this->untappedUploadData = untappedUploadData;
    requestRetries = 0;
    uploadMatchWithRetry();
}

void UntappedAPI::uploadMatchWithRetry()
{
    QNetworkRequest request(untappedUploadData.s3PutUrl);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "text/plain");
    QNetworkReply *reply = networkManager.put(request, untappedUploadData.matchData);
    connect(reply, &QNetworkReply::finished, this, [&](){
        QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
        QString rsp = reply->readAll();
        int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
        if (statusCode < 200 || statusCode > 299) {
            if (requestRetries < 3) {
                requestRetries++;
                uploadMatchWithRetry();
                return;
            }
            QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
            LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
            influx_metric(influxdb_cpp::builder()
                .meas("lt_untapped_log_upload_failed")
                .tag("status", QString("%1").arg(statusCode).toStdString())
                .tag("reason", reason.toStdString())
                .field("matchId", untappedUploadData.matchId.toStdString())
                .field("count", 1)
            );
            return;
        }

        LOGI("Match upload success to Untapped.gg");
        influx_metric(influxdb_cpp::builder()
            .meas("lt_untapped_log_upload_success")
            .field("matchId", untappedUploadData.matchId.toStdString())
            .field("count", 1)
        );
    });
}

void UntappedAPI::anonymousUploadTokenOnFinish()
{
    QNetworkReply *reply = static_cast<QNetworkReply*>(sender());
    QString rsp = reply->readAll();
    if (LOG_REQUEST_ENABLED) {
        LOGD(rsp);
    }

    int statusCode = reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
    if (statusCode < 200 || statusCode > 299) {
        QString reason = reply->attribute(QNetworkRequest::HttpReasonPhraseAttribute).toString();
        LOGW(QString("Error: %1 - %2").arg(reply->errorString()).arg(reason));
        return;
    }

    emit sgnNewAnonymousUploadToken(rsp.replace("\"", ""));
}
