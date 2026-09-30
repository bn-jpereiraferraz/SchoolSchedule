#ifndef SERVER_CONSTANTS_H
#define SERVER_CONSTANTS_H

#include <string>

namespace ServerConstants {
    // Server settings
    const std::string SERVER_HOST = "127.0.0.1";
    const int SERVER_PORT = 8080;

    // API Key
    const std::string API_KEY = "your-secret-key";

    // Data file
    const std::string DATA_FILE = "schedule.json";

    // Content types
    const std::string CONTENT_TYPE_JSON = "application/json";
    const std::string CONTENT_TYPE_TEXT = "text/plain";
}

#endif // SERVER_CONSTANTS_H
