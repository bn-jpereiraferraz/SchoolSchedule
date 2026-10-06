#include "addclassdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

AddClassDialog::AddClassDialog(QWidget *parent) : BaseDialog(parent){
    setWindowTitle("Add Recurring Class");

    QVBoxLayout *mainLayout = new QVBoxLayout();
    formLayout = new QFormLayout();

    createNameField();
    createDayField();
    createTimeFields();
    createOptionalFields();

    mainLayout->addLayout(formLayout);
    mainLayout->addWidget(buttonBox);
    setLayout(mainLayout);

    setupKeyboardShortcuts();
    configureTabOrder();
    connectValidationSignals();

    resize(420, 350);
    
    updateOkButtonState();
}

QString AddClassDialog::getClassName()const{
    return nameEdit->text();
}

int AddClassDialog::getDayOfWeek()const{
    return dayComboBox->currentData().toInt();
}

QTime AddClassDialog::getStartTime()const{
    return startTimeEdit->time();
}

QTime AddClassDialog::getEndTime()const{
    return endTimeEdit->time();
}

QString AddClassDialog::getRoom()const{
    return roomEdit->text();
}

QString AddClassDialog::getTeacher()const{
    return teacherEdit->text();
}

void AddClassDialog::setClassName(const QString &name){
    nameEdit->setText(name);
}

void AddClassDialog::setDayOfWeek(int day){
    for (int i = 0; i < dayComboBox->count(); i++){
        if (dayComboBox->itemData(i).toInt() == day){
            dayComboBox->setCurrentIndex(i);
            break;
        }
    }
}

void AddClassDialog::setStartTime(const QTime &time){
    startTimeEdit->setTime(time);
}

void AddClassDialog::setEndTime(const QTime &time){
    endTimeEdit->setTime(time);
}

void AddClassDialog::setRoom(const QString &room){
    roomEdit->setText(room);
}

void AddClassDialog::setTeacher(const QString &teacher){
    teacherEdit->setText(teacher);
}

void AddClassDialog::createNameField(){
    QHBoxLayout *nameRow = new QHBoxLayout();

    nameEdit = createRequiredLineEdit("e.g. Mathematics 101");
    nameErrorLabel = createErrorLabel();
    QLabel *requiredMarker = createRequiredMarker();

    nameRow->addWidget(nameEdit);
    nameRow->addWidget(requiredMarker);

    formLayout->addRow("Class Name:", nameRow);
    formLayout->addRow("", nameErrorLabel);
}

void AddClassDialog::createDayField(){
    QHBoxLayout *dayRow = new QHBoxLayout();

    dayComboBox = new QComboBox(this);
    dayComboBox->addItem("Monday", 1);
    dayComboBox->addItem("Tuesday", 2);
    dayComboBox->addItem("Wednesday", 3);
    dayComboBox->addItem("Thursday", 4);
    dayComboBox->addItem("Friday", 5);
    dayComboBox->addItem("Saturday", 6);
    dayComboBox->addItem("Sunday", 7);

    QLabel *requiredMarker = createRequiredMarker();
    dayRow->addWidget(dayComboBox);
    dayRow->addWidget(requiredMarker);

    formLayout->addRow("Day of Week:", dayRow);
}

void AddClassDialog::createTimeFields(){
    QHBoxLayout *timeRow = new QHBoxLayout();

    startTimeEdit = new QTimeEdit(this);
    configureTimeEdit(startTimeEdit, QTime(9, 0));

    endTimeEdit = new QTimeEdit(this);
    configureTimeEdit(endTimeEdit, QTime(10, 30));

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

void AddClassDialog::createOptionalFields(){
    roomEdit = createOptionalLineEdit("e.g. Room 204");
    teacherEdit = createOptionalLineEdit("e.g. Prof. Smith");

    formLayout->addRow("Room (optional):", roomEdit);
    formLayout->addRow("Teacher (optional):", teacherEdit);
}

bool AddClassDialog::validateName(){
    QString name = nameEdit->text().trimmed();
    if(name.isEmpty()){
        showFieldError(nameEdit, nameErrorLabel, "Class name is required");
        return false;
    }
    clearFieldError(nameEdit, nameErrorLabel);
    return true;
}

bool AddClassDialog::validateTimeRange(){
    QTime start = startTimeEdit->time();
    QTime end = endTimeEdit->time();

    if (start >= end){
        showFieldError(startTimeEdit, timeErrorLabel, "Start time must be before end time");
        return false;
    }
    clearFieldError(startTimeEdit, timeErrorLabel);
    return true;
}

bool AddClassDialog::validateAllFields(){
    bool nameValid = validateName();
    bool timeValid = validateTimeRange();
    return nameValid && timeValid;
}

void AddClassDialog::connectValidationSignals(){
    connect(nameEdit, &QLineEdit::textChanged, this, &AddClassDialog::updateOkButtonState);
    connect(startTimeEdit, &QTimeEdit::timeChanged, this, &AddClassDialog::updateOkButtonState);
    connect(endTimeEdit, &QTimeEdit::timeChanged, this, &AddClassDialog::updateOkButtonState);
}

void AddClassDialog::configureTabOrder(){
    setTabOrder(nameEdit, dayComboBox);
    setTabOrder(dayComboBox, startTimeEdit);
    setTabOrder(startTimeEdit, endTimeEdit);
    setTabOrder(endTimeEdit, roomEdit);
    setTabOrder(roomEdit, teacherEdit);
}

void AddClassDialog::accept() {
    QDialog::accept();
}