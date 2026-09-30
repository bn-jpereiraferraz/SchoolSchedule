#include "jsonhelpers.h"
#include "scheduledata.h"
#include <QJsonArray>

// Time/Date utilities
QString JsonHelpers::timeToString(const QTime &time) {
    return time.toString("HH:mm");
}

QTime JsonHelpers::stringToTime(const QString &str) {
    return QTime::fromString(str, "HH:mm");
}

QString JsonHelpers::dateToString(const QDate &date) {
    return date.toString(Qt::ISODate);
}

QDate JsonHelpers::stringToDate(const QString &str) {
    return QDate::fromString(str, Qt::ISODate);
}

// RecurringClass serialization
QJsonObject JsonHelpers::recurringClassToJson(const RecurringClass &cls) {
    QJsonObject obj;
    obj["id"] = cls.getId();
    obj["name"] = cls.getName();
    obj["dayOfWeek"] = cls.getDayOfWeek();
    obj["startTime"] = timeToString(cls.getStartTime());
    obj["endTime"] = timeToString(cls.getEndTime());
    obj["room"] = cls.getRoom();
    obj["teacher"] = cls.getTeacher();

    QJsonArray cancelledArray;
    for (const QDate &date : cls.getCancelledDates()) {
        cancelledArray.append(dateToString(date));
    }
    obj["cancelledDates"] = cancelledArray;

    return obj;
}

RecurringClass JsonHelpers::recurringClassFromJson(const QJsonObject &json) {
    RecurringClass cls;
    cls.setId(json["id"].toInt(-1));
    cls.setName(json["name"].toString());
    cls.setDayOfWeek(json["dayOfWeek"].toInt());
    cls.setStartTime(stringToTime(json["startTime"].toString()));
    cls.setEndTime(stringToTime(json["endTime"].toString()));
    cls.setRoom(json["room"].toString());
    cls.setTeacher(json["teacher"].toString());

    QJsonArray cancelledArray = json["cancelledDates"].toArray();
    for (const QJsonValue &val : cancelledArray) {
        QDate date = stringToDate(val.toString());
        if (date.isValid()) {
            cls.cancelOn(date);
        }
    }

    return cls;
}

// OneTimeEvent serialization
QJsonObject JsonHelpers::oneTimeEventToJson(const OneTimeEvent &evt) {
    QJsonObject obj;
    obj["id"] = evt.getId();
    obj["name"] = evt.getName();
    obj["date"] = dateToString(evt.getDate());
    obj["startTime"] = timeToString(evt.getStartTime());
    obj["endTime"] = timeToString(evt.getEndTime());
    obj["location"] = evt.getLocation();
    obj["notes"] = evt.getNotes();
    return obj;
}

OneTimeEvent JsonHelpers::oneTimeEventFromJson(const QJsonObject &json) {
    OneTimeEvent evt;
    evt.setId(json["id"].toInt(-1));
    evt.setName(json["name"].toString());
    evt.setDate(stringToDate(json["date"].toString()));
    evt.setStartTime(stringToTime(json["startTime"].toString()));
    evt.setEndTime(stringToTime(json["endTime"].toString()));
    evt.setLocation(json["location"].toString());
    evt.setNotes(json["notes"].toString());
    return evt;
}
