#include "mainwindow.h"
#include "scheduledata.h"
#include "entityfinder.h"
#include "networkrequestbuilder.h"
#include "jsonbuilders.h"
#include "apiresponsehandler.h"
#include "displayformatter.h"
#include <QWidget>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QThread>
#include <QCoreApplication>
#include "addclassdialog.h"
#include "addeventdialog.h"
#include <QMenu>

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
    eventList->setContextMenuPolicy(Qt::CustomContextMenu);
    connect(eventList, &QListWidget::customContextMenuRequested, this, &MainWindow::onContextMenu);
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
    itemToClassId.clear();
    itemToEventId.clear();
    if (!schedule) {
        eventList->addItem("Loading...");
        return;
    }

    //Add recurring classes
    QVector<const RecurringClass*> classes = schedule->getClassesForDate(date);
    for (const RecurringClass* cls : classes){
        QString eventStr = DisplayFormatter::formatClass(*cls);
        QListWidgetItem *item = new QListWidgetItem(eventStr);
        eventList->addItem(item);
        itemToClassId[item] = cls->getId(); //Store ID Mapping
    }


    //Add One Time Events
    QVector<const OneTimeEvent*> events = schedule->getOneTimeEventsForDate(date);
    for (const OneTimeEvent* evt : events){
        QString eventStr = DisplayFormatter::formatEvent(*evt);
        QListWidgetItem *item = new QListWidgetItem(eventStr);
        eventList->addItem(item);
        itemToEventId[item] = evt->getId();
    }

    if (classes.isEmpty() && events.isEmpty()){
        eventList->addItem("No events for this day!");
    }
}

void MainWindow::onAddClassClicked() {
    AddClassDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted){
        //Build JSON for POST request
        QJsonObject classData = JsonBuilders::buildClassJson(dialog);

        //Send Post request
        QJsonDocument doc(classData);
        QByteArray jsonData = doc.toJson();

        QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, "/api/classes", apiKey);

        QNetworkReply *reply = networkManager->post(request, jsonData);
        connect(reply, &QNetworkReply::finished, this, [this, reply](){
            onClassAdded(reply);
        });
    }
}

void MainWindow::onAddExamClicked() {
    AddEventDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted){
        //Build JSON for POST request
        QJsonObject eventData = JsonBuilders::buildEventJson(dialog);

        //Send POST Request
        QJsonDocument doc(eventData);
        QByteArray jsonData = doc.toJson();

        QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, "/api/events", apiKey);

        QNetworkReply *reply = networkManager->post(request, jsonData);
        connect(reply, &QNetworkReply::finished, this, [this, reply](){
            ApiResponseHandler::handleResponse(reply,
                [this]() { fetchEvents(); },
                [this](const QString& error) { eventList->addItem("Error adding event: " + error); }
            );
        });
    }
}


void MainWindow::fetchClasses(){
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, "/api/classes", apiKey);

    QNetworkReply *reply = networkManager->get(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        onClassesFetched(reply);
    });
}

void MainWindow::fetchEvents(){
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, "/api/events", apiKey);

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

    // Clear old events before loading new ones
    schedule->clearEvents();

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

void MainWindow::onContextMenu(const QPoint &pos){
   QListWidgetItem *item = eventList->itemAt(pos);
   if (!item) return;

   //Check if it's a class or event
   bool isClass = itemToClassId.contains(item);
   bool isEvent = itemToEventId.contains(item);

   if (!isClass && !isEvent)return;

   //Create context menu
   QMenu *menu = new QMenu(this);

   if (isClass){
    int classId = itemToClassId[item];

    QAction *cancelAction = menu->addAction("Cancel this session");
    QAction *editAction = menu->addAction("Edit class");
    QAction *deleteAction = menu->addAction("Delete Class(all dates)");
    QAction *selected = menu->exec(eventList->mapToGlobal(pos));

    if (selected == cancelAction){
        onCancelClassSession(classId);
    }
    else if (selected == deleteAction){
        onDeleteClass(classId);
    }
    else if (selected == editAction){
        onEditClass(classId);
    }
   }

   if (isEvent){
    int eventId = itemToEventId[item];

    QAction *editAction = menu->addAction("Edit event");
    QAction *deleteAction = menu->addAction("Delete event");
    QAction *selected = menu->exec(eventList->mapToGlobal(pos));

    if (selected == editAction){
        onEditEvent(eventId);
    }
    else if (selected == deleteAction){
        onDeleteEvent(eventId);
    }
   }
   delete menu;
}

