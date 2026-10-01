#ifndef BASEDIALOG_H
#define BASEDIALOG_H

#include <QDialog>
#include <QFormLayout>
#include <QDialogButtonBox>
#include <QLineEdit>
#include <QTimeEdit>
#include <QShortcut>
#include <QLabel>
#include <QPushButton>

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

    //Validation state management
    void updateOkButtonState();
    bool isFormValid()const;
    virtual bool validateAllFields() = 0;

    //Visual feedback helpers
    QLabel* createRequiredMarker();
    QLabel* createErrorLabel();
    void showFieldError(QWidget* field, QLabel* errorLabel, const QString& message);
    void clearFieldError(QWidget* field, QLabel* errorLabel);

    //Field Creation helpers
    QLineEdit* createRequiredLineEdit(const QString& placeholder);
    QLineEdit* createOptionalLineEdit(const QString& placeholder);

    //Keyboard and tab
    void setupKeyboardShortcuts();
    virtual void configureTabOrder() = 0;

    //Members
    QDialogButtonBox* buttonBox;

private:
    QPushButton* okButton;
};

#endif // BASEDIALOG_H
