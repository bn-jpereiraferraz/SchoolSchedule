#ifndef NETWORKREQUESTBUILDER_H
#define NETWORKREQUESTBUILDER_H

#include <QNetworkRequest>
#include <QString>
#include <QUrl>

class NetworkRequestBuilder {
public:
    // Build a basic request with API key
    static QNetworkRequest buildRequest(const QString& serverUrl,
                                       const QString& endpoint,
                                       const QString& apiKey);

    // Build a request for JSON content (includes Content-Type header)
    static QNetworkRequest buildJsonRequest(const QString& serverUrl,
                                           const QString& endpoint,
                                           const QString& apiKey);
};

#endif // NETWORKREQUESTBUILDER_H
