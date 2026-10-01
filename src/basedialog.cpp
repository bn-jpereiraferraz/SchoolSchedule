#include "basedialog.h"
#include <QMessageBox>

BaseDialog::BaseDialog(QWidget *parent)
    : QDialog(parent) {
    buttonBox = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel);
    okButton = buttonBox->button(QDialogButtonBox::Ok);

    connect(buttonBox, &QDialogButtonBox::accepted, this, &BaseDialog::accept);
    connect(buttonBox, &QDialogButtonBox::rejected, this, &BaseDialog::reject);
}

void BaseDialog::configureTimeEdit(QTimeEdit *timeEdit, const QTime &defaultTime) {
    timeEdit->setDisplayFormat("HH:mm");
    timeEdit->setTime(defaultTime);
}

QDialogButtonBox* BaseDialog::createButtonBox() {
    QDialogButtonBox *buttonBox = new QDialogButtonBox(
        QDialogButtonBox::Ok | QDialogButtonBox::Cancel);

    okButton = buttonBox->button(QDialogButtonBox::Ok);

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

void BaseDialog::updateOkButtonState(){
    okButton->setEnabled(isFormValid());
}

bool BaseDialog::isFormValid()const{
    return const_cast<BaseDialog*>(this)->validateAllFields();
}

QLabel* BaseDialog::createRequiredMarker(){
    QLabel* marker = new QLabel("*");
    marker->setStyleSheet("color: #F44336; font-weight: bold; font-size: 14pt;");
    return marker;
}

QLabel* BaseDialog::createErrorLabel(){
    QLabel* label = new QLabel();
    label->setStyleSheet("color: #F44336; font-size: 9pt;");
    label->setWordWrap(true);
    label->hide();
    return label;
}

void BaseDialog::showFieldError(QWidget* field, QLabel* errorLabel, const QString& message){
    field->setStyleSheet("border: 2px solid #F44336; background-color: #FFEBEE;");
    errorLabel->setText(message);
    errorLabel->show();
}

void BaseDialog::clearFieldError(QWidget* field, QLabel* errorLabel){
    field->setStyleSheet("");
    errorLabel->hide();
}

QLineEdit* BaseDialog::createRequiredLineEdit(const QString& placeholder){
    QLineEdit* edit = new QLineEdit(this);
    edit->setPlaceholderText(placeholder);
    return edit;
}

QLineEdit* BaseDialog::createOptionalLineEdit(const QString& placeholder){
    QLineEdit* edit = new QLineEdit(this);
    edit->setPlaceholderText(placeholder);
    return edit;
}

void BaseDialog::setupKeyboardShortcuts(){
    QShortcut* submitShortcut = new QShortcut(QKeySequence("Ctrl+Return"), this);
    connect(submitShortcut, &QShortcut::activated, this, [this](){
        if (isFormValid()){
            accept();
        }
    });
}
