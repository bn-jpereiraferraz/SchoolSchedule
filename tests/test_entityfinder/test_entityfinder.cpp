#include <QtTest>
#include "entityfinder.h"
#include "scheduledata.h"

class TestEntityFinder : public QObject
{
    Q_OBJECT

private slots:
    void initTestCase();
    void cleanupTestCase();

    void testFindClassById_Found();
    void testFindClassById_NotFound();
    void testFindClassById_Const();

    void testFindEventById_Found();
    void testFindEventById_NotFound();
    void testFindEventById_Const();

    void testFindClassIndexById_Found();
    void testFindClassIndexById_NotFound();

    void testFindEventIndexById_Found();
    void testFindEventIndexById_NotFound();

private:
    Schedule schedule;
};

void TestEntityFinder::initTestCase()
{
    // Add test classes
    RecurringClass cls1;
    cls1.setId(1);
    cls1.setName("Math");
    cls1.setDayOfWeek(1);
    cls1.setStartTime(QTime(9, 0));
    cls1.setEndTime(QTime(10, 0));
    schedule.addRecurringClass(cls1);

    RecurringClass cls2;
    cls2.setId(2);
    cls2.setName("Physics");
    cls2.setDayOfWeek(2);
    cls2.setStartTime(QTime(11, 0));
    cls2.setEndTime(QTime(12, 0));
    schedule.addRecurringClass(cls2);

    // Add test events
    OneTimeEvent evt1;
    evt1.setId(10);
    evt1.setName("Exam 1");
    evt1.setDate(QDate(2026, 12, 10));
    evt1.setStartTime(QTime(9, 0));
    evt1.setEndTime(QTime(11, 0));
    schedule.addOneTimeEvent(evt1);

    OneTimeEvent evt2;
    evt2.setId(20);
    evt2.setName("Exam 2");
    evt2.setDate(QDate(2026, 12, 15));
    evt2.setStartTime(QTime(14, 0));
    evt2.setEndTime(QTime(16, 0));
    schedule.addOneTimeEvent(evt2);
}

void TestEntityFinder::cleanupTestCase()
{
    // Cleanup
}

void TestEntityFinder::testFindClassById_Found()
{
    RecurringClass* found = EntityFinder::findClassById(schedule, 1);

    QVERIFY(found != nullptr);
    QCOMPARE(found->getId(), 1);
    QCOMPARE(found->getName(), QString("Math"));
}

void TestEntityFinder::testFindClassById_NotFound()
{
    RecurringClass* found = EntityFinder::findClassById(schedule, 999);

    QVERIFY(found == nullptr);
}

void TestEntityFinder::testFindClassById_Const()
{
    const Schedule& constSchedule = schedule;
    const RecurringClass* found = EntityFinder::findClassById(constSchedule, 2);

    QVERIFY(found != nullptr);
    QCOMPARE(found->getId(), 2);
    QCOMPARE(found->getName(), QString("Physics"));
}

void TestEntityFinder::testFindEventById_Found()
{
    OneTimeEvent* found = EntityFinder::findEventById(schedule, 10);

    QVERIFY(found != nullptr);
    QCOMPARE(found->getId(), 10);
    QCOMPARE(found->getName(), QString("Exam 1"));
}

void TestEntityFinder::testFindEventById_NotFound()
{
    OneTimeEvent* found = EntityFinder::findEventById(schedule, 999);

    QVERIFY(found == nullptr);
}

void TestEntityFinder::testFindEventById_Const()
{
    const Schedule& constSchedule = schedule;
    const OneTimeEvent* found = EntityFinder::findEventById(constSchedule, 20);

    QVERIFY(found != nullptr);
    QCOMPARE(found->getId(), 20);
    QCOMPARE(found->getName(), QString("Exam 2"));
}

void TestEntityFinder::testFindClassIndexById_Found()
{
    int index = EntityFinder::findClassIndexById(schedule, 1);

    QCOMPARE(index, 0);
}

void TestEntityFinder::testFindClassIndexById_NotFound()
{
    int index = EntityFinder::findClassIndexById(schedule, 999);

    QCOMPARE(index, -1);
}

void TestEntityFinder::testFindEventIndexById_Found()
{
    int index = EntityFinder::findEventIndexById(schedule, 10);

    QCOMPARE(index, 0);
}

void TestEntityFinder::testFindEventIndexById_NotFound()
{
    int index = EntityFinder::findEventIndexById(schedule, 999);

    QCOMPARE(index, -1);
}

QTEST_MAIN(TestEntityFinder)
#include "test_entityfinder.moc"
