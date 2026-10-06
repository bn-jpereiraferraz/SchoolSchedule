#include "caldavtester.h"
#include <QCoreApplication>
#include <QDebug>
#include <QTimer>

int main(int argc, char *argv[]) {
    QCoreApplication app(argc, argv);

    if (argc < 3) {
        qDebug() << "Usage: caldav_test <apple-id@icloud.com> <app-specific-password>";
        qDebug() << "";
        qDebug() << "Get your app-specific password from:";
        qDebug() << "  1. Go to appleid.apple.com";
        qDebug() << "  2. Sign in";
        qDebug() << "  3. Security → App-Specific Passwords";
        qDebug() << "  4. Generate one for 'School Calendar'";
        return 1;
    }

    QString appleId = QString::fromUtf8(argv[1]);
    QString appPassword = QString::fromUtf8(argv[2]);

    qDebug() << "===========================================";
    qDebug() << "  CalDAV Connection Test";
    qDebug() << "===========================================";
    qDebug() << "Apple ID:" << appleId;
    qDebug() << "Testing connection to caldav.icloud.com...";
    qDebug() << "";

    CalDAVTester tester;

    QObject::connect(&tester, &CalDAVTester::testStarted, []() {
        qDebug() << "[1/4] Starting authentication test...";
    });

    QObject::connect(&tester, &CalDAVTester::authenticationSuccess, [](const QString& principalUrl) {
        qDebug() << "[2/4] ✅ Authentication SUCCESS!";
        qDebug() << "      Principal URL:" << principalUrl;
    });

    QObject::connect(&tester, &CalDAVTester::authenticationFailed, [](const QString& error) {
        qDebug() << "[2/4] ❌ Authentication FAILED:";
        qDebug() << "      " << error;
    });

    QObject::connect(&tester, &CalDAVTester::eventCreated, [](const QString& eventUrl) {
        if (!eventUrl.isEmpty()) {
            qDebug() << "[3/4] ✅ Test event created!";
            qDebug() << "      Event URL:" << eventUrl;
            qDebug() << "      Check your iPhone Calendar for 'CalDAV Test Event'";
        } else {
            qDebug() << "[3/4] ⚠️  Event creation skipped (calendar discovery needed)";
        }
    });

    QObject::connect(&tester, &CalDAVTester::eventFetched, [](const QString& data) {
        qDebug() << "[4/4] ✅ Events fetched successfully!";
        if (data.contains("CalDAV Test Event")) {
            qDebug() << "      Found our test event in response!";
        }
    });

    QObject::connect(&tester, &CalDAVTester::testCompleted, [&app](bool success) {
        qDebug() << "";
        qDebug() << "===========================================";
        if (success) {
            qDebug() << "  ✅ TEST PASSED - CalDAV connection works!";
            qDebug() << "  Your School Calendar can sync with iCloud!";
        } else {
            qDebug() << "  ❌ TEST FAILED - Check credentials or network";
        }
        qDebug() << "===========================================";

        // Exit after 1 second to show final messages
        QTimer::singleShot(1000, &app, &QCoreApplication::quit);
    });

    // Start the test
    tester.testConnection(appleId, appPassword);

    // Run event loop
    return app.exec();
}
