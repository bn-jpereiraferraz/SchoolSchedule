#ifndef CONSTANTS_H
#define CONSTANTS_H

#include <QString>

namespace Constants {
    // Network endpoints
    const QString API_CLASSES = "/api/classes";
    const QString API_EVENTS = "/api/events";
    const QString API_CLASSES_CANCEL = "/api/classes/%1/cancel";
    const QString API_CLASSES_ID = "/api/classes/%1";
    const QString API_EVENTS_ID = "/api/events/%1";

    // Time formats
    const QString TIME_FORMAT = "HH:mm";
    const QString DATE_FORMAT = "yyyy-MM-dd";

    // Content types
    const QString CONTENT_TYPE_JSON = "application/json";

    // Default server settings
    const QString DEFAULT_SERVER_URL = "http://localhost:8080";
    const QString DEFAULT_API_KEY = "your-secret-key";
    const int DEFAULT_SERVER_PORT = 8080;
}

#endif // CONSTANTS_H
