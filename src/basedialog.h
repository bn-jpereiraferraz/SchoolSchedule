#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTimeEdit>

class BaseDialog : public QDialog {
    Q_OBJECT

protected:
    explicit BaseDialog(QWidget *parent = nullptr);

    // Helper methods for common setup patterns
    void configureTimeEdit(QTimeEdit *timeEdit, const QTime &defaultTime);
    QDialogButtonBox* createButtonBox();

    // Validation helpers
    bool validateNotEmpty(const QString& value, const QString& fieldName, QString& errorMsg) const;
    bool validateTimeRange(const QTime& startTime, const QTime& endTime, QString& errorMsg) const;
    void showValidationError(const QString& errorMsg) const;
};

#endif // BASEDIALOG_H
