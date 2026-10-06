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
#include <QComboBox>

MainWindow::MainWindow(QWidget *parent)
    :QMainWindow(parent){
        setupUI();
        eventListBuilder = new EventListBuilder(eventList, itemToClassId, itemToEventId);
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
    rightLayout->addLayout(createSearchFilterBar());

    eventList = new QListWidget();
    eventList->setContextMenuPolicy(Qt::CustomContextMenu);
    rightLayout->addWidget(eventList);

    addClassButton = new QPushButton("Add Recurring Class");
    addExamButton = new QPushButton("Add Test/Exam");
    rightLayout->addWidget(addClassButton);
    rightLayout->addWidget(addExamButton);

    return rightLayout;
}

QLineEdit* MainWindow::createQuickDateJumpInput() {
    QLineEdit* input = new QLineEdit(this);
    input->setPlaceholderText("Jump to date... (e.g 'Dec 25', 'next Monday')");
    input->setMaximumHeight(32);
    return input;
}

QComboBox* MainWindow::createFilterTypeCombo() {
    QComboBox* combo = new QComboBox(this);
    combo->addItem("All Events");
    combo->addItem("Classes Only");
    combo->addItem("Exams Only");
    combo->setMaximumWidth(140);
    combo->setMaximumHeight(32);
    return combo;
}

QLineEdit* MainWindow::createSearchByNameInput() {
    QLineEdit* input = new QLineEdit(this);
    input->setPlaceholderText("Search event name...");
    input->setMaximumHeight(32);
    input->setMaximumWidth(180);
    return input;
}

QHBoxLayout* MainWindow::createSearchFilterBar() {
    QHBoxLayout *searchLayout = new QHBoxLayout();

    quickDateJumpInput = createQuickDateJumpInput();
    filterTypeComboBox = createFilterTypeCombo();
    searchByNameInput = createSearchByNameInput();

    searchLayout->addWidget(quickDateJumpInput, 2);
    searchLayout->addWidget(filterTypeComboBox, 1);
    searchLayout->addWidget(searchByNameInput, 1);

    return searchLayout;
}

QDate MainWindow::parseRelativeDate(const QString& lower, const QDate& today) {
    if (lower == "today") return today;
    if (lower == "tomorrow") return today.addDays(1);
    if (lower == "yesterday") return today.addDays(-1);
    return QDate(); // Invalid
}

QDate MainWindow::parseWeekdayDate(const QString& lower, const QDate& today) {
    QStringList dayNames = {"monday", "tuesday", "wednesday", "thursday", "friday", "saturday", "sunday"};
    for (int i = 0; i < dayNames.size(); i++) {
        if (lower.contains(dayNames[i])) {
            int targetDayOfWeek = i + 1;
            int daysAhead = (targetDayOfWeek - today.dayOfWeek()) % 7;
            if (daysAhead <= 0) daysAhead += 7;
            return today.addDays(daysAhead);
        }
    }
    return QDate(); // Invalid
}

QDate MainWindow::parseFormattedDate(const QString& input, const QDate& today) {
    QStringList dateFormats = {"MMM d", "M/d", "MMMM d", "MM/dd", "d MMM", "d MMMM"};
    for (const QString& format : dateFormats) {
        QDate parsed = QDate::fromString(input, format);
        if (parsed.isValid()) {
            if (parsed < today) {
                parsed = parsed.addYears(1);
            }
            return parsed;
        }
    }
    return QDate(); // Invalid
}

QDate MainWindow::parseNaturalLanguageDate(const QString& input) {
    QString lower = input.trimmed().toLower();
    QDate today = QDate::currentDate();

    QDate result = parseRelativeDate(lower, today);
    if (result.isValid()) return result;

    result = parseWeekdayDate(lower, today);
    if (result.isValid()) return result;

    result = parseFormattedDate(input, today);
    if (result.isValid()) return result;

    return QDate(); // Invalid
}

void MainWindow::rebuildFilteredEventList() {
    eventListBuilder->clearList();

    int itemCount = 0;
    itemCount += eventListBuilder->addFilteredClasses(currentDateClasses, filterState);
    itemCount += eventListBuilder->addFilteredEvents(currentDateEvents, filterState);

    if (itemCount == 0) {
        eventListBuilder->showEmptyMessage();
    }
}

