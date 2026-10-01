#include "schedulerepository.h"
#include "networkrequestbuilder.h"
#include "jsonbuilders.h"
#include "apiresponsehandler.h"
#include "constants.h"
#include <QJsonDocument>
#include <QJsonArray>

ScheduleRepository::ScheduleRepository(const QString& serverUrl, const QString& apiKey, QObject* parent)
    :QObject(parent), networkManager(new QNetworkAccessManager(this)), schedule(std::make_unique<Schedule>())
    , serverUrl(serverUrl), apiKey(apiKey){}


ScheduleRepository::~ScheduleRepository(){
    //Smart Pointer will automatically clean up schedule
    //networkManager cleans up through Qt parent-child
}

const Schedule* ScheduleRepository::getSchedule()const{
    return schedule.get();  //Always valid, never nullptr
}

void ScheduleRepository::fetchClasses(){
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, Constants::API_CLASSES, apiKey);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onClassesFetched(reply);});
}

void ScheduleRepository::fetchEvents(){
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, Constants::API_EVENTS, apiKey);

    QNetworkReply* reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onEventsFetched(reply);});
}

void ScheduleRepository::addClass(const RecurringClass& cls){
    QJsonObject classData;
    classData["name"] = cls.getName();
    classData["dayOfWeek"] = cls.getDayOfWeek();
    classData["startTime"] = cls.getStartTime().toString(Constants::TIME_FORMAT);
    classData["endTime"] = cls.getEndTime().toString(Constants::TIME_FORMAT);
    classData["room"] = cls.getRoom();
    classData["teacher"] = cls.getTeacher();

    QJsonDocument doc(classData);
    QByteArray jsonData = doc.toJson();

    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, Constants::API_CLASSES, apiKey);

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onClassAdded(reply);});
}

void ScheduleRepository::addEvent(const OneTimeEvent& evt){
    QJsonObject eventData;
    eventData["name"] = evt.getName();
    eventData["date"] = evt.getDate().toString(Constants::DATE_FORMAT);
    eventData["startTime"] = evt.getStartTime().toString(Constants::TIME_FORMAT);
    eventData["endTime"] = evt.getEndTime().toString(Constants::TIME_FORMAT);
    eventData["location"] = evt.getLocation();
    eventData["notes"] = evt.getNotes();

    QJsonDocument doc(eventData);
    QByteArray jsonData = doc.toJson();

    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, Constants::API_EVENTS, apiKey);
    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onEventAdded(reply);});
}

void ScheduleRepository::updateClass(int id, const RecurringClass& cls){
    QJsonObject classData;
    classData["name"] = cls.getName();
    classData["dayOfWeek"] = cls.getDayOfWeek();
    classData["startTime"] = cls.getStartTime().toString(Constants::TIME_FORMAT);
    classData["endTime"] = cls.getEndTime().toString(Constants::TIME_FORMAT);
    classData["room"] = cls.getRoom();
    classData["teacher"] = cls.getTeacher();

    QJsonDocument doc(classData);
    QByteArray jsonData = doc.toJson();

    QString endpoint = QString(Constants::API_CLASSES_ID).arg(id);
    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

    QNetworkReply* reply = networkManager->put(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onClassUpdated(reply);});
}

void ScheduleRepository::updateEvent(int id, const OneTimeEvent& evt){
    QJsonObject eventData;
    eventData["name"] = evt.getName();
    eventData["date"] = evt.getDate().toString(Constants::DATE_FORMAT);
    eventData["startTime"] = evt.getStartTime().toString(Constants::TIME_FORMAT);
    eventData["endTime"] = evt.getEndTime().toString(Constants::TIME_FORMAT);
    eventData["location"] = evt.getLocation();
    eventData["notes"] = evt.getNotes();

    QJsonDocument doc(eventData);
    QByteArray jsonData = doc.toJson();

    QString endpoint = QString(Constants::API_EVENTS_ID).arg(id);
    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

    QNetworkReply* reply = networkManager->put(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onEventUpdated(reply);});
}

