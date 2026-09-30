#include "displayformatter.h"

QString DisplayFormatter::formatClass(const RecurringClass& cls) {
    QString timeRange = cls.getStartTime().toString("HH:mm") + " - " +
                       cls.getEndTime().toString("HH:mm");
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

    return eventStr;
}

QString DisplayFormatter::formatEvent(const OneTimeEvent& evt) {
    QString timeStr = evt.getStartTime().toString("HH:mm");
    if (evt.getEndTime().isValid()) {
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

    return eventStr;
}
