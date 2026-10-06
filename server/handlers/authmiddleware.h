#ifndef AUTHMIDDLEWARE_H
#define AUTHMIDDLEWARE_H

#include "../httplib.h"
#include <string>

class AuthMiddleware {
public:
    explicit AuthMiddleware(const std::string& apiKey);

    bool validateRequest(const httplib::Request& req) const;
    void sendUnauthorizedResponse(httplib::Response& res) const;

private:
    std::string apiKey;
};

#endif // AUTHMIDDLEWARE_H
