#ifndef ADDEVENTDIALOG_H
#define ADDEVENTDIALOG_H
#include "basedialog.h"
#include <QLineEdit>
#include <QDateEdit>
#include <QTimeEdit>
#include <QTextEdit>
#include <QFormLayout>

class AddEventDialog : public BaseDialog{
    Q_OBJECT
public:
    explicit AddEventDialog(QWidget *parent = nullptr);

    QString getEventName()const;
    QDate getDate()const;
    QTime getStartTime()const;
    QTime getEndTime()const;
    QString getLocation()const;
    QString getNotes() const;

    void setEventName(const QString &name);
    void setDate(const QDate &date);
    void setStartTime(const QTime &time);
    void setEndTime(const QTime &time);
    void setLocation(const QString &location);
    void setNotes(const QString &notes);

private:
    QLineEdit *nameEdit;
    QDateEdit *dateEdit;
    QTimeEdit *startTimeEdit;
    QTimeEdit *endTimeEdit;
    QLineEdit *locationEdit;
    QTextEdit *notesEdit;

};
#endif // ADDEVENTDIALOG_H
