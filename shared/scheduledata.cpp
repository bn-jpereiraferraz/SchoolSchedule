#include "scheduledata.h"
#include <QJsonDocument>
#include <QJsonArray>
#include <QFile>


//RecurringClass Json serialization
QJsonObject RecurringClass::toJson() const{
    QJsonObject obj;
    obj["name"] = name;
    obj["dayOfWeek"] = dayOfWeek;
    obj["startTime"] = startTime.toString("HH:mm");
    obj["endTime"] = endTime.toString("HH:mm");
    obj["room"] = room;
    obj["teacher"] = teacher;

    QJsonArray cancelledArray;
    for(const QDate &date : cancelledDates){
        cancelledArray.append(date.toString(Qt::ISODate));
    }

    obj["cancelledDates"] = cancelledArray;

    return obj;
}

RecurringClass RecurringClass::fromJson(const QJsonObject &json){
    RecurringClass cls;
    cls.name = json["name"].toString();
    cls.dayOfWeek = json["dayOfWeek"].toInt();
    cls.startTime = QTime::fromString(json["startTime"].toString(), "HH:mm");
    cls.endTime = QTime::fromString(json["endTime"].toString(), "HH:mm");
    cls.room = json["room"].toString();
    cls.teacher = json["teacher"].toString();

    QJsonArray cancelledArray = json["cancelledDates"].toArray();
    for (const QJsonValue &val : cancelledArray){
        QDate date = QDate::fromString(val.toString(), Qt::ISODate);
        if (date.isValid()){
            cls.cancelledDates.insert(date);
        }
    }

    return cls;
}

//OneTimeEvent JSON Serialization
QJsonObject OneTimeEvent::toJson() const{
    QJsonObject obj;
    obj["name"] = name;
    obj["date"] = date.toString(Qt::ISODate);
    obj["startTime"] = startTime.toString("HH:mm");
    obj["endTime"] = endTime.toString("HH:mm");
    obj["location"] = location;
    obj["notes"] = notes;
    return obj;
}

OneTimeEvent OneTimeEvent::fromJson(const QJsonObject &json){
    OneTimeEvent evt;
    evt.name = json["name"].toString();
    evt.date = QDate::fromString(json["date"].toString(), Qt::ISODate);
    evt.startTime = QTime::fromString(json["startTime"].toString(), "HH:mm");
    evt.endTime = QTime::fromString(json["endTime"].toString(), "HH:mm");
    evt.location = json["location"].toString();
    evt.notes = json["notes"].toString();
    return evt;
}

//Schedule Methods
QVector<QString> Schedule::getEventsForDate(const QDate &date) const{
    QVector<QString> events;

    //Add Recurring Classes for this day of week (if not cancelled)
    for (const RecurringClass &cls : recurringClasses){
        if (cls.dayOfWeek == date.dayOfWeek() && !cls.isCancelledOn(date)){
            QString timeRange = cls.startTime.toString("HH:mm") + " - " + cls.endTime.toString("HH:mm");
            QString eventStr = timeRange + " | " + cls.name;
            if (!cls.room.isEmpty()){
                eventStr += " (" + cls.room + ")";
            }
            events.append(eventStr);
        }
    }

    //Add one time events
    for (const OneTimeEvent &evt : oneTimeEvents){
        if (evt.date == date){
            QString timeStr = evt.startTime.toString("HH:mm");
            if (evt.endTime.isValid()){
                timeStr += " - " + evt.endTime.toString("HH:mm");
            }
            QString eventStr = timeStr +" | " + evt.name;
            if (!evt.location.isEmpty()){
                eventStr += " (" + evt.location + ")";
            }
            events.append(eventStr);
        }
    }
    return events;
}

QVector<const RecurringClass*> Schedule::getClassesForDate(const QDate &date) const{
    QVector<const RecurringClass*> classes;
    for (const RecurringClass &cls : recurringClasses){
        if (cls.dayOfWeek == date.dayOfWeek() && !cls.isCancelledOn(date)){
            classes.append(&cls);
        }
    }
    return classes;
}

QVector<const OneTimeEvent*> Schedule::getOneTimeEventsForDate(const QDate &date) const{
    QVector<const OneTimeEvent*> events;
    for (const OneTimeEvent &evt : oneTimeEvents){
        if (evt.date == date){
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
