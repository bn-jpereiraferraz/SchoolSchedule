#include "caldavtester.h"
#include <QNetworkRequest>
#include <QUrl>
#include <QDebug>

CalDAVTester::CalDAVTester(QObject* parent)
    : QObject(parent), networkManager(new QNetworkAccessManager(this)){    
    }

CalDAVTester::~CalDAVTester(){
}

void CalDAVTester::testConnection(const QString& appleId, const QString& appPassword){
    this->appleId = appleId;
    this->appPassword = appPassword;
    emit testStarted();
    sendPropfindRequest();
}

//Authentication
QString CalDAVTester::buildBasicAuthHeader()const{
    QString credentials = appleId + ":" + appPassword;
    QString encoded = credentials.toUtf8().toBase64();
    QString header = "Basic " + encoded;
    qDebug() << "Auth header length:" << header.length();
    qDebug() << "Username:" << appleId;
    qDebug() << "Password length:" << appPassword.length();
    return header;
}

//Request Building
QNetworkRequest CalDAVTester::buildAuthenticatedRequest(const QUrl& url){
    QNetworkRequest request(url);
    request.setRawHeader("Authorization", buildBasicAuthHeader().toUtf8());
    return request;
}

QByteArray CalDAVTester::buildPropfindXml(){
    return "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
           "<d:propfind xmlns:d=\"DAV:\">"
           " <d:prop>"
           "   <d:current-user-principal/>"
           " </d:prop>"
           "</d:propfind>";
}

QByteArray CalDAVTester::buildCalendarDiscoveryXml(){
    return "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
           "<d:propfind xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">"
           "  <d:prop>"
           "    <d:resourcetype/>"
           "    <d:displayname/>"
           "    <c:calendar-home-set/>"
           "  </d:prop>"
           "</d:propfind>";
}

QByteArray CalDAVTester::buildTestEventIcal(){
    return "BEGIN:VCALENDAR\r\n"
            "VERSION:2.0\r\n"
            "PRODID:-//School Calendar Test//EN\r\n"
            "BEGIN:VEVENT\r\n"
            "UID:test-event-12345@schoolcalendar\r\n"
            "DTSTAMP:20261006T120000Z\r\n"
            "DTSTART:20261006T140000Z\r\n"
            "DTEND:20261006T150000Z\r\n"
            "SUMMARY:CalDAV Test Event\r\n"
            "DESCRIPTION:Test from School Calendar\r\n"
            "END:VEVENT\r\n"
            "END:VCALENDAR\r\n";
}

QByteArray CalDAVTester::buildCalendarQueryXml(){
    return "<?xml version=\"1.0\" encoding=\"utf-8\"?>"
            "<c:calendar-query xmlns:d=\"DAV:\" xmlns:c=\"urn:ietf:params:xml:ns:caldav\">"
            "  <d:prop>"
            "    <d:getetag/>"
            "    <c:calendar-data/>"
            "  </d:prop>"
            "  <c:filter>"
            "   <c:comp-filter name=\"VCALENDAR\">"
            "    <c:comp-filter name=\"VEVENT\"/>"
            "  </c:comp-filter>"
            " </c:filter>"
            "</c:calendar-query>";
}

QString CalDAVTester::buildTestEventUrl(){
    return "https://caldav.icloud.com" + calendarUrl + "test-event-12345.ics";
}

//Response parsing
QString CalDAVTester::extractPrincipalUrl(const QString& xmlResponse){
    int principalStart = xmlResponse.indexOf("/principal/");
    if (principalStart == -1) return QString();

    int userIdStart = xmlResponse.lastIndexOf('/', principalStart - 1);
    if (userIdStart == -1) return QString();

    return xmlResponse.mid(userIdStart, principalStart - userIdStart + 11);
}

QString CalDAVTester::buildCalendarHomeUrl(const QString& principal){
    return principal.left(principal.indexOf("/principal/")) + "/calendars/";
}

QString CalDAVTester::extractFirstCalendarUrl(const QString& xmlResponse){
    // Look for href elements containing calendar URLs
    // Pattern: <href>/8013622098/calendars/CALENDAR-UUID/</href>

    int hrefStart = xmlResponse.indexOf("<href>");
    while (hrefStart != -1) {
        int hrefEnd = xmlResponse.indexOf("</href>", hrefStart);
        if (hrefEnd == -1) break;

        // Extract URL between tags
        QString url = xmlResponse.mid(hrefStart + 6, hrefEnd - hrefStart - 6);

        // Check if it's a calendar URL (contains /calendars/ and ends with /)
        if (url.contains("/calendars/") && url.endsWith('/') && url.count('/') >= 4) {
            qDebug() << "Found calendar URL:" << url;
            return url;
        }

        // Try next href
        hrefStart = xmlResponse.indexOf("<href>", hrefEnd);
    }

    qDebug() << "No valid calendar URL found in response";
    return QString();
}

