#ifndef SCHEDULEREPOSITORY_H
#define SCHEDULEREPOSITORY_H

#include <QObject>
#include <QNetworkAccessManager>
#include <QNetworkReply>
#include <memory>
#include "scheduledata.h"

//Repository handles all data access and network operations
//MainWindow will depend on this instead of doing networking directly
class ScheduleRepository : public QObject{
    Q_OBJECT

public:
    explicit ScheduleRepository (const QString& serverUrl, const QString& apiKey, QObject* parent = nullptr);

    ~ScheduleRepository();

    const Schedule* getSchedule() const;

    //Network Operations - all async with signals
    void fetchClasses();
    void fetchEvents();
    void addClass(const RecurringClass& cls);
    void addEvent(const OneTimeEvent& evt);
    void updateClass(int id, const RecurringClass& cls);
    void updateEvent(int id, const OneTimeEvent& evt);
    void deleteClass(int id);
    void deleteEvent(int id);
    void cancelClassSession(int id, const QDate& date);

signals:
    //Success signals
    void classesLoaded();
    void eventsLoaded();
    void classAdded();
    void eventAdded();
    void classUpdated();
    void eventUpdated();
    void classDeleted();
    void eventDeleted();
    void classSessionCancelled();
    
    //Error Signal - emitted for any operation failure
    void operationFailed(const QString& operation, const QString& error);

private slots:
    void onClassesFetched(QNetworkReply* reply);
    void onEventsFetched(QNetworkReply* reply);
    void onClassAdded(QNetworkReply* reply);
    void onEventAdded(QNetworkReply* reply);
    void onClassUpdated(QNetworkReply* reply);
    void onEventUpdated(QNetworkReply* reply);
    void onClassDeleted(QNetworkReply* reply);
    void onEventDeleted(QNetworkReply* reply);
    void onClassSessionCancelled(QNetworkReply* reply);

private:
    QNetworkAccessManager* networkManager;
    std::unique_ptr<Schedule> schedule; //<- smartpointer
    QString serverUrl;
    QString apiKey;

    
};



#endif //SCHEDULEREPOSITORY_H