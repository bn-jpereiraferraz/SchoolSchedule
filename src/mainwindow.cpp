#include "mainwindow.h"
#include "scheduledata.h"
#include <QWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QCoreApplication>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
            //Initialize schedule pointer
    schedule = nullptr;
    setupUI();
    setWindowTitle("School Calendar");
    resize(900, 600);

    //Start server process automatically
    serverProcess = new QProcess(this);
    QString serverPath = QCoreApplication::applicationDirPath() + "/SchoolCalendarServer";
    serverProcess->start(serverPath);

    //Wait for the server to start
    QThread::msleep(1000);

    //Initialize network manager and server settings
    networkManager = new QNetworkAccessManager(this);
    serverUrl = "http://localhost:8080";
    apiKey = "your-secret-key";

    //Fetch initial data from the server
    fetchClasses();
    fetchEvents();
}

MainWindow::~MainWindow() {
    //Stop the server process when client closes
    if (serverProcess && serverProcess->state() == QProcess::Running){
        serverProcess->terminate();
        serverProcess->waitForFinished(3000); //Wait up to 3 seconds

        if (serverProcess->state() == QProcess::Running){
            serverProcess->kill(); //Force kill if still running
        }
    }

    //Clean up schedule
    if (schedule){
        delete schedule;
    }
}

void MainWindow::setupUI() {
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);

    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    // Left side - Calendar
    QVBoxLayout *leftLayout = new QVBoxLayout();
    calendar = new QCalendarWidget();
    calendar->setGridVisible(true);
    leftLayout->addWidget(calendar);

    // Right side - Events and controls
    QVBoxLayout *rightLayout = new QVBoxLayout();

    selectedDateLabel = new QLabel("Select a date");
    selectedDateLabel->setStyleSheet("font-size: 14pt; font-weight: bold;");
    rightLayout->addWidget(selectedDateLabel);

    eventList = new QListWidget();
    rightLayout->addWidget(eventList);

    addClassButton = new QPushButton("Add Recurring Class");
    addExamButton = new QPushButton("Add Test/Exam");

    rightLayout->addWidget(addClassButton);
    rightLayout->addWidget(addExamButton);

    mainLayout->addLayout(leftLayout, 2);
    mainLayout->addLayout(rightLayout, 1);

    // Connect signals
    connect(calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        onDateSelected(calendar->selectedDate());
    });
    connect(addClassButton, &QPushButton::clicked, this, &MainWindow::onAddClassClicked);
    connect(addExamButton, &QPushButton::clicked, this, &MainWindow::onAddExamClicked);

    // Set initial date
    onDateSelected(calendar->selectedDate());
}

void MainWindow::onDateSelected(const QDate &date) {
    selectedDateLabel->setText(date.toString("dddd, MMMM d, yyyy"));
    updateEventListForDate(date);
}

void MainWindow::updateEventListForDate(const QDate &date) {
    eventList->clear();

    if (!schedule) {
        eventList->addItem("Loading...");
        return;
    }

    QVector<QString> events = schedule->getEventsForDate(date);

    if (events.isEmpty()) {
        eventList->addItem("No events for this day");
    } else {
        for (const QString &event : events) {
            eventList->addItem(event);
        }
    }
}

void MainWindow::onAddClassClicked() {
    // TODO: Open dialog to add recurring class
    eventList->addItem("Add Class feature coming soon...");
}

void MainWindow::onAddExamClicked() {
    // TODO: Open dialog to add exam
    eventList->addItem("Add Exam feature coming soon...");
}


void MainWindow::fetchClasses(){
    QNetworkRequest request(QUrl(serverUrl + "/api/classes"));
    request.setRawHeader("X-API-Key", apiKey.toUtf8());

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onClassesFetched(reply);
    });
}

void MainWindow::fetchEvents(){
    QNetworkRequest request(QUrl(serverUrl + "/api/events"));
    request.setRawHeader("X-API-Key", apiKey.toUtf8());

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onEventsFetched(reply);
    });
}

void MainWindow::onClassesFetched(QNetworkReply *reply){
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError){
        eventList->addItem("Error fetching classes: " + reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonArray classesArray = doc.array();

    //Clear existing schedule and load classes 
    if(schedule){
        delete schedule;
    }
    schedule = new Schedule();

    for(const QJsonValue &val : classesArray){
        RecurringClass cls = RecurringClass::fromJson(val.toObject());
        schedule->addRecurringClass(cls);
    }

    //Refresh display
    onDateSelected(calendar->selectedDate());
}

void MainWindow::onEventsFetched(QNetworkReply *reply){
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError){
        eventList->addItem("Error fetching events: " + reply->errorString());
        return;
    }

    QByteArray responseData = reply->readAll();
    QJsonDocument doc = QJsonDocument::fromJson(responseData);
    QJsonArray eventsArray = doc.array();

    //Load Events into schedule
    if (!schedule){
        schedule = new Schedule();
    }

    for(const QJsonValue &val : eventsArray){
        OneTimeEvent evt = OneTimeEvent::fromJson(val.toObject());
        schedule->addOneTimeEvent(evt);
    }

    //Refresh display
    onDateSelected(calendar->selectedDate());
}

void MainWindow::onClassAdded(QNetworkReply *reply){
    reply->deleteLater();

    if (reply->error() != QNetworkReply::NoError){
        eventList->addItem("Error adding class: " + reply->errorString());
        return;
    }

    //Refresh data from server
    fetchClasses();
}