void ScheduleRepository::deleteClass(int id){
    QString endpoint = QString(Constants::API_CLASSES_ID).arg(id);
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, endpoint, apiKey);

    QNetworkReply* reply = networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onClassDeleted(reply);});
}

void ScheduleRepository::deleteEvent(int id){
    QString endpoint = QString(Constants::API_EVENTS_ID).arg(id);
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, endpoint, apiKey);

    QNetworkReply* reply = networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onEventDeleted(reply);});
}

void ScheduleRepository::cancelClassSession(int id, const QDate& date){
    QJsonObject requestData;
    requestData["date"] = date.toString(Qt::ISODate);

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QString endpoint = QString(Constants::API_CLASSES_CANCEL).arg(id);
    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

    QNetworkReply* reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){onClassSessionCancelled(reply);});
}

//Private Slot handlers
void ScheduleRepository::onClassesFetched(QNetworkReply* reply){
    reply->deleteLater();
    if (reply->error() != QNetworkReply::NoError){
        emit operationFailed("Fetch Classes", reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonArray classesArray = doc.array();

    //Replace schedule with fresh data
    schedule = std::make_unique<Schedule>();

    for (const QJsonValue& val : classesArray){
        RecurringClass cls = RecurringClass::fromJson(val.toObject());
        schedule->addRecurringClass(cls);
    }

    emit classesLoaded();
}

void ScheduleRepository::onEventsFetched(QNetworkReply* reply){
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError){
        emit operationFailed("Fetch Events", reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonArray eventsArray = doc.array();

    //Clear old events, keep classes
    schedule->clearEvents();

    for(const QJsonValue& val : eventsArray){
        OneTimeEvent evt = OneTimeEvent::fromJson(val.toObject());
        schedule->addOneTimeEvent(evt);
    }

    emit eventsLoaded();
}

void ScheduleRepository::handleModificationResponse(QNetworkReply* reply,
                                                     const QString& operationName,
                                                     void (ScheduleRepository::*fetchMethod)(),
                                                     void (ScheduleRepository::*successSignal)()) {
    ApiResponseHandler::handleResponse(reply, [this, fetchMethod, successSignal](){
            (this->*fetchMethod)();
            emit (this->*successSignal)();
        },
        [this, operationName](const QString& error){
            emit operationFailed(operationName, error);
    });
}

void ScheduleRepository::onClassAdded(QNetworkReply* reply){
    handleModificationResponse(reply, "Add Class", &ScheduleRepository::fetchClasses, &ScheduleRepository::classAdded);
}

void ScheduleRepository::onEventAdded(QNetworkReply* reply){
    handleModificationResponse(reply, "Add Event", &ScheduleRepository::fetchEvents, &ScheduleRepository::eventAdded);
}

void ScheduleRepository::onClassUpdated(QNetworkReply* reply){
    handleModificationResponse(reply, "Update Class", &ScheduleRepository::fetchClasses, &ScheduleRepository::classUpdated);
}

void ScheduleRepository::onEventUpdated(QNetworkReply* reply){
    handleModificationResponse(reply, "Update Event", &ScheduleRepository::fetchEvents, &ScheduleRepository::eventUpdated);
}

void ScheduleRepository::onClassDeleted(QNetworkReply* reply){
    handleModificationResponse(reply, "Delete Class", &ScheduleRepository::fetchClasses, &ScheduleRepository::classDeleted);
}

void ScheduleRepository::onEventDeleted(QNetworkReply* reply){
    handleModificationResponse(reply, "Delete Event", &ScheduleRepository::fetchEvents, &ScheduleRepository::eventDeleted);
}

void ScheduleRepository::onClassSessionCancelled(QNetworkReply* reply){
    handleModificationResponse(reply, "Cancel Session", &ScheduleRepository::fetchClasses, &ScheduleRepository::classSessionCancelled);
}