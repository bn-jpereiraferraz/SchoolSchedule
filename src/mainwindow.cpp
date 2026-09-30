#include "mainwindow.h"
#include "entityfinder.h"
#include "jsonbuilders.h"
#include "displayformatter.h"
#include "constants.h"
#include <QWidget>
#include <QThread>
#include <QCoreApplication>
#include "addclassdialog.h"
#include "addeventdialog.h"
#include <QMenu>
#include <QMessageBox>

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent){
        setupUI();
        setWindowTitle("School Calendar");
        resize(Constants::DEFAULT_WINDOW_WIDTH, Constants::DEFAULT_WINDOW_HEIGHT);

        startServer();
        setupRepository();
    }

void MainWindow::startServer(){
    serverProcess = new QProcess(this);
    QString serverPath = QCoreApplication::applicationDirPath() + "/SchoolCalendarServer";

    connect(serverProcess, &QProcess::started, this, [this](){
        initializeRepository();
    });

    connect(serverProcess, &QProcess::errorOccurred, this, [this](QProcess::ProcessError error){
        QMessageBox::critical(this, "Server Error", "Failed to start server: " + serverProcess->errorString());
    });

    serverProcess->start(serverPath);

    //Show loading indicator while server starts
    eventList->addItem("Starting server...");
}

void MainWindow::setupRepository(){
    repository = new ScheduleRepository(Constants::DEFAULT_SERVER_URL, Constants::DEFAULT_API_KEY, this);

    //connect all signals from schedulerepository
    connect(repository, &ScheduleRepository::classesLoaded, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::eventsLoaded, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::classAdded, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::eventAdded, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::classUpdated, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::eventUpdated, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::classDeleted, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::eventDeleted, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::classSessionCancelled, this, &MainWindow::onDataLoaded);
    connect(repository, &ScheduleRepository::operationFailed, this, &MainWindow::onRepositoryError);
}

void MainWindow::initializeRepository(){
    //Called after server starts
    eventList->clear();
    eventList->addItem("Loading data...");

    repository->fetchClasses();
    repository->fetchEvents();
}

MainWindow::~MainWindow(){
    //Stop the server process when client closes
    if (serverProcess && serverProcess->state() == QProcess::Running){
        // Disconnect error signal to avoid popup during normal shutdown
        disconnect(serverProcess, &QProcess::errorOccurred, this, nullptr);

        serverProcess->terminate();
        serverProcess->waitForFinished(Constants::SERVER_SHUTDOWN_TIMEOUT_MS);

        if (serverProcess->state() == QProcess::Running){
            serverProcess->kill();
        }
    }
}

void MainWindow::setupUI(){
    QWidget *centralWidget = new QWidget(this);
    setCentralWidget(centralWidget);
    QHBoxLayout *mainLayout = new QHBoxLayout(centralWidget);

    mainLayout->addLayout(createLeftPanel(), 2);
    mainLayout->addLayout(createRightPanel(), 1);

    connectSignals();
    onDateSelected(calendar->selectedDate());  // Set initial date
}

QVBoxLayout* MainWindow::createLeftPanel() {
    QVBoxLayout *leftLayout = new QVBoxLayout();
    
    todayButton = new QPushButton("📅 Today");
    todayButton->setMaximumWidth(100);
    todayButton->setStyleSheet(
        "font-size: 11pt; "
        "padding: 6px 12px; "
        "background-color: #2196F3; "
        "color: white; "
        "border: none; "
        "border-radius: 4px; "
    );
    leftLayout->addWidget(todayButton);

    //Calendar
    calendar = new CustomCalendar();
    calendar->setGridVisible(true);
    leftLayout->addWidget(calendar);
    return leftLayout;
}

QVBoxLayout* MainWindow::createRightPanel() {
    QVBoxLayout *rightLayout = new QVBoxLayout();

    selectedDateLabel = new QLabel("Select a date");
    selectedDateLabel->setStyleSheet(
        "font-size: 14pt; " 
        "font-weight: bold; "
        "color:#212121; "
        "padding: 8px; "
        "background-color: #F5F5F5; "
        "border-radius: 4px; "
    );
    rightLayout->addWidget(selectedDateLabel);

    eventList = new QListWidget();
    eventList->setContextMenuPolicy(Qt::CustomContextMenu);
    rightLayout->addWidget(eventList);

    addClassButton = new QPushButton("Add Recurring Class");
    addExamButton = new QPushButton("Add Test/Exam");
    rightLayout->addWidget(addClassButton);
    rightLayout->addWidget(addExamButton);

    return rightLayout;
}

