#ifndef JSONCONVERTER_H
#define JSONCONVERTER_H

#include "json.hpp"
#include <QJsonObject>
#include <QJsonDocument>
#include <QByteArray>
#include <QString>

using json = nlohmann::json;

class JsonConverter {
public:
    // Convert nlohmann::json to Qt QJsonObject
    static QJsonObject toQJsonObject(const json& j);

    // Convert Qt QJsonObject to nlohmann::json
    static json fromQJsonObject(const QJsonObject& obj);
};

#endif // JSONCONVERTER_H
