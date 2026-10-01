#ifndef SCHOOLCALENDARSERVER_H
#define SCHOOLCALENDARSERVER_H

#include "httplib.h"
#include "../shared/scheduledata.h"
#include "idgenerator.h"
#include "handlers/authmiddleware.h"
#include "handlers/classhandler.h"
#include "handlers/eventhandler.h"
#include <string>
#include <memory>

class SchoolCalendarServer {
public:
    SchoolCalendarServer(const std::string& host, int port, const std::string& apiKey, const std::string& dataFile);

    void start();

private:
    void loadSchedule();
    void setupRoutes();
    void setupHealthCheck();
    void run();

    httplib::Server server;
    Schedule schedule;
    IdGenerator idGenerator;
    std::unique_ptr<AuthMiddleware> authMiddleware;
    std::unique_ptr<ClassHandler> classHandler;
    std::unique_ptr<EventHandler> eventHandler;

    std::string host;
    int port;
    std::string apiKey;
    std::string dataFile;
};

#endif // SCHOOLCALENDARSERVER_H
