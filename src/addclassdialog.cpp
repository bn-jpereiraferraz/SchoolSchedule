#include "addclassdialog.h"
#include <QVBoxLayout>
#include <QDialogButtonBox>
#include <QLabel>

AddClassDialog::AddClassDialog(QWidget *parent)
    :BaseDialog(parent){

        setWindowTitle("Add Recurring Class");
        resize(400, 300);

        //Create form layout
        QFormLayout *formLayout = new QFormLayout();

        //Class name field
        nameEdit = new QLineEdit();
        formLayout->addRow("Class Name:", nameEdit);

        //Day of Week dropdown;
        dayComboBox = new QComboBox();
        dayComboBox->addItem("Monday", 1);
        dayComboBox->addItem("Tuesday", 2);
        dayComboBox->addItem("Wednesday", 3);
        dayComboBox->addItem("Thursday", 4);
        dayComboBox->addItem("Friday", 5);
        dayComboBox->addItem("Saturday", 6);
        dayComboBox->addItem("Sunday", 7);
        formLayout->addRow("Day of Week:", dayComboBox);

        //Start Time
        startTimeEdit = new QTimeEdit();
        configureTimeEdit(startTimeEdit, QTime(9, 0));
        formLayout->addRow("Start Time:", startTimeEdit);

        //End Time
        endTimeEdit = new QTimeEdit();
        configureTimeEdit(endTimeEdit, QTime(10, 30));
        formLayout->addRow("End Time:", endTimeEdit);

        //Room
        roomEdit = new QLineEdit();
        formLayout->addRow("Room(optional):", roomEdit);

        //Teacher
        teacherEdit = new QLineEdit();
        formLayout->addRow("Teacher(optional):", teacherEdit);

        //Buttons
        QDialogButtonBox *buttonBox = createButtonBox();

        //Main Layout
        QVBoxLayout *mainLayout = new QVBoxLayout(this);
        mainLayout->addLayout(formLayout);
        mainLayout->addWidget(buttonBox);

        setLayout(mainLayout);
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