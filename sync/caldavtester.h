#ifndef CALDAVTESTER_H
#define CALDAVTESTER_H
#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <QString>

class CalDAVTester : public QObject{
    Q_OBJECT
public:
    explicit CalDAVTester(QObject* parent  = nullptr);
    ~CalDAVTester();

    void testConnection(const QString& appleId, const QString& appPassword);

signals:
    void testStarted();
    void authenticationSuccess(const QString& principalUrl);
    void authenticationFailed(const QString& error);
    void eventCreated(const QString& eventUrl);
    void eventFetched(const QString& eventData);
    void testCompleted(bool success);

private slots:
    void onAuthenticationReply();
    void onCalendarDiscoveryReply();
    void onCreateEventReply();
    void onFetchEventsReply();

private:
    //Network operations
    void sendPropfindRequest();
    void discoverCalendars();
    void sendPutRequest(const QString& url, const QByteArray& data);
    void sendReportRequest(const QString& url, const QByteArray& query);

    //Response parsing
    QString extractPrincipalUrl(const QString& xmlResponse);
    QString buildCalendarHomeUrl(const QString& principal);
    QString extractFirstCalendarUrl(const QString& xmlResponse);

    //Request Building
    QNetworkRequest buildAuthenticatedRequest(const QUrl& url);
    QByteArray buildPropfindXml();
    QByteArray buildCalendarDiscoveryXml();
    QByteArray buildTestEventIcal();
    QByteArray buildCalendarQueryXml();
    QString buildTestEventUrl();

    //Authentication
    QString buildBasicAuthHeader() const;

    //Error Handling
    void handleNetworkError(QNetworkReply* reply, const QString& operation);
    bool hasNetworkError(QNetworkReply* reply);

    QNetworkAccessManager* networkManager;
    QString appleId;
    QString appPassword;
    QString principalUrl;
    QString calendarHomeUrl;
    QString calendarUrl;
    QString testEventUrl;
};
#endif //CALDAVTESTER_H