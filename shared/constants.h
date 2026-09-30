#ifndef CONSTANTS_H
#define CONSTANTS_H

namespace Constants {
    // Network endpoints - using literal strings to avoid static initialization
    constexpr const char* API_CLASSES = "/api/classes";
    constexpr const char* API_EVENTS = "/api/events";
    constexpr const char* API_CLASSES_CANCEL = "/api/classes/%1/cancel";
    constexpr const char* API_CLASSES_ID = "/api/classes/%1";
    constexpr const char* API_EVENTS_ID = "/api/events/%1";

    // Time formats
    constexpr const char* TIME_FORMAT = "HH:mm";
    constexpr const char* DATE_FORMAT = "yyyy-MM-dd";

    // Content types
    constexpr const char* CONTENT_TYPE_JSON = "application/json";

    // Default server settings
    constexpr const char* DEFAULT_SERVER_URL = "http://localhost:8080";
    constexpr const char* DEFAULT_API_KEY = "your-secret-key";
    constexpr int DEFAULT_SERVER_PORT = 8080;

    // UI Settings
    constexpr int DEFAULT_WINDOW_WIDTH = 900;
    constexpr int DEFAULT_WINDOW_HEIGHT = 600;
    constexpr int SERVER_SHUTDOWN_TIMEOUT_MS = 3000;
}

#endif // CONSTANTS_H