void MainWindow::onCancelClassSession(int classId){
    //send cancel request to server
    QJsonObject requestData;
    requestData["date"] = calendar->selectedDate().toString(Qt::ISODate);

    QJsonDocument doc(requestData);
    QByteArray jsonData = doc.toJson();

    QString endpoint = "/api/classes/" + QString::number(classId) + "/cancel";
    QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

    QNetworkReply *reply = networkManager->post(request, jsonData);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        ApiResponseHandler::handleResponse(reply,
            [this]() { fetchClasses(); },
            [this](const QString& error) { eventList->addItem("Error cancelling class: " + error); }
        );
    });
}

void MainWindow::onDeleteClass(int classId){
    QString endpoint = "/api/classes/" + QString::number(classId);
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, endpoint, apiKey);

    QNetworkReply *reply = networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        ApiResponseHandler::handleResponse(reply,
            [this]() { fetchClasses(); },
            [this](const QString& error) { eventList->addItem("Error deleting class: " + error); }
        );
    });
}

void MainWindow::onEditClass(int classId){
    //Find the class data by ID
    const RecurringClass* classPtr = EntityFinder::findClassById(*schedule, classId);

    if (!classPtr) {
        eventList->addItem("Error: Class not found for editing");
        return;
    }

    RecurringClass classToEdit = *classPtr;  // Copy the class data

    //Open Dialog pre-populated with current values
    AddClassDialog dialog(this);
    dialog.setWindowTitle("Edit Recurring Class");
    dialog.setClassName(classToEdit.getName());
    dialog.setDayOfWeek(classToEdit.getDayOfWeek());
    dialog.setStartTime(classToEdit.getStartTime());
    dialog.setEndTime(classToEdit.getEndTime());
    dialog.setRoom(classToEdit.getRoom());
    dialog.setTeacher(classToEdit.getTeacher());

    if (dialog.exec() == QDialog::Accepted){
        //Build JSON for PUT request
        QJsonObject classData = JsonBuilders::buildClassJson(dialog);

        QJsonDocument doc(classData);
        QByteArray jsonData = doc.toJson();

        QString endpoint = "/api/classes/" + QString::number(classId);
        QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

        QNetworkReply *reply = networkManager->put(request, jsonData);
        connect(reply, &QNetworkReply::finished, this, [this, reply](){
            ApiResponseHandler::handleResponse(reply,
                [this]() { fetchClasses(); },
                [this](const QString& error) { eventList->addItem("Error updating class: " + error); }
            );
        });
    }
}

void MainWindow::onEditEvent(int eventId){
    //Find Event by ID
    const OneTimeEvent* eventPtr = EntityFinder::findEventById(*schedule, eventId);

    if (!eventPtr){
        eventList->addItem("Error: Event not found for editing");
        return;
    }

    OneTimeEvent eventToEdit = *eventPtr;  // Copy the event data

    //OPen Dialog pre populated with current values
    AddEventDialog dialog(this);
    dialog.setWindowTitle("Edit Event");
    dialog.setEventName(eventToEdit.getName());
    dialog.setDate(eventToEdit.getDate());
    dialog.setStartTime(eventToEdit.getStartTime());
    dialog.setEndTime(eventToEdit.getEndTime());
    dialog.setLocation(eventToEdit.getLocation());
    dialog.setNotes(eventToEdit.getNotes());

    if (dialog.exec() == QDialog::Accepted){
        QJsonObject eventData = JsonBuilders::buildEventJson(dialog);

        QJsonDocument doc(eventData);
        QByteArray jsonData = doc.toJson();

        QString endpoint = "/api/events/" + QString::number(eventId);
        QNetworkRequest request = NetworkRequestBuilder::buildJsonRequest(serverUrl, endpoint, apiKey);

        QNetworkReply *reply = networkManager->put(request, jsonData);
        connect(reply, &QNetworkReply::finished, this, [this, reply](){
            ApiResponseHandler::handleResponse(reply,
                [this]() { fetchEvents(); },
                [this](const QString& error) { eventList->addItem("Error updating event: " + error); }
            );
        });
    }
}

void MainWindow::onDeleteEvent(int eventId){
    QString endpoint = "/api/events/" + QString::number(eventId);
    QNetworkRequest request = NetworkRequestBuilder::buildRequest(serverUrl, endpoint, apiKey);

    QNetworkReply *reply = networkManager->deleteResource(request);
    connect(reply, &QNetworkReply::finished, this, [this, reply](){
        ApiResponseHandler::handleResponse(reply,
            [this]() { fetchEvents(); },
            [this](const QString& error) { eventList->addItem("Error deleting event: " + error); }
        );
    });
}