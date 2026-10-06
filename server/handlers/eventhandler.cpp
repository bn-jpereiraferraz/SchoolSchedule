#include "eventhandler.h"
#include "../requesthandler.h"
#include "../jsonconverter.h"
#include "../../shared/entityfinder.h"
#include "../json.hpp"
#include <QJsonDocument>
#include <QJsonObject>

using json = nlohmann::json;

EventHandler::EventHandler(Schedule& schedule, IdGenerator& idGen, const AuthMiddleware& auth, const std::string& dataFile)
    : schedule(schedule), idGenerator(idGen), authMiddleware(auth), dataFile(dataFile) {
}

void EventHandler::handleGetAll(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    sendEventsResponse(res);
}

void EventHandler::handleCreate(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        OneTimeEvent newEvent = parseEventFromRequest(req.body);
        newEvent.setId(idGenerator.generateEventId());

        schedule.addOneTimeEvent(newEvent);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Event added successfully");
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid JSON or data format", e.what());
    }
}

void EventHandler::handleUpdate(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        int eventId = std::stoi(req.matches[1]);

        OneTimeEvent* evt = EntityFinder::findEventById(schedule, eventId);
        if (!evt) {
            RequestHandler::sendNotFound(res, "Event");
            return;
        }

        updateEventFields(*evt, req.body);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Event updated successfully");
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid request", e.what());
    }
}

void EventHandler::handleDelete(const httplib::Request& req, httplib::Response& res) {
    if (!authMiddleware.validateRequest(req)) {
        authMiddleware.sendUnauthorizedResponse(res);
        return;
    }

    try {
        int eventId = std::stoi(req.matches[1]);

        int index = EntityFinder::findEventIndexById(schedule, eventId);
        if (index == -1) {
            RequestHandler::sendNotFound(res, "Event");
            return;
        }

        schedule.removeEventAt(index);
        persistSchedule();

        RequestHandler::sendSuccess(res, "Event deleted successfully");
    } catch (const std::exception& e) {
        RequestHandler::sendError(res, 400, "Invalid request", e.what());
    }
}

OneTimeEvent EventHandler::parseEventFromRequest(const std::string& body) {
    json requestData = json::parse(body);
    QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);
    return OneTimeEvent::fromJson(jsonObj);
}

void EventHandler::updateEventFields(OneTimeEvent& evt, const std::string& body) {
    json requestData = json::parse(body);
    QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

    evt.setName(jsonObj["name"].toString());
    evt.setDate(QDate::fromString(jsonObj["date"].toString(), Qt::ISODate));
    evt.setStartTime(QTime::fromString(jsonObj["startTime"].toString(), "HH:mm"));
    evt.setEndTime(QTime::fromString(jsonObj["endTime"].toString(), "HH:mm"));
    evt.setLocation(jsonObj["location"].toString());
    evt.setNotes(jsonObj["notes"].toString());
}

void EventHandler::persistSchedule() {
    schedule.saveToFile(QString::fromStdString(dataFile));
}

void EventHandler::sendEventsResponse(httplib::Response& res) {
    json eventsArray = json::array();
    for (const auto& evt : schedule.getOneTimeEvents()) {
        eventsArray.push_back(json::parse(QString(QJsonDocument(evt.toJson()).toJson()).toStdString()));
    }
    res.set_content(eventsArray.dump(), "application/json");
}
