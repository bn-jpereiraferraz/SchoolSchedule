#ifndef JSONHELPERS_H
#define JSONHELPERS_H

#include <QJsonObject>
#include <QTime>
#include <QDate>

// Forward declarations
class RecurringClass;
class OneTimeEvent;

class JsonHelpers {
public:
    // Time/Date serialization utilities
    static QString timeToString(const QTime &time);
    static QTime stringToTime(const QString &str);
    static QString dateToString(const QDate &date);
    static QDate stringToDate(const QString &str);

    // RecurringClass serialization
    static QJsonObject recurringClassToJson(const RecurringClass &cls);
    static RecurringClass recurringClassFromJson(const QJsonObject &json);

    // OneTimeEvent serialization
    static QJsonObject oneTimeEventToJson(const OneTimeEvent &evt);
    static OneTimeEvent oneTimeEventFromJson(const QJsonObject &json);
};

#endif // JSONHELPERS_H
