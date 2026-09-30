#include "scheduledata.h"
#include "jsonhelpers.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>


//RecurringClass Json serialization (delegates to JsonHelpers)
QJsonObject RecurringClass::toJson() const{
    return JsonHelpers::recurringClassToJson(*this);
}

RecurringClass RecurringClass::fromJson(const QJsonObject &json){
    return JsonHelpers::recurringClassFromJson(json);
}

//OneTimeEvent JSON Serialization (delegates to JsonHelpers)
QJsonObject OneTimeEvent::toJson() const{
    return JsonHelpers::oneTimeEventToJson(*this);
}

OneTimeEvent OneTimeEvent::fromJson(const QJsonObject &json){
    return JsonHelpers::oneTimeEventFromJson(json);
}

//Schedule Methods
QVector<QString> Schedule::getEventsForDate(const QDate &date) const{
    QVector<QString> events;

    //Add Recurring Classes for this day of week (if not cancelled)
    for (const RecurringClass &cls : recurringClasses){
        if (cls.getDayOfWeek() == date.dayOfWeek() && !cls.isCancelledOn(date)){
            QString timeRange = cls.getStartTime().toString("HH:mm") + " - " + cls.getEndTime().toString("HH:mm");
            QString eventStr = timeRange + " | " + cls.getName();

            QStringList details;
            if (!cls.getRoom().isEmpty()) {
                details.append(cls.getRoom());
            }
            if (!cls.getTeacher().isEmpty()) {
                details.append(cls.getTeacher());
            }
            if (!details.isEmpty()) {
                eventStr += " (" + details.join(", ") + ")";
            }

            events.append(eventStr);
        }
    }

    //Add one time events
    for (const OneTimeEvent &evt : oneTimeEvents){
        if (evt.getDate() == date){
            QString timeStr = evt.getStartTime().toString("HH:mm");
            if (evt.getEndTime().isValid()){
                timeStr += " - " + evt.getEndTime().toString("HH:mm");
            }
            QString eventStr = timeStr + " | " + evt.getName();

            QStringList details;
            if (!evt.getLocation().isEmpty()) {
                details.append(evt.getLocation());
            }
            if (!evt.getNotes().isEmpty()) {
                details.append(evt.getNotes());
            }
            if (!details.isEmpty()) {
                eventStr += " (" + details.join(", ") + ")";
            }

            events.append(eventStr);
        }
    }
    return events;
}

QVector<const RecurringClass*> Schedule::getClassesForDate(const QDate &date) const{
    QVector<const RecurringClass*> classes;
    for (const RecurringClass &cls : recurringClasses){
        if (cls.getDayOfWeek() == date.dayOfWeek() && !cls.isCancelledOn(date)){
            classes.append(&cls);
        }
    }
    return classes;
}

QVector<const OneTimeEvent*> Schedule::getOneTimeEventsForDate(const QDate &date) const{
    QVector<const OneTimeEvent*> events;
    for (const OneTimeEvent &evt : oneTimeEvents){
        if (evt.getDate() == date){
            events.append(&evt);
        }
    }
    return events;
}

bool Schedule::saveToFile(const QString &filename) const{
    QJsonObject root;

    //Save Recurring classes
    QJsonArray classesArray;
    for (const RecurringClass &cls : recurringClasses){
        classesArray.append(cls.toJson());
    }
    root ["recurringClasses"] = classesArray;

    //Save one-time Events
    QJsonArray eventsArray;
    for (const OneTimeEvent &evt : oneTimeEvents){
        eventsArray.append(evt.toJson());
    }
    root ["oneTimeEvents"]  = eventsArray;

    QJsonDocument doc(root);

    QFile file(filename);
    if (!file.open(QIODevice::WriteOnly)){
        return false;
    }

    file.write(doc.toJson());
    file.close();
    return true;
}

bool Schedule::loadFromFile(const QString &filename){
    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly)){
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (!doc.isObject()){
        return false;
    }

    QJsonObject root = doc.object();

    //Load Recurring classes
    recurringClasses.clear();
    QJsonArray classesArray = root ["recurringClasses"].toArray();

    for (const QJsonValue &val : classesArray){
        recurringClasses.append(RecurringClass::fromJson(val.toObject()));
    }

    //Load One-Time Events
    oneTimeEvents.clear();
    QJsonArray eventsArray = root["oneTimeEvents"].toArray();
    for (const QJsonValue &val : eventsArray){
        oneTimeEvents.append(OneTimeEvent::fromJson(val.toObject()));
    } 

    return true;
}
