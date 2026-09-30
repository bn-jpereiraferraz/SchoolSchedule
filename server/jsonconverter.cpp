#include "jsonconverter.h"

QJsonObject JsonConverter::toQJsonObject(const json& j) {
    QJsonDocument doc = QJsonDocument::fromJson(
        QByteArray::fromStdString(j.dump())
    );
    return doc.object();
}

json JsonConverter::fromQJsonObject(const QJsonObject& obj) {
    QJsonDocument doc(obj);
    return json::parse(QString(doc.toJson()).toStdString());
}
