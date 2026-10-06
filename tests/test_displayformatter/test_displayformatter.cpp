#include <QtTest>
#include "displayformatter.h"
#include "scheduledata.h"

class TestDisplayFormatter : public QObject
{
    Q_OBJECT

private slots:
    void testFormatClass_Complete();
    void testFormatClass_NoRoom();
    void testFormatClass_NoTeacher();
    void testFormatClass_NoRoomOrTeacher();

    void testFormatEvent_Complete();
    void testFormatEvent_NoLocation();
    void testFormatEvent_NoNotes();
    void testFormatEvent_NoLocationOrNotes();
    void testFormatEvent_NoEndTime();
};

void TestDisplayFormatter::testFormatClass_Complete()
{
    RecurringClass cls;
    cls.setName("Mathematics");
    cls.setStartTime(QTime(9, 0));
    cls.setEndTime(QTime(10, 30));
    cls.setRoom("A101");
    cls.setTeacher("Dr. Smith");

    QString formatted = DisplayFormatter::formatClass(cls);

    QCOMPARE(formatted, QString("09:00 - 10:30 | Mathematics (A101, Dr. Smith)"));
}

void TestDisplayFormatter::testFormatClass_NoRoom()
{
    RecurringClass cls;
    cls.setName("Physics");
    cls.setStartTime(QTime(11, 0));
    cls.setEndTime(QTime(12, 30));
    cls.setRoom("");
    cls.setTeacher("Prof. Johnson");

    QString formatted = DisplayFormatter::formatClass(cls);

    QCOMPARE(formatted, QString("11:00 - 12:30 | Physics (Prof. Johnson)"));
}

void TestDisplayFormatter::testFormatClass_NoTeacher()
{
    RecurringClass cls;
    cls.setName("Chemistry");
    cls.setStartTime(QTime(14, 0));
    cls.setEndTime(QTime(15, 30));
    cls.setRoom("B202");
    cls.setTeacher("");

    QString formatted = DisplayFormatter::formatClass(cls);

    QCOMPARE(formatted, QString("14:00 - 15:30 | Chemistry (B202)"));
}

void TestDisplayFormatter::testFormatClass_NoRoomOrTeacher()
{
    RecurringClass cls;
    cls.setName("Biology");
    cls.setStartTime(QTime(8, 0));
    cls.setEndTime(QTime(9, 30));
    cls.setRoom("");
    cls.setTeacher("");

    QString formatted = DisplayFormatter::formatClass(cls);

    QCOMPARE(formatted, QString("08:00 - 09:30 | Biology"));
}

void TestDisplayFormatter::testFormatEvent_Complete()
{
    OneTimeEvent evt;
    evt.setName("Final Exam");
    evt.setStartTime(QTime(9, 0));
    evt.setEndTime(QTime(12, 0));
    evt.setLocation("Main Hall");
    evt.setNotes("Bring ID");

    QString formatted = DisplayFormatter::formatEvent(evt);

    QCOMPARE(formatted, QString("09:00 - 12:00 | Final Exam (Main Hall, Bring ID)"));
}

void TestDisplayFormatter::testFormatEvent_NoLocation()
{
    OneTimeEvent evt;
    evt.setName("Study Session");
    evt.setStartTime(QTime(15, 0));
    evt.setEndTime(QTime(17, 0));
    evt.setLocation("");
    evt.setNotes("Chapter 5-8");

    QString formatted = DisplayFormatter::formatEvent(evt);

    QCOMPARE(formatted, QString("15:00 - 17:00 | Study Session (Chapter 5-8)"));
}

void TestDisplayFormatter::testFormatEvent_NoNotes()
{
    OneTimeEvent evt;
    evt.setName("Guest Lecture");
    evt.setStartTime(QTime(10, 0));
    evt.setEndTime(QTime(11, 30));
    evt.setLocation("Auditorium");
    evt.setNotes("");

    QString formatted = DisplayFormatter::formatEvent(evt);

    QCOMPARE(formatted, QString("10:00 - 11:30 | Guest Lecture (Auditorium)"));
}

void TestDisplayFormatter::testFormatEvent_NoLocationOrNotes()
{
    OneTimeEvent evt;
    evt.setName("Meeting");
    evt.setStartTime(QTime(13, 0));
    evt.setEndTime(QTime(14, 0));
    evt.setLocation("");
    evt.setNotes("");

    QString formatted = DisplayFormatter::formatEvent(evt);

    QCOMPARE(formatted, QString("13:00 - 14:00 | Meeting"));
}

void TestDisplayFormatter::testFormatEvent_NoEndTime()
{
    OneTimeEvent evt;
    evt.setName("Office Hours");
    evt.setStartTime(QTime(16, 0));
    evt.setEndTime(QTime());  // Invalid time
    evt.setLocation("Room 101");
    evt.setNotes("Drop-in");

    QString formatted = DisplayFormatter::formatEvent(evt);

    QCOMPARE(formatted, QString("16:00 | Office Hours (Room 101, Drop-in)"));
}

QTEST_MAIN(TestDisplayFormatter)
#include "test_displayformatter.moc"