void MainWindow::updateFilterState(){
    filterState.setFilterType((FilterState::FilterType)filterTypeComboBox->currentIndex());
    filterState.setSearchText(searchByNameInput->text());
    rebuildFilteredEventList();
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

    //Quick Date Jump 
    connect(quickDateJumpInput, &QLineEdit::returnPressed, this, [this](){
        QDate parsedDate = parseNaturalLanguageDate(quickDateJumpInput->text());
        if (parsedDate.isValid()){
            quickDateJumpInput->clear();
            calendar->setSelectedDate(parsedDate);
        }else{
            QMessageBox::warning(this, "Invalid Date", "Could not parse date. Try 'Dec 25', 'next Monday'm or 'tomorrow'");
            quickDateJumpInput->selectAll();
        }
    });

    //Filter Type Changed
    connect(filterTypeComboBox, QOverload<int>::of(&QComboBox::currentIndexChanged), this, &MainWindow::updateFilterState);

    //Search Text Changed
    connect(searchByNameInput, &QLineEdit::textChanged, this, &MainWindow::updateFilterState);
    
}

void MainWindow::onDateSelected(const QDate &date){
    selectedDateLabel->setText(date.toString("dddd, MMMM d, yyyy"));
    updateEventListForDate(date);
}

void MainWindow::updateEventListForDate(const QDate &date){
    //Clear caches
    currentDateClasses.clear();
    currentDateEvents.clear();
    itemToClassId.clear();
    itemToEventId.clear();

    //Reset search/filter UI
    searchByNameInput->clear();
    filterTypeComboBox->setCurrentIndex(0);
    filterState.setFilterType(FilterState::All);
    filterState.setSearchText("");

    //Check if repository is initialized
    if (!repository){
        eventList->clear();
        eventList->addItem("Initializing...");
        return;
    }

    const Schedule* schedule = repository->getSchedule();
    if (!schedule){
        eventList->clear();
        eventList->addItem("Loading...");
        return;
    }

    //Cache the unfiltered results
    currentDateClasses = schedule->getClassesForDate(date);
    currentDateEvents = schedule->getOneTimeEventsForDate(date);

    //Apply filters and build List
    rebuildFilteredEventList();
}

void MainWindow::onAddClassClicked() {
    AddClassDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        repository->addClass(createClassFromDialog(dialog));
    }
}

void MainWindow::onAddExamClicked() {
    AddEventDialog dialog(this);
    if (dialog.exec() == QDialog::Accepted) {
        repository->addEvent(createEventFromDialog(dialog));
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

    AddClassDialog dialog(this);
    populateDialogFromClass(dialog, *classPtr);

    if (dialog.exec() == QDialog::Accepted) {
        repository->updateClass(classId, createClassFromDialog(dialog));
    }
}

void MainWindow::onEditEvent(int eventId){
    const Schedule* schedule = repository->getSchedule();
    const OneTimeEvent* eventPtr = EntityFinder::findEventById(*schedule, eventId);

    if (!eventPtr) {
        QMessageBox::warning(this, "Error", "Event not found for editing");
        return;
    }

    AddEventDialog dialog(this);
    populateDialogFromEvent(dialog, *eventPtr);

    if (dialog.exec() == QDialog::Accepted) {
        repository->updateEvent(eventId, createEventFromDialog(dialog));
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
RecurringClass MainWindow::createClassFromDialog(const AddClassDialog& dialog) {
    RecurringClass cls;
    cls.setName(dialog.getClassName());
    cls.setDayOfWeek(dialog.getDayOfWeek());
    cls.setStartTime(dialog.getStartTime());
    cls.setEndTime(dialog.getEndTime());
    cls.setRoom(dialog.getRoom());
    cls.setTeacher(dialog.getTeacher());
    return cls;
}

OneTimeEvent MainWindow::createEventFromDialog(const AddEventDialog& dialog) {
    OneTimeEvent evt;
    evt.setName(dialog.getEventName());
    evt.setDate(dialog.getDate());
    evt.setStartTime(dialog.getStartTime());
    evt.setEndTime(dialog.getEndTime());
    evt.setLocation(dialog.getLocation());
    evt.setNotes(dialog.getNotes());
    return evt;
}

void MainWindow::populateDialogFromClass(AddClassDialog& dialog, const RecurringClass& cls) {
    dialog.setWindowTitle("Edit Recurring Class");
    dialog.setClassName(cls.getName());
    dialog.setDayOfWeek(cls.getDayOfWeek());
    dialog.setStartTime(cls.getStartTime());
    dialog.setEndTime(cls.getEndTime());
    dialog.setRoom(cls.getRoom());
    dialog.setTeacher(cls.getTeacher());
}

void MainWindow::populateDialogFromEvent(AddEventDialog& dialog, const OneTimeEvent& evt) {
    dialog.setWindowTitle("Edit Event");
    dialog.setEventName(evt.getName());
    dialog.setDate(evt.getDate());
    dialog.setStartTime(evt.getStartTime());
    dialog.setEndTime(evt.getEndTime());
    dialog.setLocation(evt.getLocation());
    dialog.setNotes(evt.getNotes());
}
