#ifndef CLASSHANDLER_H
#define CLASSHANDLER_H

#include "../httplib.h"
#include "../../shared/scheduledata.h"
#include "../idgenerator.h"
#include "authmiddleware.h"
#include <string>

class ClassHandler {
public:
    ClassHandler(Schedule& schedule, IdGenerator& idGen, const AuthMiddleware& auth, const std::string& dataFile);

    void handleGetAll(const httplib::Request& req, httplib::Response& res);
    void handleCreate(const httplib::Request& req, httplib::Response& res);
    void handleUpdate(const httplib::Request& req, httplib::Response& res);
    void handleDelete(const httplib::Request& req, httplib::Response& res);
    void handleCancelOccurrence(const httplib::Request& req, httplib::Response& res);

private:
    RecurringClass parseClassFromRequest(const std::string& body);
    void updateClassFields(RecurringClass& cls, const std::string& body);
    void persistSchedule();
    void sendClassesResponse(httplib::Response& res);

    Schedule& schedule;
    IdGenerator& idGenerator;
    const AuthMiddleware& authMiddleware;
    std::string dataFile;
};

#endif // CLASSHANDLER_H
