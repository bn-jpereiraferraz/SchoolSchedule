#include "basedialog.h"

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
