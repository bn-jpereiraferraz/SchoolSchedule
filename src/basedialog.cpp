#include "basedialog.h"
#include <QMessageBox>

BaseDialog::BaseDialog(QWidget *parent)
    : QDialog(parent) {
}

void BaseDialog::configureTimeEdit(QTimeEdit *timeEdit, const QTime &defaultTime) {
    timeEdit->setDisplayFormat("HH:mm");
    timeEdit->setTime(defaultTime);
}

QDialogButtonBox* BaseDialog::createButtonBox() {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &QDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &QDialog::reject);

    return buttonBox;
}

bool BaseDialog::validateNotEmpty(const QString& value, const QString& fieldName, QString& errorMsg) const {
    if (value.trimmed().isEmpty()) {
        errorMsg = fieldName + " cannot be empty.";
        return false;
    }
    return true;
}

bool BaseDialog::validateTimeRange(const QTime& startTime, const QTime& endTime, QString& errorMsg) const {
    if (startTime >= endTime) {
        errorMsg = "Start time must be before end time.";
        return false;
    }
    return true;
}

void BaseDialog::showValidationError(const QString& errorMsg) const {
    QMessageBox::warning(const_cast<BaseDialog*>(this), "Validation Error", errorMsg);
}
