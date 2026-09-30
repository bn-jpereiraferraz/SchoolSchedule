#ifndef JSONBUILDERS_H
#define JSONBUILDERS_H

#include <QJsonObject>

// Forward declarations
class AddClassDialog;
class AddEventDialog;

class JsonBuilders {
public:
    // Build JSON object from AddClassDialog data
    static QJsonObject buildClassJson(const AddClassDialog& dialog);

    // Build JSON object from AddEventDialog data
    static QJsonObject buildEventJson(const AddEventDialog& dialog);
};

#endif // JSONBUILDERS_H
