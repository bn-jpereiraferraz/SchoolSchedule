#include "schoolcalendarserver.h"
#include "constants.h"
#include <iostream>

using namespace ServerConstants;

SchoolCalendarServer::SchoolCalendarServer(const std::string& host, int port, const std::string& apiKey, const std::string& dataFile)
    : host(host), port(port), apiKey(apiKey), dataFile(dataFile) {

    authMiddleware = std::make_unique<AuthMiddleware>(apiKey);
    classHandler = std::make_unique<ClassHandler>(schedule, idGenerator, *authMiddleware, dataFile);
    eventHandler = std::make_unique<EventHandler>(schedule, idGenerator, *authMiddleware, dataFile);
}

void SchoolCalendarServer::start() {
    std::cout << "School Calendar Server starting on http://" << host << ":" << port << std::endl;

    loadSchedule();
    setupRoutes();
    run();
}

void SchoolCalendarServer::loadSchedule() {
    if (schedule.loadFromFile(QString::fromStdString(dataFile))) {
        std::cout << "Loaded existing schedule from " << dataFile << std::endl;
        idGenerator.initializeFromSchedule(schedule);
    } else {
        std::cout << "No existing schedule found, starting fresh" << std::endl;
    }
}

void SchoolCalendarServer::setupRoutes() {
    server.Get("/api/classes", [this](const httplib::Request& req, httplib::Response& res) {
        classHandler->handleGetAll(req, res);
    });

    server.Post("/api/classes", [this](const httplib::Request& req, httplib::Response& res) {
        classHandler->handleCreate(req, res);
    });

    server.Put(R"(/api/classes/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        classHandler->handleUpdate(req, res);
    });

    server.Delete(R"(/api/classes/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        classHandler->handleDelete(req, res);
    });

    server.Post(R"(/api/classes/(\d+)/cancel)", [this](const httplib::Request& req, httplib::Response& res) {
        classHandler->handleCancelOccurrence(req, res);
    });

    server.Get("/api/events", [this](const httplib::Request& req, httplib::Response& res) {
        eventHandler->handleGetAll(req, res);
    });

    server.Post("/api/events", [this](const httplib::Request& req, httplib::Response& res) {
        eventHandler->handleCreate(req, res);
    });

    server.Put(R"(/api/events/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        eventHandler->handleUpdate(req, res);
    });

    server.Delete(R"(/api/events/(\d+))", [this](const httplib::Request& req, httplib::Response& res) {
        eventHandler->handleDelete(req, res);
    });

    setupHealthCheck();
}

void SchoolCalendarServer::setupHealthCheck() {
    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("School Calendar API Server - Running", CONTENT_TYPE_TEXT.c_str());
    });
}

void SchoolCalendarServer::run() {
    server.listen(host.c_str(), port);
}
