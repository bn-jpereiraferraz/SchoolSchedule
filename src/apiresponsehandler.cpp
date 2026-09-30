#include "apiresponsehandler.h"

void ApiResponseHandler::handleResponse(QNetworkReply* reply,
                                       std::function<void()> onSuccess,
                                       std::function<void(const QString&)> onError) {
    reply->deleteLater();

    if (reply->error() == QNetworkReply::NoError) {
        if (onSuccess) {
            onSuccess();
        }
    } else {
        if (onError) {
            QString errorMsg = "Error: " + reply->errorString();
            onError(errorMsg);
        }
    }
}

bool ApiResponseHandler::isSuccess(QNetworkReply* reply) {
    return reply->error() == QNetworkReply::NoError;
}

QString ApiResponseHandler::getErrorMessage(QNetworkReply* reply) {
    return reply->errorString();
}
