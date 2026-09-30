#ifndef REQUESTHANDLER_H
#define REQUESTHANDLER_H

#include "httplib.h"
#include "json.hpp"
#include <string>

using json = nlohmann::json;

class RequestHandler {
public:
    // Validate API key in request headers
    static bool validateApiKey(const httplib::Request& req, const std::string& expectedKey);

    // Send unauthorized response
    static void sendUnauthorized(httplib::Response& res);

    // Send success response
    static void sendSuccess(httplib::Response& res, const std::string& message);

    // Send error response
    static void sendError(httplib::Response& res, int statusCode, const std::string& error, const std::string& details = "");

    // Send not found response
    static void sendNotFound(httplib::Response& res, const std::string& entityType);
};

#endif // REQUESTHANDLER_H
