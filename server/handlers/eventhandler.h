#ifndef EVENTHANDLER_H
#define EVENTHANDLER_H

#include "../httplib.h"
#include "../../shared/scheduledata.h"
#include "../idgenerator.h"
#include "authmiddleware.h"
#include <string>

class EventHandler {
public:
    EventHandler(Schedule& schedule, IdGenerator& idGen, const AuthMiddleware& auth, const std::string& dataFile);

    void handleGetAll(const httplib::Request& req, httplib::Response& res);
    void handleCreate(const httplib::Request& req, httplib::Response& res);
    void handleUpdate(const httplib::Request& req, httplib::Response& res);
    void handleDelete(const httplib::Request& req, httplib::Response& res);

private:
    OneTimeEvent parseEventFromRequest(const std::string& body);
    void updateEventFields(OneTimeEvent& evt, const std::string& body);
    void persistSchedule();
    void sendEventsResponse(httplib::Response& res);

    Schedule& schedule;
    IdGenerator& idGenerator;
    const AuthMiddleware& authMiddleware;
    std::string dataFile;
};

#endif // EVENTHANDLER_H
