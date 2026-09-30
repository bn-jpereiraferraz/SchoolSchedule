#include "networkrequestbuilder.h"

QNetworkRequest NetworkRequestBuilder::buildRequest(const QString& serverUrl,
                                                   const QString& endpoint,
                                                   const QString& apiKey) {
    QNetworkRequest request(QUrl(serverUrl + endpoint));
    request.setRawHeader("X-API-Key", apiKey.toUtf8());
    return request;
}

QNetworkRequest NetworkRequestBuilder::buildJsonRequest(const QString& serverUrl,
                                                       const QString& endpoint,
                                                       const QString& apiKey) {
    QNetworkRequest request = buildRequest(serverUrl, endpoint, apiKey);
    request.setHeader(QNetworkRequest::ContentTypeHeader, "application/json");
    return request;
}
