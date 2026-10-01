#include "classhandler.h"
#include "../requesthandler.h"
#include "../jsonconverter.h"
#include "../../shared/entityfinder.h"
#include "../json.hpp"
#include <QJsonDocument>
#include <QJsonObject>

using json = nlohmann::json;

ClassHandler::ClassHandler(Schedule& schedule, IdGenerator& idGen, const AuthMiddleware& auth, const std::string& dataFile)
    : schedule(schedule), idGenerator(idGen), authMiddleware(auth), dataFile(dataFile) {
}

void ClassHandler::handleGetAll(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    sendClassesResponse(res);
}

void ClassHandler::handleCreate(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        RecurringClass newClass = parseClassFromRequest(req.body);
        newClass.setId(idGenerator.generateClassId());

        schedule.addRecurringClass(newClass);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Class added successfully");
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid JSON or data format", e.what());
    }
}

void ClassHandler::handleUpdate(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        int classId = std::stoi(req.matches[1]);

        RecurringClass* cls = EntityFinder::findClassById(schedule, classId);
        if (!cls) {
            RequestHandler::sendNotFound(res, "Class");
            return;
        }

        updateClassFields(*cls, req.body);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Class updated successfully");
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid request", e.what());
    }
}

void ClassHandler::handleDelete(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        int classId = std::stoi(req.matches[1]);

        int index = EntityFinder::findClassIndexById(schedule, classId);
        if (index == -1) {
            RequestHandler::sendNotFound(res, "Class");
            return;
        }

        schedule.removeClassAt(index);
        persistSchedule();

        json response;
        response["success"] = true;
        response["message"] = "Class deleted successfully";
        res.set_content(response.dump(), "application/json");
    } catch (const std::exception& e) {
        res.status = 400;
        json errorResponse;
        errorResponse["error"] = "Invalid request";
        errorResponse["details"] = e.what();
        res.set_content(errorResponse.dump(), "application/json");
    }
}

void ClassHandler::handleCancelOccurrence(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        int classId = std::stoi(req.matches[1]);

        json requestData = json::parse(req.body);
        std::string dateStr = requestData["date"];
        QDate date = QDate::fromString(QString::fromStdString(dateStr), Qt::ISODate);

        RecurringClass* cls = EntityFinder::findClassById(schedule, classId);
        if (!cls) {
            RequestHandler::sendNotFound(res, "Class");
            return;
        }

        cls->cancelOn(date);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Class cancelled on " + dateStr);
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid Request", e.what());
    }
}

RecurringClass ClassHandler::parseClassFromRequest(const std::string& body) {
    json requestData = json::parse(body);
    QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);
    return RecurringClass::fromJson(jsonObj);
}

void ClassHandler::updateClassFields(RecurringClass& cls, const std::string& body) {
    json requestData = json::parse(body);
    QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

    cls.setName(jsonObj["name"].toString());
    cls.setDayOfWeek(jsonObj["dayOfWeek"].toInt());
    cls.setStartTime(QTime::fromString(jsonObj["startTime"].toString(), "HH:mm"));
    cls.setEndTime(QTime::fromString(jsonObj["endTime"].toString(), "HH:mm"));
    cls.setRoom(jsonObj["room"].toString());
    cls.setTeacher(jsonObj["teacher"].toString());
}

void ClassHandler::persistSchedule() {
    schedule.saveToFile(QString::fromStdString(dataFile));
}

void ClassHandler::sendClassesResponse(httplib::Response& res) {
    json classesArray = json::array();
    for (const auto& cls : schedule.getRecurringClasses()) {
        classesArray.push_back(json::parse(QString(QJsonDocument(cls.toJson()).toJson()).toStdString()));
    }
    res.set_content(classesArray.dump(), "application/json");
}
