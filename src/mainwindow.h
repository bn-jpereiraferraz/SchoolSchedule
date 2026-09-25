#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QCalendarWidget>
#include <QListWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>

class Schedule;  // Forward declaration

class MainWindow : public QMainWindow {
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void onDateSelected(const QDate &date);
    void onAddClassClicked();
    void onAddExamClicked();

private:
    void setupUI();

    QCalendarWidget *calendar;
    QListWidget *eventList;
    QPushButton *addClassButton;
    QPushButton *addExamButton;
    QLabel *selectedDateLabel;
    Schedule *schedule;
    QString scheduleFilePath;

    void loadSchedule();
    void saveSchedule();
};

#endif // MAINWINDOW_H
