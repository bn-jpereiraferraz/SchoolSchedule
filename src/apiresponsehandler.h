#ifndef APIRESPONSEHANDLER_H
#define APIRESPONSEHANDLER_H

#include <QNetworkReply>
#include <QString>
#include <functional>

class ApiResponseHandler {
public:
    // Handle a network reply with success and error callbacks
    // Automatically deletes the reply when done
    static void handleResponse(QNetworkReply* reply,
                              std::function<void()> onSuccess,
                              std::function<void(const QString&)> onError);

    // Check if reply was successful
    static bool isSuccess(QNetworkReply* reply);

    // Get error message from reply
    static QString getErrorMessage(QNetworkReply* reply);
};

#endif // APIRESPONSEHANDLER_H
