// School Calendar REST API Server
#include "httplib.h"
#include "json.hpp"
#include "requesthandler.h"
#include "jsonconverter.h"
#include "constants.h"
#include "../shared/scheduledata.h"
#include "../shared/entityfinder.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <iostream>

using json = nlohmann::json;
using namespace ServerConstants;

//Global schedule and data file path
Schedule globalSchedule;
int nextClassId = 1;
int nextEventId = 1;


//Helper function to Convert schedule to JSON
json scheduleToJson(const Schedule& schedule){
    json j;

    json classesArray = json::array();
    for (const auto& cls : schedule.getRecurringClasses()){
        classesArray.push_back(json::parse(QString(QJsonDocument(cls.toJson()).toJson()).toStdString()));
    }
    j["recurringClasses"] = classesArray;

    json eventsArray = json::array();

    for (const auto& evt : schedule.getOneTimeEvents()){
        eventsArray.push_back(json::parse(QString(QJsonDocument(evt.toJson()).toJson()).toStdString()));
    }
    j["oneTimeEvents"] = eventsArray;

    return j;
}

int main() {
    httplib::Server server; // Server instance 

    std::cout << "School Calendar Server starting on http://localhost:8080" << std::endl;

    //Load existing schedule from file
    if (globalSchedule.loadFromFile(QString::fromStdString(DATA_FILE))){
        std::cout << "Loaded existing schedule from " << DATA_FILE << std::endl;
        
        //Find highest existing ID
        for (const auto& cls : globalSchedule.getRecurringClasses()){
            if (cls.getId() >= nextClassId){
                nextClassId = cls.getId() + 1;
            }
        }

        for (const auto& evt : globalSchedule.getOneTimeEvents()){
            if (evt.getId() >= nextEventId){
                nextEventId = evt.getId() + 1;
            }
        }
    }else{
        std::cout << "No existing schedule found, starting fresh" << std::endl;
    }

    //GET /api/classes - Get all recurring classes
    server.Get("/api/classes", [](const httplib::Request& req, httplib::Response& res){

        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        json response = scheduleToJson(globalSchedule);
        res.set_content(response["recurringClasses"].dump(), "application/json");
    });

    //POST /api/classes - Add new recurring classes
    server.Post("/api/classes", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        try{
            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert nlohmann::json to Qt's QJsonObject
            QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

            //Create RecurringClass from JSON
            RecurringClass newClass = RecurringClass::fromJson(jsonObj);

            //Assign new ID
            newClass.setId(nextClassId++);

            //Add to schedule
            globalSchedule.addRecurringClass(newClass);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send success response
            RequestHandler::sendSuccess(res, "Class added successfully");
        }catch (const std::exception& e){
            RequestHandler::sendError(res, 400, "Invalid JSON or data format", e.what());
        }
    });

    //GET /api/events - Get all one-time events
    server.Get("/api/events", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        json response = scheduleToJson(globalSchedule);
        res.set_content(response["oneTimeEvents"].dump(), "application/json");
    });

    //POST /api/events - Add new one-time event
    server.Post("/api/events", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        try{
            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert nlohmann::json to Qt's QJsonObject
            QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

            //Create One Time Event from JSON
            OneTimeEvent newEvent = OneTimeEvent::fromJson(jsonObj);

            //Assign new ID
            newEvent.setId(nextEventId++);

            //Add to schedule
            globalSchedule.addOneTimeEvent(newEvent);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send success response
            RequestHandler::sendSuccess(res, "Event added successfully");
        }catch (const std::exception& e){
            RequestHandler::sendError(res, 400, "Invalid JSON or data format", e.what());
        }
    });

    //POST /api/classes/{id}/cancel - Cancel specific occurrence
    server.Post(R"(/api/classes/(\d+)/cancel)", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }
        try{
            //Get Class ID from URL
            int classId = std::stoi(req.matches[1]);

            //Parse Date from request body
            json requestData = json::parse(req.body);
            std::string dateStr = requestData["date"];
            QDate date = QDate::fromString(QString::fromStdString(dateStr), Qt::ISODate);

            //Find Class by ID
            RecurringClass* cls = EntityFinder::findClassById(globalSchedule, classId);
            if (!cls){
                RequestHandler::sendNotFound(res, "Class");
                return;
            }

            cls->cancelOn(date);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //send success response
            RequestHandler::sendSuccess(res, "Class cancelled on " + dateStr);
        }catch(const std::exception& e){
            RequestHandler::sendError(res, 400, "Invalid Request", e.what());
        }
    });

    server.Delete(R"(/api/classes/(\d+))", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        try{
            //Get Class ID from URL
            int classId = std::stoi(req.matches[1]);

            //Find and Remove class by ID
            int index = EntityFinder::findClassIndexById(globalSchedule, classId);
            if (index == -1){
                RequestHandler::sendNotFound(res, "Class");
                return;
            }

            globalSchedule.removeClassAt(index);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send Success response
            json response;
            response["success"] = true;
            response["message"] = "Class deleted successfully";
            res.set_content(response.dump(), "application/json");
        }catch(const std::exception& e){
            res.status = 400;
            json errorResponse;
            errorResponse["error"] = "Invalid request";
            errorResponse["details"] = e.what();
            res.set_content(errorResponse.dump(), "application/json");
        }
    });

    //DELETE
    server.Delete(R"(/api/events/(\d+))", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }
        try{
            //Get Event ID from URL
            int eventId = std::stoi(req.matches[1]);

            //Find and remove event by ID
            int index = EntityFinder::findEventIndexById(globalSchedule, eventId);
            if (index == -1){
                RequestHandler::sendNotFound(res, "Event");
                return;
            }

            globalSchedule.removeEventAt(index);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send success response
            RequestHandler::sendSuccess(res, "Event deleted successfully");
        }catch(const std::exception& e){
            RequestHandler::sendError(res, 400, "Invalid request", e.what());
        }
    });

    //PUT
    server.Put(R"(/api/classes/(\d+))", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        try{
            //GET CLASS ID FROM URL
            int classId = std::stoi(req.matches[1]);

            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert to QJsonObject
            QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

            //Find Class by ID and update it
            RecurringClass* cls = EntityFinder::findClassById(globalSchedule, classId);
            if (!cls){
                RequestHandler::sendNotFound(res, "Class");
                return;
            }

            //Update fields
            cls->setName(jsonObj["name"].toString());
            cls->setDayOfWeek(jsonObj["dayOfWeek"].toInt());
            cls->setStartTime(QTime::fromString(jsonObj["startTime"].toString(), "HH:mm"));
            cls->setEndTime(QTime::fromString(jsonObj["endTime"].toString(), "HH:mm"));
            cls->setRoom(jsonObj["room"].toString());
            cls->setTeacher(jsonObj["teacher"].toString());

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send succes message
            RequestHandler::sendSuccess(res, "Class updated successfully");
        }catch(const std::exception&  e){
            RequestHandler::sendError(res, 400, "Invalid request", e.what());
        }
    });

    //PUT - EDIT ONE TIME EVENT
    server.Put(R"(/api/events/(\d+))", [](const httplib::Request& req, httplib::Response& res){
        if (!RequestHandler::validateApiKey(req, API_KEY)){
            RequestHandler::sendUnauthorized(res);
            return;
        }

        try{
            //GET Event ID from URL
            int eventId = std::stoi(req.matches[1]);

            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert to QJsonObject
            QJsonObject jsonObj = JsonConverter::toQJsonObject(requestData);

            //Find Event by ID and update it
            OneTimeEvent* evt = EntityFinder::findEventById(globalSchedule, eventId);
            if (!evt){
                RequestHandler::sendNotFound(res, "Event");
                return;
            }

            //Update fields
            evt->setName(jsonObj["name"].toString());
            evt->setDate(QDate::fromString(jsonObj["date"].toString(), Qt::ISODate));
            evt->setStartTime(QTime::fromString(jsonObj["startTime"].toString(), "HH:mm"));
            evt->setEndTime(QTime::fromString(jsonObj["endTime"].toString(), "HH:mm"));
            evt->setLocation(jsonObj["location"].toString());
            evt->setNotes(jsonObj["notes"].toString());

           //Save to file
           globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

           //Send success response
           RequestHandler::sendSuccess(res, "Event updated successfully");
        }catch(std::exception& e){
            RequestHandler::sendError(res, 400, "Invalid request", e.what());
        }
    });

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("School Calendar API Server - Running", CONTENT_TYPE_TEXT.c_str());
    });

    server.listen(SERVER_HOST.c_str(), SERVER_PORT);
    return 0;
}
