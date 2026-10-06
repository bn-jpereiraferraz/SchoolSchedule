#ifndef ADDCLASSDIALOG_H
#define ADDCLASSDIALOG_H
#include "basedialog.h"
#include <QLineEdit>
#include <QComboBox>
#include <QTimeEdit>
#include <QFormLayout>

class AddClassDialog : public BaseDialog {
    Q_OBJECT
public:
    explicit AddClassDialog(QWidget *parent = nullptr);

    QString getClassName()const;
    int getDayOfWeek()const;
    QTime getStartTime()const;
    QTime getEndTime()const;
    QString getRoom()const;
    QString getTeacher()const;

    void setClassName(const QString &name);
    void setDayOfWeek(int day);
    void setStartTime(const QTime &time);
    void setEndTime(const QTime &time);
    void setRoom(const QString &room);
    void setTeacher(const QString &teacher);

public slots:
    void accept() override;

private:
    QLineEdit *nameEdit;
    QComboBox *dayComboBox;
    QTimeEdit *startTimeEdit;
    QTimeEdit *endTimeEdit;
    QLineEdit *roomEdit;
    QLineEdit *teacherEdit;
    QFormLayout *formLayout;
    //Field creation
    void createNameField();
    void createDayField();
    void createTimeFields();
    void createOptionalFields();
    //Validation
    bool validateName();
    bool validateTimeRange();
    bool validateAllFields()override;
    //Setup
    void connectValidationSignals();
    void configureTabOrder()override;
    //Error labels
    QLabel *nameErrorLabel;
    QLabel *timeErrorLabel;
};

#endif // ADDCLASSDIALOG_H
