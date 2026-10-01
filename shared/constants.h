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

    //UI Colors - Event Types
    namespace EventColors{
        constexpr const char* CLASS_COLOR = "#2196F3";
        constexpr const char* EXAM_COLOR = "#F44336";
        constexpr const char* EVENT_COLOR = "#4CAF50";
        constexpr const char* CANCELLED_COLOR = "#9E9E9E";

        //Background colors for list items
        constexpr const char* CLASS_BG_LIGHT = "#E3F2FD";   //Light blue
        constexpr const char* EXAM_BG_LIGHT = "#FFEBEE";    //Light red
        constexpr const char* EVENT_BG_LIGHT = "#E8F5E9";   //Light green

        //Calendar background colors
        constexpr const char* CALENDAR_TODAY_BG = "#E3F2FD";    //Light blue for today
        constexpr const char* CALENDAR_WEEKEND_BG = "#F5F5F5";   //Light gray for weekends
        constexpr const char* CALENDAR_PAST_BG = "#FAFAFA";      //Very light gray for past dates

        //Text Colors
        constexpr const char* PRIMARY_TEXT = "#212121";
        constexpr const char* SECONDARY_TEXT = "#757575";
        constexpr const char* DISABLED_TEXT = "#BDBDBD";
    }

    //UI Icons - UniCode
    namespace Icons{
        constexpr const char* CLASS_ICON = "📚";
        constexpr const char* EXAM_ICON = "📝";
        constexpr const char* EVENT_ICON = "📅";
        constexpr const char* ROOM_ICON = "📍";
        constexpr const char* TEACHER_ICON = "👤";
        constexpr const char* TIME_ICON = "🕒";
        constexpr const char* CANCELLED_ICON = "❌";
    }
}

#endif // CONSTANTS_H
