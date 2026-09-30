#include "requesthandler.h"

bool RequestHandler::validateApiKey(const httplib::Request& req, const std::string& expectedKey) {
    auto it = req.headers.find("X-API-Key");
    if (it == req.headers.end()) {
        return false;
    }
    return it->second == expectedKey;
}

void RequestHandler::sendUnauthorized(httplib::Response& res) {
    res.status = 401;
    res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
}

void RequestHandler::sendSuccess(httplib::Response& res, const std::string& message) {
    json response;
    response["success"] = true;
    response["message"] = message;
    res.set_content(response.dump(), "application/json");
}

void RequestHandler::sendError(httplib::Response& res, int statusCode, const std::string& error, const std::string& details) {
    res.status = statusCode;
    json errorResponse;
    errorResponse["error"] = error;
    if (!details.empty()) {
        errorResponse["details"] = details;
    }
    res.set_content(errorResponse.dump(), "application/json");
}

void RequestHandler::sendNotFound(httplib::Response& res, const std::string& entityType) {
    res.status = 404;
    json errorResponse;
    errorResponse["error"] = entityType + " not found";
    res.set_content(errorResponse.dump(), "application/json");
}
