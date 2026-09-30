#include "jsonbuilders.h"
#include "addclassdialog.h"
#include "addeventdialog.h"

QJsonObject JsonBuilders::buildClassJson(const AddClassDialog& dialog) {
    QJsonObject classData;
    classData["name"] = dialog.getClassName();
    classData["dayOfWeek"] = dialog.getDayOfWeek();
    classData["startTime"] = dialog.getStartTime().toString("HH:mm");
    classData["endTime"] = dialog.getEndTime().toString("HH:mm");
    classData["room"] = dialog.getRoom();
    classData["teacher"] = dialog.getTeacher();
    return classData;
}

QJsonObject JsonBuilders::buildEventJson(const AddEventDialog& dialog) {
    QJsonObject eventData;
    eventData["name"] = dialog.getEventName();
    eventData["date"] = dialog.getDate().toString("yyyy-MM-dd");
    eventData["startTime"] = dialog.getStartTime().toString("HH:mm");
    eventData["endTime"] = dialog.getEndTime().toString("HH:mm");
    eventData["location"] = dialog.getLocation();
    eventData["notes"] = dialog.getNotes();
    return eventData;
}
