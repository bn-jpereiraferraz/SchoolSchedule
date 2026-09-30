#ifndef DISPLAYFORMATTER_H
#define DISPLAYFORMATTER_H

#include <QString>
#include "scheduledata.h"

class DisplayFormatter {
public:
    // Format a recurring class for display in the event list
    static QString formatClass(const RecurringClass& cls);
    // Format a one-time event for display in the event list
    static QString formatEvent(const OneTimeEvent& evt);

    //Format with Icons and Rich text
    static QString formatClassRich(const RecurringClass& cls);
    static QString formatEventRich(const OneTimeEvent& evt);

    //Helper function to get icon for event type
    static QString getEventTypeIcon(bool isClass);
};

#endif // DISPLAYFORMATTER_H
