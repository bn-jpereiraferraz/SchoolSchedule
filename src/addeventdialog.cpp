#include "addeventdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

AddEventDialog::AddEventDialog(QWidget *parent) : BaseDialog(parent){
    setWindowTitle("Add Test/Exam");

    QVBoxLayout *mainLayout = new QVBoxLayout();
    formLayout = new QFormLayout;

    createNameField();
    createDateField();
    createTimeFields();
    createOptionalFields();

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setupKeyboardShortcuts();
    configureTabOrder();
    connectValidationSignals();

    resize(420, 380);

    updateOkButtonState();
}

QString AddEventDialog::getEventName()const{
    return nameEdit->text();
}

QDate AddEventDialog::getDate()const{
    return dateEdit->date();
}

QTime AddEventDialog::getStartTime()const{
    return startTimeEdit->time();
}

QTime AddEventDialog::getEndTime()const{
    return endTimeEdit->time();
}

QString AddEventDialog::getLocation()const{
    return locationEdit->text();
}

QString AddEventDialog::getNotes()const{
    return notesEdit->toPlainText();
}

void AddEventDialog::setEventName(const QString &name){
    nameEdit->setText(name);
}

void AddEventDialog::setDate(const QDate &date){
    dateEdit->setDate(date);
}

void AddEventDialog::setStartTime(const QTime &time){
    startTimeEdit->setTime(time);
}

void AddEventDialog::setEndTime(const QTime &time){
    endTimeEdit->setTime(time);
}

void AddEventDialog::setLocation(const QString &location){
    locationEdit->setText(location);
}

void AddEventDialog::setNotes(const QString &notes){
    notesEdit->setPlainText(notes);
}

void AddEventDialog::createNameField(){
    QHBoxLayout *nameRow = new QHBoxLayout();

    nameEdit = createRequiredLineEdit("e.g. German Exam");
    nameErrorLabel = createErrorLabel();
    QLabel *requiredMarker = createRequiredMarker();

    nameRow->addWidget(nameEdit);
    nameRow->addWidget(requiredMarker);

    formLayout->addRow("Event Name:", nameRow);
    formLayout->addRow("", nameErrorLabel);
}

void AddEventDialog::createDateField(){
    QHBoxLayout *dateRow = new QHBoxLayout();

    dateEdit = new QDateEdit(this);
    dateEdit->setCalendarPopup(true);
    dateEdit->setDate(QDate::currentDate());
    dateEdit->setDisplayFormat("yyyy-MM-dd");

    dateErrorLabel = createErrorLabel();
    QLabel *requiredMarker = createRequiredMarker();

    dateRow->addWidget(dateEdit);
    dateRow->addWidget(requiredMarker);

    formLayout->addRow("Date:", dateRow);
    formLayout->addRow("", dateErrorLabel);
}

void AddEventDialog::createTimeFields(){
    QHBoxLayout *timeRow = new QHBoxLayout();

    startTimeEdit = new QTimeEdit(this);
    configureTimeEdit(startTimeEdit, QTime(14, 0));

    endTimeEdit = new QTimeEdit(this);
    configureTimeEdit(endTimeEdit, QTime(16, 0));

    QLabel *startMarker = createRequiredMarker();
    QLabel *endMarker = createRequiredMarker();
    timeErrorLabel = createErrorLabel();

    timeRow->addWidget(new QLabel("Start:"));
    timeRow->addWidget(startTimeEdit);
    timeRow->addWidget(startMarker);
    timeRow->addWidget(new QLabel("End:"));
    timeRow->addWidget(endTimeEdit);
    timeRow->addWidget(endMarker);

    formLayout->addRow("Time:", timeRow);
    formLayout->addRow("", timeErrorLabel);
}

void AddEventDialog::createOptionalFields(){
    locationEdit = createOptionalLineEdit("e.g. Exam Room 405");
    notesEdit = new QTextEdit(this);
    notesEdit->setPlaceholderText("Additional Notes (optional):");
    notesEdit->setMaximumHeight(80);

    formLayout->addRow("Location:", locationEdit);
    formLayout->addRow("Notes:", notesEdit);
}

bool AddEventDialog::validateName(){
    QString name = nameEdit->text().trimmed();
    if (name.isEmpty()){
        showFieldError(nameEdit, nameErrorLabel, "Event name is required");
        return false;
    }
    clearFieldError(nameEdit, nameErrorLabel);
    return true;
}

bool AddEventDialog::validateDate(){
    QDate selected = dateEdit->date();
    if (selected < QDate::currentDate()){
        showFieldError(dateEdit, dateErrorLabel, "Event cannot be in the past");
        return false;
    }
    clearFieldError(dateEdit, dateErrorLabel);
    return true;
}

bool AddEventDialog::validateTimeRange(){
    QTime start = startTimeEdit->time();
    QTime end = endTimeEdit->time();

    if (start >= end){
        showFieldError(startTimeEdit, timeErrorLabel, "Start Time must be before End Time");
        return false;
    }
    clearFieldError(startTimeEdit, timeErrorLabel);
    return true;
}

bool AddEventDialog::validateAllFields(){
    bool nameValid = validateName();
    bool dateValid = validateDate();
    bool timeValid = validateTimeRange();
    return nameValid && dateValid && timeValid;
}

void AddEventDialog::connectValidationSignals(){
    connect(nameEdit, &QLineEdit::textChanged, this, &AddEventDialog::updateOkButtonState);
    connect(dateEdit, &QDateEdit::dateChanged, this, &AddEventDialog::updateOkButtonState);
    connect(startTimeEdit, &QTimeEdit::timeChanged, this, &AddEventDialog::updateOkButtonState);
    connect(endTimeEdit, &QTimeEdit::timeChanged, this, &AddEventDialog::updateOkButtonState);
}

void AddEventDialog::configureTabOrder(){
    setTabOrder(nameEdit, dateEdit);
    setTabOrder(dateEdit, startTimeEdit);
    setTabOrder(startTimeEdit, endTimeEdit);
    setTabOrder(endTimeEdit, locationEdit);
    setTabOrder(locationEdit, notesEdit);
}


void AddEventDialog::accept() {
    // All validation passed
    QDialog::accept();
}
