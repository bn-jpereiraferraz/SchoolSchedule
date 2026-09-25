#ifndef SCHEDULEDATA_H
#define SCHEDULEDATA_H
#include <QString>
#include <QTime>
#include <QDate>
#include <QVector>
#include <QJsonObject>
#include <QSet>

class RecurringClass {
    public:
        QString name;
        int dayOfWeek;
        QTime startTime;
        QTime endTime;
        QString room;
        QString teacher;
        QSet<QDate> cancelledDates;

    //Default Constructor
    RecurringClass() : dayOfWeek(1){}

    //Overloaded Constructor
    RecurringClass(const QString &name, int dayOfWeek, const QTime &start, const QTime &end, const QString &room = "", const QString &teacher = "")
        : name (name), dayOfWeek(dayOfWeek), startTime(start), endTime(end), room(room), teacher(teacher){}

    //Check if this class is cancelled on a specific date
    bool isCancelledOn(const QDate &date)const{
        return cancelledDates.contains(date);
    }

    //Mark this class as cancelled on a specific date
    void cancelOn(const QDate &date){
        cancelledDates.insert(date);
    }

    //Remove Cancellation for a specific date
    void uncancelOn(const QDate &date){
        cancelledDates.remove(date);
    }

    //Convert to Json for savig
    QJsonObject toJson() const;

    //Create from Json when Loading
    static RecurringClass fromJson(const QJsonObject &json);
};

//Represents a one-time event (exam, test, etc.)
class OneTimeEvent{
    public:
        QString name;
        QDate date;
        QTime startTime;
        QTime endTime;
        QString location;
        QString notes;

    //Default Constructor
    OneTimeEvent(){}

    //Overloaded Constructor
    OneTimeEvent(const QString &name, const QDate &date,
                 const QTime &start, const QTime &end = QTime(),
                 const QString &location = "", const QString &notes = "")
        : name(name), date(date), startTime(start), endTime(end), location(location), notes(notes){}
    
    //Convert JSON for saving
    QJsonObject toJson() const;

    //Create from JSON when loading
    static OneTimeEvent fromJson(const QJsonObject &json);
};


//Main Schedule manager
class Schedule{
    public:
        Schedule(){}

        void addRecurringClass(const RecurringClass &cls){
            recurringClasses.append(cls);
        }

        void addOneTimeEvent(const OneTimeEvent &evt){
            oneTimeEvents.append(evt);
        }

        void removeRecurringClass(int index){
            if (index >= 0 && index < recurringClasses.size()){
                recurringClasses.removeAt(index);
            }
        }

        void removeOneTimeEvent(int index){
            if (index >= 0 && index < oneTimeEvents.size()){
                oneTimeEvents.removeAt(index);
            }
        }

        //Get all events (classes + one-time events) for a specific date as formated strings
        QVector<QString> getEventsForDate(const QDate &date) const;

        //Get Recurring Classes that occur on this date(not cancelled)
        QVector<const RecurringClass*> getClassesForDate(const QDate &date) const;

        //Get one time events for this date
        QVector<const OneTimeEvent*> getOneTimeEventsForDate(const QDate &date) const;

        const QVector<RecurringClass>& getRecurringClasses() const {
            return recurringClasses;
        }

        const QVector<OneTimeEvent>& getOneTimeEvents()const {
            return oneTimeEvents;
        }

        QVector<RecurringClass>& getRecurringClassesMutable(){
            return recurringClasses;
        }

        bool saveToFile(const QString &filename) const;
        bool loadFromFile(const QString &filename);

    private:
        QVector<RecurringClass> recurringClasses;
        QVector<OneTimeEvent> oneTimeEvents;
};

#endif // SCHEDULEDATA_H
