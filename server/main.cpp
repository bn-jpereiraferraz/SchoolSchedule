// School Calendar REST API Server
#include "httplib.h"
#include "json.hpp"
#include "../shared/scheduledata.h"
#include <iostream>

using json = nlohmann::json;

int main() {
    httplib::Server server;

    std::cout << "School Calendar Server starting on http://localhost:8080" << std::endl;

    // TODO: Implement API endpoints

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("School Calendar API Server - Running", "text/plain");
    });

    server.listen("127.0.0.1", 8080);
    return 0;
}