//Network Operations
void CalDAVTester::sendPropfindRequest(){
    QUrl url("https://caldav.icloud.com/");

    QNetworkRequest request = buildAuthenticatedRequest(url);
    request.setRawHeader("Depth", "0");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    qDebug() << "Sending PROPFIND to:" << url.toString();
    qDebug() << "Request headers:";
    for (const auto& header : request.rawHeaderList()) {
        if (header == "Authorization") {
            qDebug() << "  " << header << ": [REDACTED]";
        } else {
            qDebug() << "  " << header << ":" << request.rawHeader(header);
        }
    }

    QByteArray body = buildPropfindXml();
    QNetworkReply* reply = networkManager->sendCustomRequest(request, "PROPFIND", body);
    connect(reply, &QNetworkReply::finished, this, &CalDAVTester::onAuthenticationReply);
}

void CalDAVTester::discoverCalendars(){
    QString url = "https://caldav.icloud.com" + calendarHomeUrl;
    qDebug() << "Discovering calendars at:" << url;

    QNetworkRequest request = buildAuthenticatedRequest(QUrl(url));
    request.setRawHeader("Depth", "1");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    QByteArray body = buildCalendarDiscoveryXml();
    QNetworkReply* reply = networkManager->sendCustomRequest(request, "PROPFIND", body);
    connect(reply, &QNetworkReply::finished, this, &CalDAVTester::onCalendarDiscoveryReply);
}

void CalDAVTester::sendPutRequest(const QString& url, const QByteArray& data){
    QNetworkRequest request = buildAuthenticatedRequest(QUrl(url));
    request.setRawHeader("Content-Type", "text/calendar; charset=utf-8");
    request.setRawHeader("If-None-Match", "*");

    QNetworkReply* reply = networkManager->put(request, data);
    connect(reply, &QNetworkReply::finished, this, &CalDAVTester::onCreateEventReply);
}

void CalDAVTester::sendReportRequest(const QString& url, const QByteArray& query){
    QNetworkRequest request = buildAuthenticatedRequest(QUrl(url));
    request.setRawHeader("Depth", "1");
    request.setRawHeader("Content-Type", "application/xml; charset=utf-8");

    QNetworkReply* reply = networkManager->sendCustomRequest(request, "REPORT", query);
    connect (reply, &QNetworkReply::finished, this, &CalDAVTester::onFetchEventsReply);
}

//Error Handling
bool CalDAVTester::hasNetworkError(QNetworkReply* reply){
    return reply->error() != QNetworkReply::NoError;
}

void CalDAVTester::handleNetworkError(QNetworkReply* reply, const QString& operation){
    QString errorMsg = operation + " failed: " + reply->errorString();
    qDebug() << errorMsg;
    qDebug() << "Status:" << reply->attribute(QNetworkRequest::HttpStatusCodeAttribute).toInt();
}

//Orcestrator Methods
void CalDAVTester::onAuthenticationReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (hasNetworkError(reply)){
        handleNetworkError(reply, "Authentication");
        emit authenticationFailed(reply->errorString());
        emit testCompleted(false);
        reply->deleteLater();
        return;
    }

    QString response = QString::fromUtf8(reply->readAll());
    principalUrl = extractPrincipalUrl(response);

    if (principalUrl.isEmpty()){
        emit authenticationFailed("Principal URL not found");
        emit testCompleted(false);
        reply->deleteLater();
        return;
    }

    calendarHomeUrl = buildCalendarHomeUrl(principalUrl);
    emit authenticationSuccess(principalUrl);

    // Discover calendars first
    discoverCalendars();

    reply->deleteLater();
}

void CalDAVTester::onCalendarDiscoveryReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (hasNetworkError(reply)){
        handleNetworkError(reply, "Calendar Discovery");
        emit testCompleted(false);
        reply->deleteLater();
        return;
    }

    QString response = QString::fromUtf8(reply->readAll());
    qDebug() << "Calendar discovery response received";

    calendarUrl = extractFirstCalendarUrl(response);

    if (calendarUrl.isEmpty()){
        qDebug() << "No calendar found, trying default 'home' calendar";
        calendarUrl = calendarHomeUrl + "home/";
    }

    qDebug() << "Using calendar:" << calendarUrl;

    testEventUrl = buildTestEventUrl();
    sendPutRequest(testEventUrl, buildTestEventIcal());

    reply->deleteLater();
}

void CalDAVTester::onCreateEventReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (hasNetworkError(reply)){
        handleNetworkError(reply, "Create Event");
    }else{
        emit eventCreated(testEventUrl);
    }

    QString fetchUrl = "https://caldav.icloud.com" + calendarUrl;
    sendReportRequest(fetchUrl, buildCalendarQueryXml());
    
    reply->deleteLater();
}

void CalDAVTester::onFetchEventsReply(){
    QNetworkReply* reply = qobject_cast<QNetworkReply*>(sender());
    if (!reply) return;

    if (hasNetworkError(reply)){
        handleNetworkError(reply, "Fetch Events");
        emit testCompleted(false);
        reply->deleteLater();
        return;
    }

    QString response = QString::fromUtf8(reply->readAll());
    emit eventFetched(response);
    emit testCompleted(true);

    reply->deleteLater();
}

