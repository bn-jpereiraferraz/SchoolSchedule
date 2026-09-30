#include "addeventdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

AddEventDialog::AddEventDialog(QWidget *parent)
    :BaseDialog(parent){
        setWindowTitle("Add Test/Exam");
        resize(400, 350);

        //Create form Layout
        QFormLayout *formLayout = new QFormLayout();

        //Event name field
        nameEdit = new QLineEdit();
        formLayout->addRow("Event Name:", nameEdit);

        //Date Picker
        dateEdit = new QDateEdit();
        dateEdit->setCalendarPopup(true);
        dateEdit->setDate(QDate::currentDate());
        dateEdit->setDisplayFormat("yyyy-MM-dd");
        formLayout->addRow("Date:", dateEdit);

        //Start Time
        startTimeEdit = new QTimeEdit();
        configureTimeEdit(startTimeEdit, QTime(14, 0));
        formLayout->addRow("Start Time:", startTimeEdit);

        //End Time
        endTimeEdit = new QTimeEdit();
        configureTimeEdit(endTimeEdit, QTime(16, 0));
        formLayout->addRow("End Time:", endTimeEdit);

        //Location
        locationEdit = new QLineEdit();
        formLayout->addRow("Location:", locationEdit);

        //Notes multi line
        notesEdit = new QTextEdit();
        notesEdit->setMaximumHeight(80);
        formLayout->addRow("Notes:", notesEdit);

        //Button
        QDialogButtonBox *buttonBox = createButtonBox();

        //Main Layout
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(buttonBox);

        setLayout(mainLayout);
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

    void AddEventDialog::accept() {
        QString errorMsg;

        // Validate event name (required)
        if (!validateNotEmpty(nameEdit->text(), "Event name", errorMsg)) {
            showValidationError(errorMsg);
            nameEdit->setFocus();
            return;
        }

        // Validate time range
        if (!validateTimeRange(startTimeEdit->time(), endTimeEdit->time(), errorMsg)) {
            showValidationError(errorMsg);
            startTimeEdit->setFocus();
            return;
        }

        // All validation passed
        QDialog::accept();
    }
