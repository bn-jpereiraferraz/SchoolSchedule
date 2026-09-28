// School Calendar REST API Server
#include "httplib.h"
#include "json.hpp"
#include "../shared/scheduledata.h"
#include <QJsonDocument>
#include <QJsonObject>
#include <QByteArray>
#include <iostream>

using json = nlohmann::json;

//Global schedule and data file path
Schedule globalSchedule;
const std::string DATA_FILE = "schedule.json";
const std::string API_KEY = "your-secret-key";

//Helper function for API key checking
bool validateApiKey (const httplib::Request& req){
    auto it = req.headers.find("X-API-Key");
    if (it == req.headers.end()){
        return false;
    }
    return it->second == API_KEY;
}

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
    httplib::Server server;

    std::cout << "School Calendar Server starting on http://localhost:8080" << std::endl;

    //Load existing schedule from file
    if (globalSchedule.loadFromFile(QString::fromStdString(DATA_FILE))){
        std::cout << "Loaded existing schedule from " << DATA_FILE << std::endl;
    }else{
        std::cout << "No existing schedule found, starting fresh" << std::endl;
    }

    //GET /api/classes - Get all recurring classes
    server.Get("/api/classes", [](const httplib::Request& req, httplib::Response& res){

        if (!validateApiKey(req)){
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }

        json response = scheduleToJson(globalSchedule);
        res.set_content(response["recurringClasses"].dump(), "application/json");
    });

    //POST /api/classes - Add new recurring classes
    server.Post("/api/classes", [](const httplib::Request& req, httplib::Response& res){
        if (!validateApiKey(req)){
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }

        try{
            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert nlohmann::json to Qt's QJsonObject
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(requestData.dump()));
            QJsonObject jsonObj = doc.object();

            //Create RecurringClass from JSON
            RecurringClass newClass = RecurringClass::fromJson(jsonObj);

            //Add to schedule
            globalSchedule.addRecurringClass(newClass);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send success response
            json response;
            response["success"] = true;
            response["message"] = "Class added successfully";
            res.set_content(response.dump(), "application/json");
        }catch (const std::exception& e){
            res.status = 400;
            json errorResponse;
            errorResponse["error"] = "Invalid JSON or data format";
            errorResponse["details"] = e.what();
            res.set_content(errorResponse.dump(), "application/json");
        }
    });

    //GET /api/events - Get all one-time events
    server.Get("/api/events", [](const httplib::Request& req, httplib::Response& res){
        if (!validateApiKey(req)){
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }

        json response = scheduleToJson(globalSchedule);
        res.set_content(response["oneTimeEvents"].dump(), "application/json");
    });

    //POST /api/events - Add new one-time event
    server.Post("/api/events", [](const httplib::Request& req, httplib::Response& res){
        if (!validateApiKey(req)){
            res.status = 401;
            res.set_content("{\"error\":\"Unauthorized\"}", "application/json");
            return;
        }

        try{
            //Parse JSON from request body
            json requestData = json::parse(req.body);

            //Convert nlohmann::json to Qt's QJsonObject
            QJsonDocument doc = QJsonDocument::fromJson(QByteArray::fromStdString(requestData.dump()));
            QJsonObject jsonObj = doc.object();

            //Create One Time Event from JSON
            OneTimeEvent newEvent = OneTimeEvent::fromJson(jsonObj);

            //Add to schedule
            globalSchedule.addOneTimeEvent(newEvent);

            //Save to file
            globalSchedule.saveToFile(QString::fromStdString(DATA_FILE));

            //Send success response
            json response;
            response["success"] = true;
            response["message"] = "Event added successfully";
            res.set_content(response.dump(), "application/json");
        }catch (const std::exception& e){
            res.status = 400;
            json errorResponse;
            errorResponse["error"] = "Invalid JSON or data format";
            errorResponse["details"] = e.what();
            res.set_content(errorResponse.dump(), "application/json");
        }
    });

    server.Get("/", [](const httplib::Request&, httplib::Response& res) {
        res.set_content("School Calendar API Server - Running", "text/plain");
    });

    server.listen("127.0.0.1", 8080);
    return 0;
}
