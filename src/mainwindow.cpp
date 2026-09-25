#include "mainwindow.h"
#include "scheduledata.h"
#include <QWidget>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent) {
    setupUI();
    setWindowTitle("School Calendar");
    resize(900, 600);
}

MainWindow::~MainWindow() {
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

    // TODO: Load events for selected date
    eventList->clear();
    eventList->addItem("No events for this day (coming soon...)");
}

void MainWindow::onAddClassClicked() {
    // TODO: Open dialog to add recurring class
    eventList->addItem("Add Class feature coming soon...");
}

void MainWindow::onAddExamClicked() {
    // TODO: Open dialog to add exam
    eventList->addItem("Add Exam feature coming soon...");
}