void MainWindow::connectSignals() {
    connect(calendar, &QCalendarWidget::selectionChanged, this, [this]() {
        onDateSelected(calendar->selectedDate());
    });
    connect(todayButton, &QPushButton::clicked, this, [this](){
        calendar->setSelectedDate(QDate::currentDate());});

    connect(addClassButton, &QPushButton::clicked, this, &MainWindow::onAddClassClicked);
    connect(addExamButton, &QPushButton::clicked, this, &MainWindow::onAddExamClicked);
    connect(eventList, &QListWidget::customContextMenuRequested, this, &MainWindow::onContextMenu);
}

void MainWindow::onDateSelected(const QDate &date){
    selectedDateLabel->setText(date.toString("dddd, MMMM d, yyyy"));
    updateEventListForDate(date);
}

void MainWindow::updateEventListForDate(const QDate &date){
    eventList->clear();
    itemToClassId.clear();
    itemToEventId.clear();

    // Check if repository is initialized
    if (!repository) {
        eventList->addItem("Initializing...");
        return;
    }

    const Schedule* schedule = repository->getSchedule();
    if (!schedule){
        eventList->addItem("Loading...");
        return;
    }

    int itemCount = 0;
    itemCount += addClassesToList(date, schedule);
    itemCount += addEventsToList(date, schedule);

    if (itemCount == 0) {
        eventList->addItem("No events for this day!");
    }
}

void MainWindow::applyItemStyle(QListWidgetItem* item, const QString& backgroundColor, const QString& textColor){
    item->setBackground(QBrush(QColor(backgroundColor)));
    item->setForeground(QBrush(QColor(textColor)));
}

QListWidgetItem* MainWindow::createStyledClassItem(const RecurringClass& cls){
    QString eventStr = DisplayFormatter::formatClassRich(cls);
    QListWidgetItem *item = new QListWidgetItem(eventStr);

    applyItemStyle(item, Constants::EventColors::CLASS_BG_LIGHT, Constants::EventColors::PRIMARY_TEXT);

    return item;
}

QListWidgetItem* MainWindow::createStyledEventItem(const OneTimeEvent& evt){
    QString eventStr = DisplayFormatter::formatEventRich(evt);
    QListWidgetItem *item = new QListWidgetItem(eventStr);

    applyItemStyle(item, Constants::EventColors::EXAM_BG_LIGHT, Constants::EventColors::PRIMARY_TEXT);
    return item;    
}

int MainWindow::addClassesToList(const QDate& date, const Schedule* schedule) {
    QVector<const RecurringClass*> classes = schedule->getClassesForDate(date);

    for(const RecurringClass* cls : classes){
        QListWidgetItem* item = createStyledClassItem(*cls);
        eventList->addItem(item);
        itemToClassId[item] = cls->getId();
    }

    return classes.size();
}

int MainWindow::addEventsToList(const QDate& date, const Schedule* schedule) {
    QVector<const OneTimeEvent*> events = schedule->getOneTimeEventsForDate(date);

    for (const OneTimeEvent* evt : events){
        QListWidgetItem* item = createStyledEventItem(*evt);
        eventList->addItem(item);
        itemToEventId[item] = evt->getId();
    }
    return events.size();
}

void MainWindow::onAddClassClicked() {
    AddClassDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted){
        RecurringClass newClass;
        newClass.setName(dialog.getClassName());
        newClass.setDayOfWeek(dialog.getDayOfWeek());
        newClass.setStartTime(dialog.getStartTime());
        newClass.setEndTime(dialog.getEndTime());
        newClass.setRoom(dialog.getRoom());
        newClass.setTeacher(dialog.getTeacher());

        repository->addClass(newClass);
    }
}

void MainWindow::onAddExamClicked() {
    AddEventDialog dialog(this);

    if (dialog.exec() == QDialog::Accepted){
        OneTimeEvent newEvent;
        newEvent.setName(dialog.getEventName());
        newEvent.setDate(dialog.getDate());
        newEvent.setStartTime(dialog.getStartTime());
        newEvent.setEndTime(dialog.getEndTime());
        newEvent.setLocation(dialog.getLocation());
        newEvent.setNotes(dialog.getNotes());

        repository->addEvent(newEvent);
    }
}

void MainWindow::onContextMenu(const QPoint &pos) {
    QListWidgetItem *item = eventList->itemAt(pos);
    if (!item) return;

    if (itemToClassId.contains(item)) {
        showClassContextMenu(pos, itemToClassId[item]);
    } else if (itemToEventId.contains(item)) {
        showEventContextMenu(pos, itemToEventId[item]);
    }
}

