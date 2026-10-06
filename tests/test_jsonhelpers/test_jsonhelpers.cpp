#include <QtTest>
#include "jsonhelpers.h"
#include "scheduledata.h"

class TestJsonHelpers : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    // RecurringClass tests
    void testRecurringClassToJson();
    void testRecurringClassFromJson();
    void testRecurringClassRoundTrip();

    // OneTimeEvent tests
    void testOneTimeEventToJson();
    void testOneTimeEventFromJson();
    void testOneTimeEventRoundTrip();
};

void TestJsonHelpers::initTestCase()
{
    // Setup before all tests
}

void TestJsonHelpers::cleanupTestCase()
{
    // Cleanup after all tests
}

void TestJsonHelpers::testRecurringClassToJson()
{
    RecurringClass cls;
    cls.setId(1);
    cls.setName("Mathematics");
    cls.setDayOfWeek(1); // Monday
    cls.setStartTime(QTime(9, 0));
    cls.setEndTime(QTime(10, 30));
    cls.setRoom("A101");
    cls.setTeacher("Dr. Smith");

    QJsonObject json = cls.toJson();

    QCOMPARE(json["id"].toInt(), 1);
    QCOMPARE(json["name"].toString(), QString("Mathematics"));
    QCOMPARE(json["dayOfWeek"].toInt(), 1);
    QCOMPARE(json["startTime"].toString(), QString("09:00"));
    QCOMPARE(json["endTime"].toString(), QString("10:30"));
    QCOMPARE(json["room"].toString(), QString("A101"));
    QCOMPARE(json["teacher"].toString(), QString("Dr. Smith"));
}

void TestJsonHelpers::testRecurringClassFromJson()
{
    QJsonObject json;
    json["id"] = 2;
    json["name"] = "Physics";
    json["dayOfWeek"] = 3; // Wednesday
    json["startTime"] = "14:00";
    json["endTime"] = "15:30";
    json["room"] = "B202";
    json["teacher"] = "Prof. Johnson";
    json["cancelledDates"] = QJsonArray();

    RecurringClass cls = RecurringClass::fromJson(json);

    QCOMPARE(cls.getId(), 2);
    QCOMPARE(cls.getName(), QString("Physics"));
    QCOMPARE(cls.getDayOfWeek(), 3);
    QCOMPARE(cls.getStartTime(), QTime(14, 0));
    QCOMPARE(cls.getEndTime(), QTime(15, 30));
    QCOMPARE(cls.getRoom(), QString("B202"));
    QCOMPARE(cls.getTeacher(), QString("Prof. Johnson"));
}

void TestJsonHelpers::testRecurringClassRoundTrip()
{
    // Create original
    RecurringClass original;
    original.setId(5);
    original.setName("Chemistry");
    original.setDayOfWeek(5);
    original.setStartTime(QTime(11, 0));
    original.setEndTime(QTime(12, 30));
    original.setRoom("C303");
    original.setTeacher("Dr. Brown");

    // Convert to JSON and back
    QJsonObject json = original.toJson();
    RecurringClass restored = RecurringClass::fromJson(json);

    // Verify all fields match
    QCOMPARE(restored.getId(), original.getId());
    QCOMPARE(restored.getName(), original.getName());
    QCOMPARE(restored.getDayOfWeek(), original.getDayOfWeek());
    QCOMPARE(restored.getStartTime(), original.getStartTime());
    QCOMPARE(restored.getEndTime(), original.getEndTime());
    QCOMPARE(restored.getRoom(), original.getRoom());
    QCOMPARE(restored.getTeacher(), original.getTeacher());
}

void TestJsonHelpers::testOneTimeEventToJson()
{
    OneTimeEvent evt;
    evt.setId(10);
    evt.setName("Final Exam");
    evt.setDate(QDate(2026, 12, 15));
    evt.setStartTime(QTime(9, 0));
    evt.setEndTime(QTime(12, 0));
    evt.setLocation("Main Hall");
    evt.setNotes("Bring ID");

    QJsonObject json = evt.toJson();

    QCOMPARE(json["id"].toInt(), 10);
    QCOMPARE(json["name"].toString(), QString("Final Exam"));
    QCOMPARE(json["date"].toString(), QString("2026-12-15"));
    QCOMPARE(json["startTime"].toString(), QString("09:00"));
    QCOMPARE(json["endTime"].toString(), QString("12:00"));
    QCOMPARE(json["location"].toString(), QString("Main Hall"));
    QCOMPARE(json["notes"].toString(), QString("Bring ID"));
}

void TestJsonHelpers::testOneTimeEventFromJson()
{
    QJsonObject json;
    json["id"] = 20;
    json["name"] = "Midterm Exam";
    json["date"] = "2026-10-20";
    json["startTime"] = "10:00";
    json["endTime"] = "11:30";
    json["location"] = "Room 404";
    json["notes"] = "Chapter 1-5";

    OneTimeEvent evt = OneTimeEvent::fromJson(json);

    QCOMPARE(evt.getId(), 20);
    QCOMPARE(evt.getName(), QString("Midterm Exam"));
    QCOMPARE(evt.getDate(), QDate(2026, 10, 20));
    QCOMPARE(evt.getStartTime(), QTime(10, 0));
    QCOMPARE(evt.getEndTime(), QTime(11, 30));
    QCOMPARE(evt.getLocation(), QString("Room 404"));
    QCOMPARE(evt.getNotes(), QString("Chapter 1-5"));
}

void TestJsonHelpers::testOneTimeEventRoundTrip()
{
    // Create original
    OneTimeEvent original;
    original.setId(30);
    original.setName("Guest Lecture");
    original.setDate(QDate(2026, 11, 5));
    original.setStartTime(QTime(14, 0));
    original.setEndTime(QTime(16, 0));
    original.setLocation("Auditorium");
    original.setNotes("Dr. Einstein");

    // Convert to JSON and back
    QJsonObject json = original.toJson();
    OneTimeEvent restored = OneTimeEvent::fromJson(json);

    // Verify all fields match
    QCOMPARE(restored.getId(), original.getId());
    QCOMPARE(restored.getName(), original.getName());
    QCOMPARE(restored.getDate(), original.getDate());
    QCOMPARE(restored.getStartTime(), original.getStartTime());
    QCOMPARE(restored.getEndTime(), original.getEndTime());
    QCOMPARE(restored.getLocation(), original.getLocation());
    QCOMPARE(restored.getNotes(), original.getNotes());
}

QTEST_MAIN(TestJsonHelpers)
#include "test_jsonhelpers.moc"
