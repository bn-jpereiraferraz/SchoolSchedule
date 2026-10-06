#include "displayformatter.h"
#include "constants.h"
#include <QStringList>

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

QString DisplayFormatter::formatClassRich(const RecurringClass& cls){
    QString timeRange = cls.getStartTime().toString(Constants::TIME_FORMAT) + " - " + cls.getEndTime().toString(Constants::TIME_FORMAT);

    //Build formatted string with icon
    QString formatted = QString("%1 %2 | %3").arg(Constants::Icons::CLASS_ICON).arg(timeRange).arg(cls.getName());

    //Add optional Details
    QStringList details;
    if (!cls.getRoom().isEmpty()){
        details.append(QString("%1 %2").arg(Constants::Icons::ROOM_ICON).arg(cls.getRoom()));
    }
    if (!cls.getTeacher().isEmpty()){
        details.append(QString("%1 %2").arg(Constants::Icons::TEACHER_ICON).arg(cls.getTeacher()));
    }
    if (!details.isEmpty()){
        formatted += " (" + details.join(", ") + ")";
    }
    return formatted;
}

QString DisplayFormatter::formatEventRich(const OneTimeEvent& evt){
    QString timeStr = evt.getStartTime().toString(Constants::TIME_FORMAT);
    if (evt.getEndTime().isValid()){
        timeStr += " - " + evt.getEndTime().toString(Constants::TIME_FORMAT);
    }
    //Build formatted string with icon
    QString formatted = QString("%1 %2 | %3").arg(Constants::Icons::EXAM_ICON).arg(timeStr).arg(evt.getName());

    //Add optional details
    QStringList details;
    if (!evt.getLocation().isEmpty()){
        details.append(QString("%1 %2").arg(Constants::Icons::ROOM_ICON).arg(evt.getLocation()));
    }
    if (!evt.getNotes().isEmpty()){
        details.append(evt.getNotes());
    }
    if (!details.isEmpty()){
        formatted += " (" + details.join(", ") + ")";
    }
    return formatted;
}

QString DisplayFormatter::getEventTypeIcon(bool isClass){
    return isClass ? Constants::Icons::CLASS_ICON : Constants::Icons::EXAM_ICON;
}