void MainWindow::showClassContextMenu(const QPoint& pos, int classId) {
    QMenu menu(this);
    QAction *cancelAction = menu.addAction("Cancel this session");
    QAction *editAction = menu.addAction("Edit class");
    QAction *deleteAction = menu.addAction("Delete Class(all dates)");

    QAction *selected = menu.exec(eventList->mapToGlobal(pos));

    if (selected == cancelAction) {
        onCancelClassSession(classId);
    } else if (selected == deleteAction) {
        onDeleteClass(classId);
    } else if (selected == editAction) {
        onEditClass(classId);
    }
}

void MainWindow::showEventContextMenu(const QPoint& pos, int eventId) {
    QMenu menu(this);
    QAction *editAction = menu.addAction("Edit event");
    QAction *deleteAction = menu.addAction("Delete event");

    QAction *selected = menu.exec(eventList->mapToGlobal(pos));

    if (selected == editAction) {
        onEditEvent(eventId);
    } else if (selected == deleteAction) {
        onDeleteEvent(eventId);
    }
}

void MainWindow::onCancelClassSession(int classId){
    repository->cancelClassSession(classId, calendar->selectedDate());
}

void MainWindow::onDeleteClass(int classId){
    repository->deleteClass(classId);
}

void MainWindow::onEditClass(int classId){
    const Schedule* schedule = repository->getSchedule();
    const RecurringClass* classPtr = EntityFinder::findClassById(*schedule, classId);

    if (!classPtr) {
        QMessageBox::warning(this, "Error", "Class not found for editing");
        return;
    }

    RecurringClass classToEdit = *classPtr;

    AddClassDialog dialog(this);
    dialog.setWindowTitle("Edit Recurring Class");
    dialog.setClassName(classToEdit.getName());
    dialog.setDayOfWeek(classToEdit.getDayOfWeek());
    dialog.setStartTime(classToEdit.getStartTime());
    dialog.setEndTime(classToEdit.getEndTime());
    dialog.setRoom(classToEdit.getRoom());
    dialog.setTeacher(classToEdit.getTeacher());

    if (dialog.exec() == QDialog::Accepted){
        RecurringClass updatedClass;
        updatedClass.setName(dialog.getClassName());
        updatedClass.setDayOfWeek(dialog.getDayOfWeek());
        updatedClass.setStartTime(dialog.getStartTime());
        updatedClass.setEndTime(dialog.getEndTime());
        updatedClass.setRoom(dialog.getRoom());
        updatedClass.setTeacher(dialog.getTeacher());

        repository->updateClass(classId, updatedClass);
    }
}

void MainWindow::onEditEvent(int eventId){
    const Schedule* schedule = repository->getSchedule();
    const OneTimeEvent* eventPtr = EntityFinder::findEventById(*schedule, eventId);

    if (!eventPtr){
        QMessageBox::warning(this, "Error", "Event not found for editing");
        return;
    }

    OneTimeEvent eventToEdit = *eventPtr;

    AddEventDialog dialog(this);
    dialog.setWindowTitle("Edit Event");
    dialog.setEventName(eventToEdit.getName());
    dialog.setDate(eventToEdit.getDate());
    dialog.setStartTime(eventToEdit.getStartTime());
    dialog.setEndTime(eventToEdit.getEndTime());
    dialog.setLocation(eventToEdit.getLocation());
    dialog.setNotes(eventToEdit.getNotes());

    if (dialog.exec() == QDialog::Accepted){
        OneTimeEvent updatedEvent;
        updatedEvent.setName(dialog.getEventName());
        updatedEvent.setDate(dialog.getDate());
        updatedEvent.setStartTime(dialog.getStartTime());
        updatedEvent.setEndTime(dialog.getEndTime());
        updatedEvent.setLocation(dialog.getLocation());
        updatedEvent.setNotes(dialog.getNotes());

        repository->updateEvent(eventId, updatedEvent);
    }
}

void MainWindow::onDeleteEvent(int eventId){
    repository->deleteEvent(eventId);
}

void MainWindow::onRepositoryError(const QString& operation, const QString& error){
    QMessageBox::critical(this, "Operation Failed",
                         operation + " failed:\n" + error);
}

void MainWindow::onDataLoaded(){
    if (repository){
        calendar->setSchedule(repository->getSchedule());
    }
    updateEventListForDate(calendar->selectedDate());
}