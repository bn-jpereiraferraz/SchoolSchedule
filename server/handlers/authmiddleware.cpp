#include "authmiddleware.h"
#include "../requesthandler.h"

AuthMiddleware::AuthMiddleware(const std::string& apiKey)
    : apiKey(apiKey) {
}

bool AuthMiddleware::validateRequest(const httplib::Request& req) const {
    return RequestHandler::validateApiKey(req, apiKey);
}

void AuthMiddleware::sendUnauthorizedResponse(httplib::Response& res) const {
    RequestHandler::sendUnauthorized(res);
}
