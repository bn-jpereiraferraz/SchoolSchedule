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
};

#endif // BASEDIALOG_H
