#ifndef SCHEDULEDATA_H
#define SCHEDULEDATA_H
#include <QString>
#include <QTime>
#include <QDate>
#include <QVector>
#include <QJsonObject>
#include <QSet>

class RecurringClass {
private:
    int id;
    QString name;
    int dayOfWeek;
    QTime startTime;
    QTime endTime;
    QString room;
    QString teacher;
    QSet<QDate> cancelledDates;

public:
    //Default Constructor
    RecurringClass() : id(-1), dayOfWeek(1){}

    //Overloaded Constructor
    RecurringClass(const QString &name, int dayOfWeek, const QTime &start, const QTime &end, const QString &room = "", const QString &teacher = "")
        : id(-1), name (name), dayOfWeek(dayOfWeek), startTime(start), endTime(end), room(room), teacher(teacher){}

    // Getters
    int getId() const { return id; }
    QString getName() const { return name; }
    int getDayOfWeek() const { return dayOfWeek; }
    QTime getStartTime() const { return startTime; }
    QTime getEndTime() const { return endTime; }
    QString getRoom() const { return room; }
    QString getTeacher() const { return teacher; }
    const QSet<QDate>& getCancelledDates() const { return cancelledDates; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const QString &newName) { name = newName; }
    void setDayOfWeek(int day) { dayOfWeek = day; }
    void setStartTime(const QTime &time) { startTime = time; }
    void setEndTime(const QTime &time) { endTime = time; }
    void setRoom(const QString &newRoom) { room = newRoom; }
    void setTeacher(const QString &newTeacher) { teacher = newTeacher; }

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
private:
    int id;
    QString name;
    QDate date;
    QTime startTime;
    QTime endTime;
    QString location;
    QString notes;

public:
    //Default Constructor
    OneTimeEvent() : id(-1) {}

    //Overloaded Constructor
    OneTimeEvent(const QString &name, const QDate &date,
                 const QTime &start, const QTime &end = QTime(),
                 const QString &location = "", const QString &notes = "")
        : id(-1), name(name), date(date), startTime(start), endTime(end), location(location), notes(notes){}

    // Getters
    int getId() const { return id; }
    QString getName() const { return name; }
    QDate getDate() const { return date; }
    QTime getStartTime() const { return startTime; }
    QTime getEndTime() const { return endTime; }
    QString getLocation() const { return location; }
    QString getNotes() const { return notes; }

    // Setters
    void setId(int newId) { id = newId; }
    void setName(const QString &newName) { name = newName; }
    void setDate(const QDate &newDate) { date = newDate; }
    void setStartTime(const QTime &time) { startTime = time; }
    void setEndTime(const QTime &time) { endTime = time; }
    void setLocation(const QString &newLocation) { location = newLocation; }
    void setNotes(const QString &newNotes) { notes = newNotes; }

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

        QVector<OneTimeEvent>& getOneTimeEventsMutable(){
            return oneTimeEvents;
        }

        void clearEvents() {
            oneTimeEvents.clear();
        }

        bool saveToFile(const QString &filename) const;
        bool loadFromFile(const QString &filename);

    private:
        QVector<RecurringClass> recurringClasses;
        QVector<OneTimeEvent> oneTimeEvents;
};

#endif // SCHEDULEDATA_H
